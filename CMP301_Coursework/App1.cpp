
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
	terrain = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext(),1000);
	water = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext(),2400);
	//water = new TessellationPlane(renderer->getDevice(), renderer->getDeviceContext(), 100, 100);

	textureMgr->loadTexture(L"sand", L"res/sand.png");
	textureMgr->loadTexture(L"terrainHeight", L"res/heightmap.png");
	textureMgr->loadTexture(L"water", L"res/water2.jpg");
	textureMgr->loadTexture(L"grass", L"res/grass.png");
	textureMgr->loadTexture(L"waterMap1", L"res/waterheightmap/0.png");
	textureMgr->loadTexture(L"waterMap2", L"res/waterheightmap/1.png");
	lightShader = new LightShader(renderer->getDevice(), hwnd);
	terrainShader = new TerrainShader(renderer->getDevice(), hwnd);
	waterShader = new WaterShader(renderer->getDevice(), hwnd);


	// Configure point light.
	light1 = new Light();
	light1->setAmbientColour(0.0f, 0.0f, 0.0f, 1.0f);
	light1->setDiffuseColour(1.0f, 0.0f, 0.0f, 1.0f);
	light1->setPosition(40.0f, 7.5f, 40.0f);

	light2 = new Light();
	light2->setAmbientColour(0.0f, 0.0f, 0.0f, 1.0f);
	light2->setDiffuseColour(0.0f, 0.0f, 1.0f, 1.0f);
	light2->setPosition(0.0f, 10000.0f, 00.0f);

	//directional light
	light3 = new Light();
	light3->setAmbientColour(0.2f, 0.2f, 0.2f, 1.0f);
	light3->setDiffuseColour(0.5f, 0.5f, 0.5f, 1.0f);
	light3->setDirection(0.f, -1.f, 0.f);

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
	elapsedTime += timer->getTime();
	result = render();

	if (!result)
	{
		return false;
	}

	return true;
}

bool App1::render()
{
	firstPass();

	return true;
}

void App1::firstPass()
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

	//render the terrain using the terrain shader I created
	terrain->sendData(renderer->getDeviceContext());
	terrainShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixScaling(0.1f, 1.f, 0.1f), viewMatrix, projectionMatrix, textureMgr->getTexture(L"sand"), textureMgr->getTexture(L"grass"), textureMgr->getTexture(L"terrainHeight"), light1, light2, light3);
	terrainShader->render(renderer->getDeviceContext(), terrain->getIndexCount());

	//render the water using the water shader I created
	water->sendData(renderer->getDeviceContext());
	waterShader->setShaderParameters(renderer->getDeviceContext(), (worldMatrix * XMMatrixScaling(0.25f, 1.f, 0.25f)) + XMMatrixTranslation(-400, 2, -400), viewMatrix, projectionMatrix, textureMgr->getTexture(L"water"), textureMgr->getTexture(L"waterMap1"), textureMgr->getTexture(L"waterMap2"), light1, light2, light3, elapsedTime);
	waterShader->render(renderer->getDeviceContext(), water->getIndexCount());

	// Render GUI
	gui();

	// Swap the buffers
	renderer->endScene();

}

void App1::finalPass()
{
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

