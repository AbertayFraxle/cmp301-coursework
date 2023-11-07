
#include "App1.h"

App1::App1()
{
	terrain = nullptr;
	lightShader = nullptr;
	terrainShader = nullptr;
}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in, bool VSYNC, bool FULL_SCREEN)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	// Create Mesh object and shader object
	terrain = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext());
	water = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext());
	textureMgr->loadTexture(L"sand", L"res/sand.png");
	textureMgr->loadTexture(L"terrainHeight", L"res/heightmap.png");
	textureMgr->loadTexture(L"water", L"res/water.jpg");
	textureMgr->loadTexture(L"grass", L"res/grass.png");
	lightShader = new LightShader(renderer->getDevice(), hwnd);
	terrainShader = new TerrainShader(renderer->getDevice(), hwnd);


	// Configure point light.
	light1 = new Light();
	light1->setAmbientColour(0.0f, 0.0f, 0.0f, 1.0f);
	light1->setDiffuseColour(1.0f, 0.0f, 0.0f, 1.0f);
	light1->setPosition(70.0f, 10.0f, 50.0f);

	light2 = new Light();
	light2->setAmbientColour(0.0f, 0.0f, 0.0f, 1.0f);
	light2->setDiffuseColour(0.0f, 0.0f, 1.0f, 1.0f);
	light2->setPosition(20.0f, 10.0f, 50.0f);

	//
	light3 = new Light();
	light3->setAmbientColour(0.2f, 0.2f, 0.2f, 1.0f);
	light3->setDiffuseColour(0.5f, 0.5f, 0.5f, 1.0f);
	light3->setDirection(1.f, -1.f, 0.f);

}


App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	// Release the Direct3D object.
	if (terrain)
	{
		delete terrain;
		terrain = 0;
	}

	if (water)
	{
		delete water;
		water = 0;
	}

	if (lightShader)
	{
		delete lightShader;
		lightShader = 0;
	}

	if (terrainShader)
	{
		delete terrainShader;
		terrainShader = 0;
	}
}


bool App1::frame()
{
	bool result;

	result = BaseApplication::frame();
	if (!result)
	{
		return false;
	}
	
	// Render the graphics.
	result = render();
	if (!result)
	{
		return false;
	}

	return true;
}

bool App1::render()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	// Clear the scene. (default blue colour)
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

	// Generate the view matrix based on the camera's position.
	camera->update();

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();

	// Send geometry data, set shader parameters, render object with shader
	terrain->sendData(renderer->getDeviceContext());
	terrainShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"sand"), textureMgr->getTexture(L"grass"),textureMgr->getTexture(L"terrainHeight") ,light1,light2,light3);
	terrainShader->render(renderer->getDeviceContext(), terrain->getIndexCount());

	worldMatrix = worldMatrix * XMMatrixScaling(8, 8, 8);
	worldMatrix = worldMatrix + XMMatrixTranslation(-400, 2, -400);

	water->sendData(renderer->getDeviceContext());
	lightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"water"),light1, light2, light3);
	lightShader->render(renderer->getDeviceContext(), water->getIndexCount());

	// Render GUI
	gui();

	// Swap the buffers
	renderer->endScene();

	return true;
}

void App1::gui()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	// Build UI
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);
	ImGui::Text("Press E to raise camera \nto see the plane being rendered");

	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

