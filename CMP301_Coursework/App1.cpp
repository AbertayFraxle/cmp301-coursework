
#include "App1.h"

App1::App1()
{
	lightShader = nullptr;
}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in, bool VSYNC, bool FULL_SCREEN)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);
	
	sceneTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);

	cube = new AModel(renderer->getDevice(), "res/skybox.obj");
	debugSphere = new SphereMesh(renderer->getDevice(), renderer->getDeviceContext());

	terrain = new TessellationPlane(renderer->getDevice(), renderer->getDeviceContext(),100,100);
	water = new TessellationPlane(renderer->getDevice(), renderer->getDeviceContext(),800,800);

	beachHut = new AModel(renderer->getDevice(), "res/hut.obj");
	light = new AModel(renderer->getDevice(), "res/light.obj");
	stool = new AModel(renderer->getDevice(), "res/stool.obj");
	turtle = new AModel(renderer->getDevice(), "res/turtle.fbx");
	shelf = new AModel(renderer->getDevice(), "res/shelf.obj");
	bottle1 = new AModel(renderer->getDevice(), "res/bottle1.obj");
	bottle2 = new AModel(renderer->getDevice(), "res/bottle2.obj");


	drunk = false;

	playerView = new OrthoMesh(renderer->getDevice(),renderer->getDeviceContext(), screenWidth, screenHeight);
	textureMgr->loadTexture(L"sand", L"res/sand.png");
	textureMgr->loadTexture(L"sandnormal", L"res/sandnormal.png");
	textureMgr->loadTexture(L"terrainHeight", L"res/heightmap.png");
	textureMgr->loadTexture(L"water", L"res/water2.jpg");
	textureMgr->loadTexture(L"grass", L"res/grass.png");
	textureMgr->loadTexture(L"grassnormal", L"res/grassnormal.png");
	textureMgr->loadTexture(L"waterMap1", L"res/waterheightmap/0.png");
	textureMgr->loadTexture(L"waterMap2", L"res/waterheightmap/1.png");
	textureMgr->loadTexture(L"planks", L"res/planks.png");
	textureMgr->loadTexture(L"planksnormal", L"res/planksnormal.png");
	textureMgr->loadTexture(L"bronze", L"res/bronze.png");
	textureMgr->loadTexture(L"bronzenormal", L"res/bronzenormal.png");
	textureMgr->loadTexture(L"skytex", L"res/skytex.png");
	textureMgr->loadTexture(L"stool", L"res/stool.png");
	textureMgr->loadTexture(L"stoolnormal", L"res/stoolnormal.png");
	textureMgr->loadTexture(L"turtle", L"res/turtle.png");
	textureMgr->loadTexture(L"nonormal", L"res/nonormal.png");
	textureMgr->loadTexture(L"bottle", L"res/bottle.png");
	textureMgr->loadTexture(L"bottle1", L"res/bottle1.png");


	lightShader = new LightShader(renderer->getDevice(), hwnd);
	waterShader = new WaterShader(renderer->getDevice(), hwnd);
	depthShader = new DepthShader(renderer->getDevice(), hwnd);
	textureShader = new TextureShader(renderer->getDevice(), hwnd);
	terrainDepthShader = new TerrainDepthShader(renderer->getDevice(), hwnd);
	drunkShader = new DrunkShader(renderer->getDevice(), hwnd);
	terrainTessellationShader = new TerrainTessellationShader(renderer->getDevice(), hwnd);
	waterTessellationShader = new WaterTessellationShader(renderer->getDevice(), hwnd);
	skyShader = new SkyShader(renderer->getDevice(), hwnd);

	// Variables for defining shadow map
	int shadowmapWidth = 4096;
	int shadowmapHeight = 4096;
	int sceneWidth = 120;
	int sceneHeight = 120;

	// This is your shadow map
	

	// Configure point light.

	//directional light
	lights[0] = new Light();
	lights[0]->setAmbientColour(0.075f, 0.075f, 0.15f, 1.0f);
	lights[0]->setDiffuseColour(0.3f, 0.3f, 0.4f, 1.0f);
	lights[0]->setDirection(0.5f, -1.f, 0.5f);
	lights[0]->setPosition(0.f, 100.f, 0.f);
	lights[0]->setConeAngle(0.f);
	lights[0]->setSpecularColour(1, 1, 1, 1);
	lights[0]->setSpecularPower(500);

	lights[1] = new Light();
	lights[1]->setAmbientColour(0.0f, 0.0f, 0.0f, 1.0f);
	lights[1]->setDiffuseColour(1.0f, 0.6f, 0.0f, 1.0f);
	lights[1]->setPosition(48.325f, 14.1f, 55.f);
	lights[1]->setDirection(-0.1f, -1.f, 0.f);
	lights[1]->setConeAngle(15.f);
	lights[1]->setSpecularColour(0, 1, 1, 1);
	lights[1]->setSpecularPower(100);

	lights[2] = new Light();
	lights[2]->setAmbientColour(0.0f, 0.0f, 0.0f, 1.0f);
	lights[2]->setDiffuseColour(1.0f, 0.6f, 0.0f, 1.0f);
	lights[2]->setPosition(48.325f, 14.1f, 57.f);
	lights[2]->setDirection(-0.1f, -1.f, 0.f);
	lights[2]->setConeAngle(15.f);
	lights[2]->setSpecularColour(0, 1, 1, 1);
	lights[2]->setSpecularPower(100);

	lights[3] = new Light();
	lights[3]->setAmbientColour(0.0f, 0.0f, 0.0f, 1.0f);
	lights[3]->setDiffuseColour(1.0f, 0.6f, 0.0f, 1.0f);
	lights[3]->setPosition(48.325f, 14.1f, 53.f);
	lights[3]->setDirection(-0.1f, -1.f, 0.f);
	lights[3]->setConeAngle(15.f);
	lights[3]->setSpecularColour(0, 1, 1, 1);
	lights[3]->setSpecularPower(100);

	for (int i = 0; i < LIGHTCOUNT; i++) {
		if (lights[i]->getConeAngle() == 0) {
			lights[i]->generateOrthoMatrix(sceneWidth, sceneHeight, 0.1, 400);

			shadowMaps[i] = new ShadowMap(renderer->getDevice(), shadowmapWidth*4, shadowmapHeight*4);

		}
		else {

			shadowMaps[i] = new ShadowMap(renderer->getDevice(), shadowmapWidth, shadowmapHeight);

		}


	}

	cameraDepth = new ShadowMap(renderer->getDevice(),screenWidth, screenHeight);

	tessAmount = 16;

	doubleVision = 8;
	intensity = 0.1f;

}


App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	// Release the Direct3D object.


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

		XMMATRIX lightProjectionMatrix;
		XMMATRIX lightViewMatrix;

		

		//set the current light's shadowmap to be the render target
		shadowMaps[i]->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());

			//generate view matrix for current light
		lights[i]->generateViewMatrix();

			

		//if light doesn't have a cone angle, use ortho matrix, else update and get projection matrix
		if (lights[i]->getConeAngle() == 0) {
			lightProjectionMatrix = lights[i]->getOrthoMatrix();
		}
		else {
			lights[i]->generateProjectionMatrix(0.1, 10);
			lightProjectionMatrix = lights[i]->getProjectionMatrix();
		}


		//get matrices
		lightViewMatrix = lights[i]->getViewMatrix();
		
		XMMATRIX worldMatrix = renderer->getWorldMatrix();


		//render the world
		terrain->sendData(renderer->getDeviceContext());
		terrainDepthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, textureMgr->getTexture(L"terrainHeight"),tessAmount);
		terrainDepthShader->render(renderer->getDeviceContext(), terrain->getIndexCount());

		beachHut->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(50.f, 11.75f, 55.f), lightViewMatrix, lightProjectionMatrix);
		depthShader->render(renderer->getDeviceContext(), beachHut->getIndexCount());

		shelf->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(50.f, 11.75f, 55.f), lightViewMatrix, lightProjectionMatrix);
		depthShader->render(renderer->getDeviceContext(), shelf->getIndexCount());

		light->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(50.f, 11.75f, 53.f), lightViewMatrix, lightProjectionMatrix);
		depthShader->render(renderer->getDeviceContext(), light->getIndexCount());

		light->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(50.f, 11.75f, 55.f), lightViewMatrix, lightProjectionMatrix);
		depthShader->render(renderer->getDeviceContext(), light->getIndexCount());

		light->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(50.f, 11.75f, 57.f), lightViewMatrix, lightProjectionMatrix);
		depthShader->render(renderer->getDeviceContext(), light->getIndexCount());

		stool->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(47.5f, 11.6f, 55.f), lightViewMatrix, lightProjectionMatrix);
		depthShader->render(renderer->getDeviceContext(),stool->getIndexCount());

		stool->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(47.5f, 11.6f, 53.f), lightViewMatrix, lightProjectionMatrix);
		depthShader->render(renderer->getDeviceContext(), stool->getIndexCount());

		stool->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(47.5f, 11.6f, 57.f), lightViewMatrix, lightProjectionMatrix);
		depthShader->render(renderer->getDeviceContext(), stool->getIndexCount());

		stool->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(47.5f, 11.6f, 56.f), lightViewMatrix, lightProjectionMatrix);
		depthShader->render(renderer->getDeviceContext(), stool->getIndexCount());

		stool->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(47.5f, 11.6f, 54.f), lightViewMatrix, lightProjectionMatrix);
		depthShader->render(renderer->getDeviceContext(), stool->getIndexCount());

		turtle->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixRotationX(3.14 / 2) * XMMatrixScaling(0.15, 0.15, 0.15) * XMMatrixTranslation(47.5f, 12.62f, 54.f), lightViewMatrix, lightProjectionMatrix);
		depthShader->render(renderer->getDeviceContext(), turtle->getIndexCount());

		//reset
		renderer->setBackBufferRenderTarget();
		renderer->resetViewport();
	}

	camera->update();
	//set the current light's shadowmap to be the render target
	cameraDepth->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());

	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();

	terrain->sendData(renderer->getDeviceContext());
	terrainDepthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"terrainHeight"), tessAmount);
	terrainDepthShader->render(renderer->getDeviceContext(), terrain->getIndexCount());

	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}

void App1::firstPass()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	//Clear the scene. (default blue colour)
	sceneTexture->setRenderTarget(renderer->getDeviceContext());
	sceneTexture->clearRenderTarget(renderer->getDeviceContext(),0.f,0.f,0.f, 1.0f);

	//renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);




	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();

	
	renderer->setZBuffer(false);
	cube->sendData(renderer->getDeviceContext());
	skyShader->setShaderParameters(renderer->getDeviceContext(),worldMatrix *XMMatrixRotationZ(3.14/2) *XMMatrixTranslation(camera->getPosition().x, camera->getPosition().y, camera->getPosition().z), viewMatrix, projectionMatrix, textureMgr->getTexture(L"skytex"));
	skyShader->render(renderer->getDeviceContext(), cube->getIndexCount());
	renderer->setZBuffer(true);
	
	terrain->sendData(renderer->getDeviceContext());
	terrainTessellationShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, tessAmount,textureMgr->getTexture(L"sand"), textureMgr->getTexture(L"grass"), textureMgr->getTexture(L"sandnormal"), textureMgr->getTexture(L"grassnormal"), textureMgr->getTexture(L"terrainHeight"), shadowMaps, lights,camera);
	terrainTessellationShader->render(renderer->getDeviceContext(), terrain->getIndexCount());

	water->sendData(renderer->getDeviceContext());
	waterTessellationShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(-400.f, 0.f, -400.f), viewMatrix, projectionMatrix, tessAmount,textureMgr->getTexture(L"water"), textureMgr->getTexture(L"waterMap1"), textureMgr->getTexture(L"waterMap2"),elapsedTime, lights,camera);
	waterTessellationShader->render(renderer->getDeviceContext(), water->getIndexCount());

	beachHut->sendData(renderer->getDeviceContext());
	lightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(50.f, 11.75f, 55.f), viewMatrix, projectionMatrix, textureMgr->getTexture(L"planks"), textureMgr->getTexture(L"planksnormal"), shadowMaps, lights);
	lightShader->render(renderer->getDeviceContext(), beachHut->getIndexCount());

	for (int i = 0; i < 8; i++) {
		bottle1->sendData(renderer->getDeviceContext());
		lightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(51.f, 13.35f, 53.25f + (i*0.5)), viewMatrix, projectionMatrix, textureMgr->getTexture(L"bottle1"), textureMgr->getTexture(L"nonormal"), shadowMaps, lights);
		lightShader->render(renderer->getDeviceContext(), bottle1->getIndexCount());

		bottle2->sendData(renderer->getDeviceContext());
		lightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixTranslation(51.f, 12.85f, 53.25f + (i * 0.5)), viewMatrix, projectionMatrix, textureMgr->getTexture(L"bottle"), textureMgr->getTexture(L"nonormal"), shadowMaps, lights);
		lightShader->render(renderer->getDeviceContext(), bottle2->getIndexCount());
	}

	for (int i = 0; i < 4; i++) {
		bottle2->sendData(renderer->getDeviceContext());
		lightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixScaling(0.75f, 0.75f, 0.75f) * XMMatrixTranslation(51.f, 13.34f, 53.5f + (i * 1)), viewMatrix, projectionMatrix, textureMgr->getTexture(L"bottle"), textureMgr->getTexture(L"nonormal"), shadowMaps, lights);
		lightShader->render(renderer->getDeviceContext(), bottle2->getIndexCount());

		bottle1->sendData(renderer->getDeviceContext());
		lightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixScaling(0.75f, 0.75f, 0.75f) * XMMatrixTranslation(51.f, 12.85f, 53.5f + (i * 1)), viewMatrix, projectionMatrix, textureMgr->getTexture(L"bottle1"), textureMgr->getTexture(L"nonormal"), shadowMaps, lights);
		lightShader->render(renderer->getDeviceContext(), bottle1->getIndexCount());
	}

	shelf->sendData(renderer->getDeviceContext());
	lightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(50.f, 11.75f, 55.f), viewMatrix, projectionMatrix, textureMgr->getTexture(L"planks"), textureMgr->getTexture(L"planksnormal"), shadowMaps, lights);
	lightShader->render(renderer->getDeviceContext(), shelf->getIndexCount());
    
	for (int i = 0; i < 3; i++) {
		light->sendData(renderer->getDeviceContext());
		lightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(50.f, 11.75f, 53.f +(i*2.f)), viewMatrix, projectionMatrix, textureMgr->getTexture(L"bronze"), textureMgr->getTexture(L"bronzenormal"), shadowMaps, lights);
		lightShader->render(renderer->getDeviceContext(), light->getIndexCount());
	}

	stool->sendData(renderer->getDeviceContext());
	lightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(47.5f, 11.6f, 55.f), viewMatrix, projectionMatrix, textureMgr->getTexture(L"stool"), textureMgr->getTexture(L"stoolnormal"), shadowMaps, lights);
	lightShader->render(renderer->getDeviceContext(), stool->getIndexCount());

	stool->sendData(renderer->getDeviceContext());
	lightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(47.5f, 11.6f, 53.f), viewMatrix, projectionMatrix, textureMgr->getTexture(L"stool"), textureMgr->getTexture(L"stoolnormal"), shadowMaps, lights);
	lightShader->render(renderer->getDeviceContext(), stool->getIndexCount());

	stool->sendData(renderer->getDeviceContext());
	lightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(47.5f, 11.6f, 57.f), viewMatrix, projectionMatrix, textureMgr->getTexture(L"stool"), textureMgr->getTexture(L"stoolnormal"), shadowMaps, lights);
	lightShader->render(renderer->getDeviceContext(), stool->getIndexCount());

	stool->sendData(renderer->getDeviceContext());
	lightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(47.5f, 11.6f, 56.f), viewMatrix, projectionMatrix, textureMgr->getTexture(L"stool"), textureMgr->getTexture(L"stoolnormal"), shadowMaps, lights);
	lightShader->render(renderer->getDeviceContext(), stool->getIndexCount());

	stool->sendData(renderer->getDeviceContext());
	lightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(47.5f, 11.6f, 54.f), viewMatrix, projectionMatrix, textureMgr->getTexture(L"stool"), textureMgr->getTexture(L"stoolnormal"), shadowMaps, lights);
	lightShader->render(renderer->getDeviceContext(), stool->getIndexCount());
	

	turtle->sendData(renderer->getDeviceContext());
	lightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix *XMMatrixRotationX(3.14/2)* XMMatrixScaling(0.15, 0.15, 0.15) * XMMatrixTranslation(47.5f, 12.62f, 54.f), viewMatrix, projectionMatrix, textureMgr->getTexture(L"turtle"), textureMgr->getTexture(L"aaa"), shadowMaps, lights);
	lightShader->render(renderer->getDeviceContext(), turtle->getIndexCount());
	
	/*
	for (int i = 0; i < LIGHTCOUNT; i++) {
		debugSphere->sendData(renderer->getDeviceContext());
		lightShader->setShaderParameters(renderer->getDeviceContext(), (worldMatrix * XMMatrixScaling(0.1f, 0.1f, 0.1f)) *XMMatrixTranslation(lights[i]->getPosition().x, lights[i]->getPosition().y, lights[i]->getPosition().z), viewMatrix, projectionMatrix, textureMgr->getTexture(L"sand"), textureMgr->getTexture(L"bronzenormal"),shadowMaps, lights);
		lightShader->render(renderer->getDeviceContext(), debugSphere->getIndexCount());
	}
	*/
	

	

	// Render GUI
	renderer->setBackBufferRenderTarget();
	//renderer->endScene();

}

void App1::finalPass()
{
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	renderer->setZBuffer(false);
	playerView->sendData(renderer->getDeviceContext());

	if (drunk) {
		drunkShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, camera->getOrthoViewMatrix(), sceneTexture->getOrthoMatrix(), sceneTexture->getShaderResourceView(), elapsedTime,doubleVision,intensity);
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
	//ImGui::Checkbox("Wireframe mode", &wireframeToggle);
	ImGui::Text("Press E to raise camera \nto see the plane being rendered");

	ImGui::Checkbox("Drunk", &drunk);


	if (drunk) {
		ImGui::SliderFloat("Double Vision: ",&doubleVision,1.f,16.f);
		ImGui::SliderFloat("Intensity: ", &intensity, 0.f, 5.f);
	}

	
	ImGui::SliderInt("Manual Tesselation", &tessAmount, 2, 64);

	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

