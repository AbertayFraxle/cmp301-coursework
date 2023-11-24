
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

	cube = new CubeMesh(renderer->getDevice(), renderer->getDeviceContext());
	//water = new TessellationPlane(renderer->getDevice(), renderer->getDeviceContext(), 100, 100);

	playerView = new OrthoMesh(renderer->getDevice(),renderer->getDeviceContext(), 1024, 1024);
	textureMgr->loadTexture(L"sand", L"res/sand.png");
	textureMgr->loadTexture(L"terrainHeight", L"res/heightmap.png");
	textureMgr->loadTexture(L"water", L"res/water2.jpg");
	textureMgr->loadTexture(L"grass", L"res/grass.png");
	textureMgr->loadTexture(L"waterMap1", L"res/waterheightmap/0.png");
	textureMgr->loadTexture(L"waterMap2", L"res/waterheightmap/1.png");

	lightShader = new LightShader(renderer->getDevice(), hwnd);
	terrainShader = new TerrainShader(renderer->getDevice(), hwnd);
	waterShader = new WaterShader(renderer->getDevice(), hwnd);
	depthShader = new DepthShader(renderer->getDevice(), hwnd);
	textureShader = new TextureShader(renderer->getDevice(), hwnd);
	terrainDepthShader = new TerrainDepthShader(renderer->getDevice(), hwnd);

	// Variables for defining shadow map
	int shadowmapWidth = 2048;
	int shadowmapHeight = 2048;
	int sceneWidth = 200;
	int sceneHeight = 200;

	// This is your shadow map

	shadowMap = new ShadowMap(renderer->getDevice(), shadowmapWidth, shadowmapHeight);
	

	// Configure point light.
	light1 = new Light();
	light1->setAmbientColour(0.0f, 0.0f, 0.0f, 1.0f);
	light1->setDiffuseColour(1.0f, 0.0f, 0.0f, 1.0f);
	light1->setPosition(40.0f, 7.5f, 40.0f);

	light2 = new Light();
	light2->setAmbientColour(0.0f, 0.0f, 0.0f, 1.0f);
	light2->setDiffuseColour(1.0f, 0.5f, 0.0f, 1.0f);
	light2->setPosition(debuglightPos[0], debuglightPos[1], debuglightPos[2]);

	//directional light
	light3 = new Light();
	light3->setAmbientColour(0.05f, 0.05f, 0.05f, 1.0f);
	light3->setDiffuseColour(0.3f, 0.3f, 0.3f, 1.0f);
	light3->setDirection(1.f, -1.f, 1.f);
	light3->setPosition(0.f,30.f,0.f);

	light3->generateOrthoMatrix(sceneWidth,sceneHeight,0.1,200);

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
	light2->setPosition(debuglightPos[0], debuglightPos[1], debuglightPos[2]);
	result = render();

	if (!result)
	{
		return false;
	}

	return true;
}

bool App1::render()
{
	depthPass();
	firstPass();

	return true;
}

void App1::depthPass()
{
	shadowMap->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());

	light3->generateViewMatrix();

	XMMATRIX lightViewMatrix = light3->getViewMatrix();
	XMMATRIX lightProjectionMatrix = light3->getOrthoMatrix();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	
	terrain->sendData(renderer->getDeviceContext());
	terrainDepthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixScaling(0.1f, 1.f, 0.1f), lightViewMatrix, lightProjectionMatrix, textureMgr->getTexture(L"terrainHeight"));
	terrainDepthShader->render(renderer->getDeviceContext(), terrain->getIndexCount());

	cube->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(50.f,12.5f,50.f), lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), cube->getIndexCount());

	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}

void App1::firstPass()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	// Clear the scene. (default blue colour)
	renderer->beginScene(0.f, 0.f, 0.f, 1.0f);

	// Generate the view matrix based on the camera's position.
	camera->update();


	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();

	//render the terrain using the terrain shader I created
	terrain->sendData(renderer->getDeviceContext());
	terrainShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixScaling(0.1f, 1.f, 0.1f), viewMatrix, projectionMatrix, textureMgr->getTexture(L"sand"), textureMgr->getTexture(L"grass"), textureMgr->getTexture(L"terrainHeight"), shadowMap->getDepthMapSRV(), light1, light2, light3);
	terrainShader->render(renderer->getDeviceContext(), terrain->getIndexCount());

	//render the water using the water shader I created
	water->sendData(renderer->getDeviceContext());
	waterShader->setShaderParameters(renderer->getDeviceContext(), (worldMatrix*XMMatrixScaling(0.25f, 1.f, 0.25f))* XMMatrixTranslation(-400,0,-400), viewMatrix, projectionMatrix, textureMgr->getTexture(L"water"), textureMgr->getTexture(L"waterMap1"), textureMgr->getTexture(L"waterMap2"), light1, light2, light3, elapsedTime);
	waterShader->render(renderer->getDeviceContext(), water->getIndexCount());

	cube->sendData(renderer->getDeviceContext());
	lightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(50.f, 12.5f, 50.f), viewMatrix, projectionMatrix, textureMgr->getTexture(L"sand"), light1, light2, light3);
	lightShader->render(renderer->getDeviceContext(), water->getIndexCount());

	playerView->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix,camera->getOrthoViewMatrix(), renderer->getOrthoMatrix(), shadowMap->getDepthMapSRV());
	textureShader->render(renderer->getDeviceContext(), playerView->getIndexCount());

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

	ImGui::SliderFloat3("DEBUG LIGHT POS", debuglightPos, 0, 100);

	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

