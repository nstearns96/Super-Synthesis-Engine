#include <iostream>

#include <Graphics/Camera.h>
#include <Graphics/Font.h>
#include <Graphics/Renderer.h>
#include <Graphics/Texture2D.h>

#include <Input/InputManager.h>

#include <Resources/ResourceManager.h>

#include "World/Chunk.h"

#include "World/World.h"

constexpr r32 CAMERA_ROTATE_SPEED = 5.0f;
constexpr r32 CAMERA_MOVE_SPEED = 5.0f;
constexpr r32 REACH = 5.0f;

World gWorld;

bool appInit()
{
	ResourceHandle shader = SSE::ResourceManager::loadShader({ "frag.spv","vert.spv" }, { SSE::Graphics::ShaderModuleType::SMT_FRAGMENT, SSE::Graphics::ShaderModuleType::SMT_VERTEX });
	SSE::ResourceManager::loadTexture("block_textures.bmp");

	for (st c = 0; c < 9; ++c)
	{
		gWorld.generateChunk({c/3 - 1, c%3 - 1});
	}

	SSE::gCamera.create(glm::vec3(0.0f, 0.0f, 72.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	SSE::Graphics::Renderer::gRenderer.create();
	ResourceHandle mainPipeline = SSE::Graphics::Renderer::gRenderer.createPipeline(shader);
	SSE::Graphics::Renderer::gRenderer.setPipeline(mainPipeline);
	SSE::Graphics::Renderer::gRenderer.setCamera(&SSE::gCamera);
	SSE::Graphics::Renderer::gRenderer.clearColor = { 0.494f, 0.753f, 0.933f, 1.0f };
	return true;
}

void appUpdate(r32 deltaTime)
{
	glm::ivec2 mouseDelta = SSE::Input::InputManager::getMouseDelta();
	SSE::gCamera.rotateByAxes({ mouseDelta.x * CAMERA_ROTATE_SPEED * deltaTime, mouseDelta.y * CAMERA_ROTATE_SPEED * deltaTime });

	if (SSE::Input::InputManager::checkKeyState(SDL_SCANCODE_W, SSE::Input::KS_IS_DOWN))
	{
		SSE::gCamera.translate(SSE::gCamera.getForwardVector() * CAMERA_MOVE_SPEED * deltaTime);
	}
	if (SSE::Input::InputManager::checkKeyState(SDL_SCANCODE_S, SSE::Input::KS_IS_DOWN))
	{
		SSE::gCamera.translate(-SSE::gCamera.getForwardVector() * CAMERA_MOVE_SPEED * deltaTime);
	}
	if (SSE::Input::InputManager::checkKeyState(SDL_SCANCODE_A, SSE::Input::KS_IS_DOWN))
	{
		SSE::gCamera.translate(-SSE::gCamera.getRightVector() * CAMERA_MOVE_SPEED * deltaTime);
	}
	if (SSE::Input::InputManager::checkKeyState(SDL_SCANCODE_D, SSE::Input::KS_IS_DOWN))
	{
		SSE::gCamera.translate(SSE::gCamera.getRightVector() * CAMERA_MOVE_SPEED * deltaTime);
	}
	if (SSE::Input::InputManager::checkMouseKeyState(SDL_BUTTON_LEFT, SSE::Input::KS_PRESSED))
	{
		glm::ivec3 blockPos;
		BlockFace blockFace;
		if (gWorld.getFirstBlock(SSE::gCamera.getPosition(), SSE::gCamera.getForwardVector(), REACH, blockPos, blockFace))
		{
			gWorld.breakBlock(blockPos);
		}
	}
	if (SSE::Input::InputManager::checkMouseKeyState(SDL_BUTTON_RIGHT, SSE::Input::KS_PRESSED))
	{
		glm::ivec3 blockPos;
		BlockFace blockFace;
		if (gWorld.getFirstBlock(SSE::gCamera.getPosition(), SSE::gCamera.getForwardVector(), REACH, blockPos, blockFace))
		{
			switch (blockFace)
			{
			case BF_POS_X:
			case BF_NEG_X:
			{
				blockPos.x += blockFace == BF_POS_X ? 1 : -1;
				break;
			}
			case BF_POS_Y:
			case BF_NEG_Y:
			{
				blockPos.y += blockFace == BF_POS_Y ? 1 : -1;
				break;
			}
			case BF_POS_Z:
			case BF_NEG_Z:
			{
				blockPos.z += blockFace == BF_POS_Z ? 1 : -1;
				break;
			}
			} 
			gWorld.setBlock(blockPos, 1);
		}
	}

	gWorld.update();
	gWorld.render();
}

void appCleanup()
{
	gWorld.destroy();
}