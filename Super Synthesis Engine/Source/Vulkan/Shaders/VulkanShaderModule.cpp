#include "Vulkan/Shaders/VulkanShaderModule.h"

#include "Logging/Logger.h"

#include "Vulkan/Devices/VulkanDeviceManager.h"

namespace SSE
{
	extern Logger gLogger;

	namespace Vulkan
	{
		bool VulkanShaderModule::create(const std::vector<byte>& code, Graphics::ShaderModuleType _moduleType)
		{
			VkShaderModuleCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			createInfo.codeSize = code.size();
			createInfo.pCode = (u32*) code.data();

			if (vkCreateShaderModule(LOGICAL_DEVICE_DEVICE, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) 
			{
				GLOG_CRITICAL("Failed to create shader module.");
				return false;
			}

#pragma message("TODO: Move getting shader reflection data to offline shader compilation and store in shader metadata file")
			if (spvReflectCreateShaderModule(code.size(), (const void*)code.data(), &reflectionModule) != SPV_REFLECT_RESULT_SUCCESS)
			{
				GLOG_CRITICAL("Failed to create reflection module for shader.");
				destroy();
				return false;
			}

			moduleType = _moduleType;

			return true;
		}

		void VulkanShaderModule::destroy()
		{
			vkDestroyShaderModule(LOGICAL_DEVICE_DEVICE, shaderModule, nullptr);
			moduleType = Graphics::ShaderModuleType::SMT_NONE;
		}

		Graphics::ShaderModuleType VulkanShaderModule::getModuleType() const
		{
			return moduleType;
		}

		VkShaderModule VulkanShaderModule::getModule() const
		{
			return shaderModule;
		}

		VertexFormat VulkanShaderModule::getInputFormat() const
		{
			u32 inputCount = 0;
			if (spvReflectEnumerateInputVariables(&reflectionModule, &inputCount, nullptr) != SPV_REFLECT_RESULT_SUCCESS)
			{
				GLOG_CRITICAL("Failed to get input variable count for shader module");
				return {};
			}

			std::vector<SpvReflectInterfaceVariable*> variables(inputCount);
			if (spvReflectEnumerateInputVariables(&reflectionModule, &inputCount, variables.data()) != SPV_REFLECT_RESULT_SUCCESS)
			{
				GLOG_CRITICAL("Failed to get input variables for shader module");
				return {};
			}

			std::vector<VertexAspectType> vertexAspectTypes(inputCount);
#pragma message("NOTE: This is super hacky but there's no way currently to indicate the usage of a given vertex attribute")
			for (const auto& variable : variables)
			{
				switch (variable->location)
				{
				case 0:
				{
					//position
					vertexAspectTypes[0] = variable->numeric.vector.component_count == 2 ? VA_POS2F : VA_POS3F;
					break;
				}
				case 1:
				{
					//texture coords
					vertexAspectTypes[1] = variable->numeric.vector.component_count == 1 ? VA_TEX1F :
											(variable->numeric.vector.component_count == 2 ? VA_TEX2F :
											VA_TEX3F);
					break;
				}
				default:
				{
					vertexAspectTypes[variable->location] = VA_NONE;
				}
				}
			}

			VertexFormat result;
			result.create(vertexAspectTypes);
			return result;
		}

		const char* VulkanShaderModule::getEntryPoint() const
		{
			return reflectionModule.entry_point_name;
		}

		std::vector<DescriptorSetLayoutData> VulkanShaderModule::getDescriptorSetLayoutData() const
		{
			u32 descriptorCount = 0;
			spvReflectEnumerateDescriptorSets(&reflectionModule, &descriptorCount, nullptr);

			std::vector<SpvReflectDescriptorSet*> sets(descriptorCount);
			spvReflectEnumerateDescriptorSets(&reflectionModule, &descriptorCount, sets.data());

			std::vector<DescriptorSetLayoutData> setLayouts(sets.size(), DescriptorSetLayoutData{});
			for (st setNum = 0; setNum < sets.size(); ++setNum)
			{
				const SpvReflectDescriptorSet& reflectedSet = *(sets[setNum]);

				DescriptorSetLayoutData* layout = &setLayouts[setNum];
				layout->bindings.resize(reflectedSet.binding_count);

				for (u32 bindingNum = 0; bindingNum < reflectedSet.binding_count; ++bindingNum)
				{
					const SpvReflectDescriptorBinding& reflectedBinding = *(reflectedSet.bindings[bindingNum]);

					VkDescriptorSetLayoutBinding* layoutBinding = &layout->bindings[bindingNum];
					layoutBinding->binding = reflectedBinding.binding;
					layoutBinding->descriptorType = (VkDescriptorType)reflectedBinding.descriptor_type;
					layoutBinding->descriptorCount = 1;
					for (u32 dim = 0; dim < reflectedBinding.array.dims_count; ++dim)
					{
						layoutBinding->descriptorCount *= reflectedBinding.array.dims[dim];
					}
					layoutBinding->stageFlags = (VkShaderStageFlagBits)reflectionModule.shader_stage;
				}
				layout->set_number = reflectedSet.set;
				layout->create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
				layout->create_info.bindingCount = reflectedSet.binding_count;
			}

			return setLayouts;
		}
	}
}