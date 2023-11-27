
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
	
	sceneTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);

	cube = new CubeMesh(renderer->getDevice(), renderer->getDeviceContext());
	debugSphere = new SphereMesh(renderer->getDevice(), renderer->getDeviceContext());
	//water = new TessellationPlane(renderer->getDevice(), renderer->getDeviceContext(), 100, 100);

	beachHut = new AModel(renderer->getDevice(), "res/hut.obj");
	light = new AModel(renderer->getDevice(), "res/light.obj");

	drunk = false;

	playerView = new OrthoMesh(renderer->getDevice(),renderer->getDeviceContext(), screenWidth, screenHeight);
	textureMgr->loadTexture(L"sand", L"res/sand.png");
	textureMgr->loadTexture(L"terrainHeight", L"res/heightmap.png");
	textureMgr->loadTexture(L"water", L"res/water2.jpg");
	textureMgr->loadTexture(L"grass", L"res/grass.png");
	textureMgr->loadTexture(L"waterMap1", L"res/waterheightmap/0.png");
	textureMgr->loadTexture(L"waterMap2", L"res/waterheightmap/1.png");
	textureMgr->loadTexture(L"planks", L"res/planks.png");
	textureMgr->loadTexture(L"planksnormal", L"res/planksnormal.png");
	textureMgr->loadTexture(L"bronze", L"res/bronze.png");
	textureMgr->loadTexture(L"bronzenormal", L"res/bronzenormal.png");

	lightShader = new LightShader(renderer->getDevice(), hwnd);
	terrainShader = new TerrainShader(renderer->getDevice(), hwnd);
	waterShader = new WaterShader(renderer->getDevice(), hwnd);
	depthShader = new DepthShader(renderer->getDevice(), hwnd);
	textureShader = new TextureShader(renderer->getDevice(), hwnd);
	terrainDepthShader = new TerrainDepthShader(renderer->getDevice(), hwnd);
	drunkShader = new DrunkShader(renderer->getDevice(), hwnd);

	// Variables for defining shadow map
	int shadowmapWidth = 4096;
	int shadowmapHeight = 4096;
	int sceneWidth = 120;
	int sceneHeight = 120;

	// This is your shadow map
	

	// Configure point light.

	//directional light
	lights[0] = new Light();
	lights[0]->setAmbientColour(0.05f, 0.05f, 0.05f, 1.0f);
	lights[0]->setDiffuseColour(0.3f, 0.3f, 0.3f, 1.0f);
	lights[0]->setDirection(1.f, -1.f, 1.f);
	lights[0]->setPosition(-0.25f, 30.f, 0.f);
	lights[0]->setConeAngle(0.f);

	lights[1] = new Light();
	lights[1]->setAmbientColour(0.0f, 0.0f, 0.0f, 1.0f);
	lights[1]->setDiffuseColour(1.0f, 1.f, 1.0f, 1.0f);
	lights[1]->setPosition(48.325f, 14.1f, 55.f);
	lights[1]->setDirection(-0.1f, -1.f, 0.f);
	lights[1]->setConeAngle(15.f);

	lights[2] = new Light();
	lights[2]->setAmbientColour(0.0f, 0.0f, 0.0f, 1.0f);
	lights[2]->setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	lights[2]->setPosition(48.325f, 14.1f, 57.f);
	lights[2]->setDirection(-0.1f, -1.f, 0.f);
	lights[2]->setConeAngle(15.f);

	lights[3] = new Light();
	lights[3]->setAmbientColour(0.0f, 0.0f, 0.0f, 1.0f);
	lights[3]->setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	lights[3]->setPosition(48.325f, 14.1f, 53.f);
	lights[3]->setDirection(-0.1f, -1.f, 0.f);
	lights[3]->setConeAngle(15.f);

	for (int i = 0; i < LIGHTCOUNT; i++) {
		if (lights[i]->getConeAngle() == 0) {
			lights[i]->generateOrthoMatrix(sceneWidth, sceneHeight, 0.1, 200);

			shadowMaps[i] = new ShadowMap(renderer->getDevice(), shadowmapWidth*4, shadowmapHeight*4);

		}
		else {

			shadowMaps[i] = new ShadowMap(renderer->getDevice(), shadowmapWidth, shadowmapHeight);

		}


	}


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
	depthPass();
	firstPass();
	finalPass();

	return true;
}

void App1::depthPass()
{

	//loop for how many lights we have in the scene
	for (int i = 0; i < LIGHTCOUNT; i++) {

		//set the current light's shadowmap to be the render target
		shadowMaps[i]->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());
		
		//generate view matrix for current light
		lights[i]->generateViewMatrix();
		
		XMMATRIX lightProjectionMatrix;

		//if light doesn't have a cone angle, use ortho matrix, else update and get projection matrix
		if (lights[i]->getConeAngle() == 0) {
			lightProjectionMatrix = lights[i]->getOrthoMatrix(); 
		}
		else {
			lights[i]->generateProjectionMatrix(0.1,10);
			lightProjectionMatrix = lights[i]->getProjectionMatrix();
		}


		//get matrices
		XMMATRIX lightViewMatrix = lights[i]->getViewMatrix();
		XMMATRIX worldMatrix = renderer->getWorldMatrix();


		//render the world
		terrain->sendData(renderer->getDeviceContext());
		terrainDepthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixScaling(0.1f, 1.f, 0.1f), lightViewMatrix, lightProjectionMatrix, textureMgr->getTexture(L"terrainHeight"));
		terrainDepthShader->render(renderer->getDeviceContext(), terrain->getIndexCount());

		beachHut->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(50.f, 11.75f, 55.f), lightViewMatrix, lightProjectionMatrix);
		depthShader->render(renderer->getDeviceContext(), beachHut->getIndexCount());

		light->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(50.f, 11.75f, 53.f), lightViewMatrix, lightProjectionMatrix);
		depthShader->render(renderer->getDeviceContext(), light->getIndexCount());

		light->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(50.f, 11.75f, 55.f), lightViewMatrix, lightProjectionMatrix);
		depthShader->render(renderer->getDeviceContext(), light->getIndexCount());

		light->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(50.f, 11.75f, 57.f), lightViewMatrix, lightProjectionMatrix);
		depthShader->render(renderer->getDeviceContext(), light->getIndexCount());

		//reset
		renderer->setBackBufferRenderTarget();
		renderer->resetViewport();
	}
}

void App1::firstPass()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	// Clear the scene. (default blue colour)
	sceneTexture->setRenderTarget(renderer->getDeviceContext());
	sceneTexture->clearRenderTarget(renderer->getDeviceContext(),0.f,0.f,0.f, 1.0f);

	// Generate the view matrix based on the camera's position.
	camera->update();


	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();

	//render the terrain using the terrain shader I created
	terrain->sendData(renderer->getDeviceContext());
	terrainShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixScaling(0.1f, 1.f, 0.1f), viewMatrix, projectionMatrix, textureMgr->getTexture(L"sand"), textureMgr->getTexture(L"grass"), textureMgr->getTexture(L"terrainHeight"), shadowMaps,lights);
	terrainShader->render(renderer->getDeviceContext(), terrain->getIndexCount());

	//render the water using the water shader I created
	water->sendData(renderer->getDeviceContext());
	waterShader->setShaderParameters(renderer->getDeviceContext(), (worldMatrix*XMMatrixScaling(0.25f, 1.f, 0.25f))* XMMatrixTranslation(-400,0,-400), viewMatrix, projectionMatrix, textureMgr->getTexture(L"water"), textureMgr->getTexture(L"waterMap1"), textureMgr->getTexture(L"waterMap2"),  lights, elapsedTime);
	waterShader->render(renderer->getDeviceContext(), water->getIndexCount());

	beachHut->sendData(renderer->getDeviceContext());
	lightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(50.f, 11.75f, 55.f), viewMatrix, projectionMatrix, textureMgr->getTexture(L"planks"), textureMgr->getTexture(L"planksnormal"), lights);
	lightShader->render(renderer->getDeviceContext(), beachHut->getIndexCount());
    
	light->sendData(renderer->getDeviceContext());
	lightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(50.f, 11.75f, 53.f), viewMatrix, projectionMatrix, textureMgr->getTexture(L"bronze"), textureMgr->getTexture(L"bronzenormal"), lights);
	lightShader->render(renderer->getDeviceContext(), light->getIndexCount());

	light->sendData(renderer->getDeviceContext());
	lightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(50.f, 11.75f, 55.f), viewMatrix, projectionMatrix, textureMgr->getTexture(L"bronze"), textureMgr->getTexture(L"bronzenormal"), lights);
	lightShader->render(renderer->getDeviceContext(), light->getIndexCount());

	light->sendData(renderer->getDeviceContext());
	lightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(50.f, 11.75f, 57.f), viewMatrix, projectionMatrix, textureMgr->getTexture(L"bronze"), textureMgr->getTexture(L"bronzenormal"), lights);
	lightShader->render(renderer->getDeviceContext(), light->getIndexCount());

	/*
	for (int i = 0; i < LIGHTCOUNT; i++) {
		debugSphere->sendData(renderer->getDeviceContext());
		lightShader->setShaderParameters(renderer->getDeviceContext(), (worldMatrix * XMMatrixScaling(0.1f, 0.1f, 0.1f)) *XMMatrixTranslation(lights[i]->getPosition().x, lights[i]->getPosition().y, lights[i]->getPosition().z), viewMatrix, projectionMatrix, textureMgr->getTexture(L"sand"), textureMgr->getTexture(L"bronzenormal"), lights);
		lightShader->render(renderer->getDeviceContext(), debugSphere->getIndexCount());
	}
	*/

	

	// Render GUI
	renderer->setBackBufferRenderTarget();



}

void App1::finalPass()
{
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	renderer->setZBuffer(false);
	playerView->sendData(renderer->getDeviceContext());

	if (drunk) {
		drunkShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, camera->getOrthoViewMatrix(), sceneTexture->getOrthoMatrix(), sceneTexture->getShaderResourceView(), elapsedTime);
		drunkShader->render(renderer->getDeviceContext(), playerView->getIndexCount());
	}
	else {
		textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, camera->getOrthoViewMatrix(), sceneTexture->getOrthoMatrix(), sceneTexture->getShaderResourceView());
		textureShader->render(renderer->getDeviceContext(), playerView->getIndexCount());
	}
	renderer->setZBuffer(true);
	gui();

	// Swap the buffers
	renderer->endScene();
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

	ImGui::Checkbox("Drunk", &drunk);

	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

