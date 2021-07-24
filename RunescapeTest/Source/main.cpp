#include <Super Synthesis.h>

#include <Graphics/Camera.h>
#include <Graphics/Renderer.h>

#include <Graphics/Shaders/Shader.h>

#include <Input/InputManager.h>

#include <Resources/ResourceManager.h>

#include <EngineTypeDefs.h>

constexpr r32 CAMERA_ROTATE_SPEED = 5.0f;
constexpr r32 CAMERA_MOVE_SPEED = 5.0f;

SSE::Object* gObject = nullptr;

bool appInit()
{
	ResourceHandle shader = SSE::ResourceManager::loadShader({ "frag.spv","vert.spv" }, { SSE::Graphics::ShaderModuleType::SMT_FRAGMENT, SSE::Graphics::ShaderModuleType::SMT_VERTEX });
	ResourceHandle modelHandle = SSE::ResourceManager::loadModel("untitled.obj");
	ResourceHandle textureHandle = SSE::ResourceManager::loadTexture("grass.bmp");
	gObject = new SSE::Object;
	gObject->create(modelHandle, textureHandle);
	SSE::gCamera.create(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	SSE::Graphics::Renderer::gRenderer.create();
	ResourceHandle mainPipeline = SSE::Graphics::Renderer::gRenderer.createPipeline(shader);
	SSE::Graphics::Renderer::gRenderer.setPipeline(mainPipeline);
	SSE::Graphics::Renderer::gRenderer.setCamera(&SSE::gCamera);
	SSE::Graphics::Renderer::gRenderer.clearColor = { 0.0, 0.0f, 0.0f, 1.0f };

	gObject->transform(glm::vec3(0.0f), glm::vec3(1.0f), glm::mat4(1.0f));
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

	SSE::Graphics::Renderer::gRenderer.renderObject(*gObject);
}

void appCleanup()
{
	gObject->destroy();
	delete gObject;
}