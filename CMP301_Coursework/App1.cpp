
#include "App1.h"

App1::App1()
{
	lightShader = nullptr;
}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in, bool VSYNC, bool FULL_SCREEN)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);
	


	//load the skybox model that i made and unwrapped in blender
	cube = new AModel(renderer->getDevice(), "res/models/skybox.obj");

	//this sphere deserves the medal of honour
	debugSphere = new SphereMesh(renderer->getDevice(), renderer->getDeviceContext());

	//create two planes that can be tesselated, used for terrain and the water
	terrain = new TessellationPlane(renderer->getDevice(), renderer->getDeviceContext(),100,100);
	water = new TessellationPlane(renderer->getDevice(), renderer->getDeviceContext(),800,800);

	//load all the models for the scene
	beachHut = new AModel(renderer->getDevice(), "res/models/hut.obj");
	light = new AModel(renderer->getDevice(), "res/models/light.obj");
	stool = new AModel(renderer->getDevice(), "res/models/stool.obj");
	turtle = new AModel(renderer->getDevice(), "res/models/turtle.fbx");
	shelf = new AModel(renderer->getDevice(), "res/models/shelf.obj");
	bottle1 = new AModel(renderer->getDevice(), "res/models/bottle1.obj");
	bottle2 = new AModel(renderer->getDevice(), "res/models/bottle2.obj");
	moai = new AModel(renderer->getDevice(), "res/models/moaiHead.fbx");
	coconut = new AModel(renderer->getDevice(), "res/models/coco.obj");

	//initialise bool to trigger post processing
	drunk = false;

	//intialise texture to store render pas before post processing
	sceneTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);

	//intialise orthomesh to show the scene to the player
	playerView = new OrthoMesh(renderer->getDevice(),renderer->getDeviceContext(), screenWidth, screenHeight);

	//load all textures we need

	//ALL TEXTURES (apart from turtle) USED WITH LICENSING FROM TEXTURES.COM

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
	textureMgr->loadTexture(L"stone", L"res/stone.png");
	textureMgr->loadTexture(L"stonenormal", L"res/stoneNormal.png");
	textureMgr->loadTexture(L"coconut", L"res/coco.png");
	textureMgr->loadTexture(L"torch", L"res/torch.png");
	textureMgr->loadTexture(L"torchnormal", L"res/torchnormal.png");

	//load all the shaders used in the scene
	lightShader = new LightShader(renderer->getDevice(), hwnd);
	waterShader = new WaterShader(renderer->getDevice(), hwnd);
	depthShader = new DepthShader(renderer->getDevice(), hwnd);
	textureShader = new TextureShader(renderer->getDevice(), hwnd);
	terrainDepthShader = new TerrainDepthShader(renderer->getDevice(), hwnd);
	drunkShader = new DrunkShader(renderer->getDevice(), hwnd);
	terrainTessellationShader = new TerrainTessellationShader(renderer->getDevice(), hwnd);
	waterTessellationShader = new WaterTessellationShader(renderer->getDevice(), hwnd);
	skyShader = new SkyShader(renderer->getDevice(), hwnd);
	waterDepthShader = new WaterDepthShader(renderer->getDevice(), hwnd);
	bottleShader = new BottleShader(renderer->getDevice(), hwnd);

	// Variables for defining shadow map
	int shadowmapWidth = 4096;
	int shadowmapHeight = 4096;
	int sceneWidth = 500;
	int sceneHeight = 500;


	//define all the lights that I've used in the scene

	//directional light 
	lights[0] = new Light();
	lights[0]->setAmbientColour(0.075f, 0.075f, 0.15f, 1.0f);
	lights[0]->setDiffuseColour(0.3f, 0.3f, 0.4f, 1.0f);
	lights[0]->setDirection(0.5f, -1.f, 0.5f);
	lights[0]->setPosition(0.f, 100.f, 0.f);
	lights[0]->setConeAngle(0.f);
	lights[0]->setSpecularColour(1, 1, 1, 1);
	lights[0]->setSpecularPower(500);

	//bar light 1
	lights[1] = new Light();
	lights[1]->setAmbientColour(0.0f, 0.0f, 0.0f, 1.0f);
	lights[1]->setDiffuseColour(1.0f, 0.6f, 0.0f, 1.0f);
	lights[1]->setPosition(48.325f, 14.1f, 55.f);
	lights[1]->setDirection(-0.1f, -1.f, 0.f);
	lights[1]->setConeAngle(15.f);
	lights[1]->setSpecularColour(0, 1, 1, 1);
	lights[1]->setSpecularPower(100);

	//bar light 2
	lights[2] = new Light();
	lights[2]->setAmbientColour(0.0f, 0.0f, 0.0f, 1.0f);
	lights[2]->setDiffuseColour(1.0f, 0.6f, 0.0f, 1.0f);
	lights[2]->setPosition(48.325f, 14.1f, 57.f);
	lights[2]->setDirection(-0.1f, -1.f, 0.f);
	lights[2]->setConeAngle(15.f);
	lights[2]->setSpecularColour(0, 1, 1, 1);
	lights[2]->setSpecularPower(100);

	//bar light 3
	lights[3] = new Light();
	lights[3]->setAmbientColour(0.0f, 0.0f, 0.0f, 1.0f);
	lights[3]->setDiffuseColour(1.0f, 0.6f, 0.0f, 1.0f);
	lights[3]->setPosition(48.325f, 14.1f, 53.f);
	lights[3]->setDirection(-0.1f, -1.f, 0.f);
	lights[3]->setConeAngle(15.f);
	lights[3]->setSpecularColour(0, 1, 1, 1);
	lights[3]->setSpecularPower(100);

	//bar light 4
	lights[4] = new Light();
	lights[4]->setAmbientColour(0.0f, 0.0f, 0.0f, 1.0f);
	lights[4]->setDiffuseColour(1.0f, 0.6f, 0.0f, 1.0f);
	lights[4]->setPosition(51.24f, 14.1f, 55.f);
	lights[4]->setDirection(-0.5f, -1.f, 0.f);
	lights[4]->setConeAngle(15.f);
	lights[4]->setSpecularColour(0, 1, 1, 1);
	lights[4]->setSpecularPower(100);

	//bar light 5
	lights[5] = new Light();
	lights[5]->setAmbientColour(0.0f, 0.0f, 0.0f, 1.0f);
	lights[5]->setDiffuseColour(1.0f, 0.6f, 0.0f, 1.0f);
	lights[5]->setPosition(51.24f, 14.1f, 57.f);
	lights[5]->setDirection(-0.5f, -1.f, 0.f);
	lights[5]->setConeAngle(15.f);
	lights[5]->setSpecularColour(0, 1, 1, 1);
	lights[5]->setSpecularPower(100);

	//bar light 6
	lights[6] = new Light();
	lights[6]->setAmbientColour(0.0f, 0.0f, 0.0f, 1.0f);
	lights[6]->setDiffuseColour(1.0f, 0.6f, 0.0f, 1.0f);
	lights[6]->setPosition(51.24f, 14.1f, 53.f);
	lights[6]->setDirection(-0.5f, -1.f, 0.f);
	lights[6]->setConeAngle(15.f);
	lights[6]->setSpecularColour(0, 1, 1, 1);
	lights[6]->setSpecularPower(100);

	//for each light, generae a shadow map (all only have one as i never implemented point light shadows)
	for (int i = 0; i < LIGHTCOUNT; i++) {
		if (lights[i]->getConeAngle() == 0) {
			lights[i]->generateOrthoMatrix(sceneWidth, sceneHeight, 0.1, 400);

			shadowMaps[i] = new ShadowMap(renderer->getDevice(), shadowmapWidth*4, shadowmapHeight*4);

		}
		else {

			shadowMaps[i] = new ShadowMap(renderer->getDevice(), shadowmapWidth, shadowmapHeight);

		}
	}


	//define values to affect post processing effect shader
	doubleVision = 8;
	intensity = 0.1f;
	
	//set camera initial position
	camera->setPosition(47.5f, 13.3f, 53.f);

	//set default tesselation parameters
	terTess[0] = 10;
	terTess[0] = 200;
	terTess[0] = 2;
	terTess[0] = 64;
	wTessValues = XMINT4(10, 200, 2, 64);
	tTessValues = XMINT4(0, 100, 2, 64);
}


App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	// Release the Direct3D object.

	//delete all objects appropriately

	if (cube) {
		delete cube;
		cube = 0;
	}

	if (debugSphere) {
		delete debugSphere;
		debugSphere = 0;
	}

	if (terrain) {
		delete terrain;
		terrain = 0;
	}

	if (water)
	{
		delete water;
		water = 0;
	}

	if (beachHut)
	{
		delete beachHut;
		beachHut = 0;
	}

	if (stool)
	{
		delete stool;
		stool = 0;
	}

	if (turtle)
	{
		delete turtle;
		turtle = 0;
	}

	if (shelf)
	{
		delete shelf;
		shelf = 0;
	}

	if (bottle1)
	{
		delete bottle1;
		bottle1 = 0;
	}

	if (bottle2)
	{
		delete bottle2;
		bottle2 = 0;
	}

	if (moai)
	{
		delete moai;
		moai = 0;
	}

	if (coconut) {
		delete coconut;
		coconut = 0;
	}

	if (sceneTexture)
	{
		delete sceneTexture;
		sceneTexture = 0;
	}

	if (playerView) {
		delete playerView;
		playerView = 0;
	}

	if (lightShader)
	{
		delete lightShader;
		lightShader = 0;
	}

	if (waterShader)
	{
		delete waterShader;
		waterShader = 0;
	}

	if (depthShader)
	{
		delete depthShader;
		depthShader = 0;
	}

	if (textureShader)
	{
		delete textureShader;
		textureShader = 0;
	}

	if (terrainDepthShader)
	{
		delete terrainDepthShader;
		terrainDepthShader = 0;
	}

	if (drunkShader)
	{
		delete drunkShader;
		drunkShader = 0;
	}

	if (terrainTessellationShader)
	{
		delete terrainTessellationShader;
		terrainTessellationShader = 0;
	}

	if (waterTessellationShader)
	{
		delete waterTessellationShader;
		waterTessellationShader = 0;
	}

	if (skyShader)
	{
		delete skyShader;
		skyShader = 0;
	}

	if (waterDepthShader)
	{
		delete waterDepthShader;
		waterDepthShader = 0;
	}

	if (bottleShader)
	{
		delete bottleShader;
		bottleShader = 0;
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
	

	//increase the counter of elapsed time for water and for drunk shader
	elapsedTime += timer->getTime();


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
	//capture the depth of the scene from the lights positions
	depthPass();

	//render the scene geometry to a render texture
	firstPass();

	//render the texture with or without post processing applied
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
		terrainDepthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, textureMgr->getTexture(L"terrainHeight"),tTessValues);
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

		moai->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix* XMMatrixScaling(0.55f, 0.55f, 0.55f) * XMMatrixRotationX(3.14 / 2) * XMMatrixRotationY(3.14 / 2) * XMMatrixTranslation(49.1f, 11.75f, 53.f), lightViewMatrix, lightProjectionMatrix);
		depthShader->render(renderer->getDeviceContext(),moai->getIndexCount());

		moai->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixScaling(0.55f, 0.55f, 0.55f) * XMMatrixRotationX(3.14 / 1.9) * XMMatrixTranslation(50.f, 1.f, 100.f), lightViewMatrix, lightProjectionMatrix);
		depthShader->render(renderer->getDeviceContext(), moai->getIndexCount());

		coconut->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(48.2f, 12.75f, 53.f), lightViewMatrix, lightProjectionMatrix);
		depthShader->render(renderer->getDeviceContext(), coconut->getIndexCount());

		water->sendData(renderer->getDeviceContext());
		waterDepthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(-400.f, 0.f, -400.f), lightViewMatrix, lightProjectionMatrix, textureMgr->getTexture(L"waterMap1"), textureMgr->getTexture(L"waterMap2"),  elapsedTime,wTessValues);
		waterDepthShader->render(renderer->getDeviceContext(), water->getIndexCount());

		for (int i = 0; i < 8; i++) {
			bottle1->sendData(renderer->getDeviceContext());
			depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(51.f, 13.25f, 53.25f + (i * 0.5)), lightViewMatrix, lightProjectionMatrix);
			depthShader->render(renderer->getDeviceContext(), bottle1->getIndexCount());

			bottle2->sendData(renderer->getDeviceContext());
			depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixTranslation(51.f, 12.85f, 53.25f + (i * 0.5)), lightViewMatrix, lightProjectionMatrix);
			depthShader->render(renderer->getDeviceContext(), bottle2->getIndexCount());
		}

		for (int i = 0; i < 4; i++) {
			bottle2->sendData(renderer->getDeviceContext());
			depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixScaling(0.75f, 0.75f, 0.75f) * XMMatrixTranslation(51.f, 13.34f, 53.5f + (i * 1)), lightViewMatrix, lightProjectionMatrix);
			depthShader->render(renderer->getDeviceContext(), bottle2->getIndexCount());

			bottle1->sendData(renderer->getDeviceContext());
			depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixScaling(0.75f, 0.75f, 0.75f) * XMMatrixTranslation(51.f, 12.85f, 53.5f + (i * 1)), lightViewMatrix, lightProjectionMatrix);
			depthShader->render(renderer->getDeviceContext(), bottle1->getIndexCount());
		}

		bottle1->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(48.2f, 12.75f, 53.3f), lightViewMatrix, lightProjectionMatrix);
		depthShader->render(renderer->getDeviceContext(), bottle1->getIndexCount());

		//reset
		renderer->setBackBufferRenderTarget();
		renderer->resetViewport();
	}

	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}

void App1::firstPass()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	//update the camera
	camera->update();

	//bind the render texture as the render target
	sceneTexture->setRenderTarget(renderer->getDeviceContext());
	sceneTexture->clearRenderTarget(renderer->getDeviceContext(),0.f,0.f,0.f, 1.0f);

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();

	//disable depth testing and render the skybox at the camera's position
	renderer->setZBuffer(false);
	cube->sendData(renderer->getDeviceContext());
	skyShader->setShaderParameters(renderer->getDeviceContext(),worldMatrix *XMMatrixRotationZ(3.14/2) *XMMatrixTranslation(camera->getPosition().x, camera->getPosition().y, camera->getPosition().z), viewMatrix, projectionMatrix, textureMgr->getTexture(L"skytex"));
	skyShader->render(renderer->getDeviceContext(), cube->getIndexCount());
	renderer->setZBuffer(true);
	
	//render the terrain
	terrain->sendData(renderer->getDeviceContext());
	terrainTessellationShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"sand"), textureMgr->getTexture(L"grass"), textureMgr->getTexture(L"sandnormal"), textureMgr->getTexture(L"grassnormal"), textureMgr->getTexture(L"terrainHeight"), shadowMaps, lights,camera,tTessValues);
	terrainTessellationShader->render(renderer->getDeviceContext(), terrain->getIndexCount());

	//render the water
	water->sendData(renderer->getDeviceContext());
	waterTessellationShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(-400.f, 0.f, -400.f), viewMatrix, projectionMatrix,textureMgr->getTexture(L"water"), textureMgr->getTexture(L"waterMap1"), textureMgr->getTexture(L"waterMap2"),elapsedTime, lights,camera,shadowMaps,wTessValues);
	waterTessellationShader->render(renderer->getDeviceContext(), water->getIndexCount());

	//render the beach hut
	beachHut->sendData(renderer->getDeviceContext());
	lightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(50.f, 11.75f, 55.f), viewMatrix, projectionMatrix, textureMgr->getTexture(L"planks"), textureMgr->getTexture(L"planksnormal"), shadowMaps, lights);
	lightShader->render(renderer->getDeviceContext(), beachHut->getIndexCount());

	//render the bottles on the beach hut shelves
	for (int i = 0; i < 8; i++) {
		bottle1->sendData(renderer->getDeviceContext());
		bottleShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(51.f, 13.25f, 53.25f + (i*0.5)), viewMatrix, projectionMatrix, textureMgr->getTexture(L"bottle1"), textureMgr->getTexture(L"nonormal"), shadowMaps, lights,camera);
		bottleShader->render(renderer->getDeviceContext(), bottle1->getIndexCount());

		bottle2->sendData(renderer->getDeviceContext());
		bottleShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixTranslation(51.f, 12.85f, 53.25f + (i * 0.5)), viewMatrix, projectionMatrix, textureMgr->getTexture(L"bottle"), textureMgr->getTexture(L"nonormal"), shadowMaps, lights,camera);
		bottleShader->render(renderer->getDeviceContext(), bottle2->getIndexCount());
	}

	for (int i = 0; i < 4; i++) {
		bottle2->sendData(renderer->getDeviceContext());
		bottleShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixScaling(0.75f, 0.75f, 0.75f) * XMMatrixTranslation(51.f, 13.34f, 53.5f + (i * 1)), viewMatrix, projectionMatrix, textureMgr->getTexture(L"bottle"), textureMgr->getTexture(L"nonormal"), shadowMaps, lights,camera);
		bottleShader->render(renderer->getDeviceContext(), bottle2->getIndexCount());

		bottle1->sendData(renderer->getDeviceContext());
		bottleShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixScaling(0.75f, 0.75f, 0.75f) * XMMatrixTranslation(51.f, 12.85f, 53.5f + (i * 1)), viewMatrix, projectionMatrix, textureMgr->getTexture(L"bottle1"), textureMgr->getTexture(L"nonormal"), shadowMaps, lights,camera);
		bottleShader->render(renderer->getDeviceContext(), bottle1->getIndexCount());
	}

	bottle1->sendData(renderer->getDeviceContext());
	bottleShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(48.2f, 12.75f, 53.3f), viewMatrix, projectionMatrix, textureMgr->getTexture(L"bottle1"), textureMgr->getTexture(L"nonormal"), shadowMaps, lights, camera);
	bottleShader->render(renderer->getDeviceContext(), bottle1->getIndexCount());

	//render the beach hut shelves
	shelf->sendData(renderer->getDeviceContext());
	lightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(50.f, 11.75f, 55.f), viewMatrix, projectionMatrix, textureMgr->getTexture(L"planks"), textureMgr->getTexture(L"planksnormal"), shadowMaps, lights);
	lightShader->render(renderer->getDeviceContext(), shelf->getIndexCount());
    
	//render the lights in the beach hut
	for (int i = 0; i < 3; i++) {
		light->sendData(renderer->getDeviceContext());
		lightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(50.f, 11.75f, 53.f +(i*2.f)), viewMatrix, projectionMatrix, textureMgr->getTexture(L"bronze"), textureMgr->getTexture(L"bronzenormal"), shadowMaps, lights);
		lightShader->render(renderer->getDeviceContext(), light->getIndexCount());
	}

	for (int i = 0; i < 3; i++) {
		light->sendData(renderer->getDeviceContext());
		lightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(52.9f, 11.75f, 53.f + (i * 2.f)), viewMatrix, projectionMatrix, textureMgr->getTexture(L"bronze"), textureMgr->getTexture(L"bronzenormal"), shadowMaps, lights);
		lightShader->render(renderer->getDeviceContext(), light->getIndexCount());
	}

	//render the stools at the bar
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
	

	//render the friends :)
	turtle->sendData(renderer->getDeviceContext());
	bottleShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix *XMMatrixRotationX(3.14/2)* XMMatrixScaling(0.15, 0.15, 0.15) * XMMatrixTranslation(47.5f, 12.62f, 54.f), viewMatrix, projectionMatrix, textureMgr->getTexture(L"turtle"), textureMgr->getTexture(L"aaa"), shadowMaps, lights,camera);
	bottleShader->render(renderer->getDeviceContext(), turtle->getIndexCount());

	moai->sendData(renderer->getDeviceContext());
	lightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixScaling(0.55f, 0.55f, 0.55f) *XMMatrixRotationX(3.14 / 2) * XMMatrixRotationY(3.14 / 2) * XMMatrixTranslation(49.1f, 11.75f, 53.f), viewMatrix, projectionMatrix, textureMgr->getTexture(L"stone"), textureMgr->getTexture(L"stonenormal"), shadowMaps, lights);
	lightShader->render(renderer->getDeviceContext(), moai->getIndexCount());

	moai->sendData(renderer->getDeviceContext());
	lightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixScaling(0.55f, 0.55f, 0.55f) * XMMatrixRotationX(3.14 / 1.9)  *XMMatrixTranslation(50.f, 1.f, 100.f), viewMatrix, projectionMatrix, textureMgr->getTexture(L"stone"), textureMgr->getTexture(L"stonenormal"), shadowMaps, lights);
	lightShader->render(renderer->getDeviceContext(), moai->getIndexCount());


	//render the drinking apparatus
	coconut->sendData(renderer->getDeviceContext());
	lightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix  * XMMatrixTranslation(48.2f, 12.75f, 53.f), viewMatrix, projectionMatrix, textureMgr->getTexture(L"coconut"), textureMgr->getTexture(L"aaa"), shadowMaps, lights);
	lightShader->render(renderer->getDeviceContext(),coconut->getIndexCount());
	

	//this was used to debug lights positions
	/*
	for (int i = 0; i < LIGHTCOUNT; i++) {
		debugSphere->sendData(renderer->getDeviceContext());
		lightShader->setShaderParameters(renderer->getDeviceContext(), (worldMatrix * XMMatrixScaling(0.1f, 0.1f, 0.1f)) *XMMatrixTranslation(lights[i]->getPosition().x, lights[i]->getPosition().y, lights[i]->getPosition().z), viewMatrix, projectionMatrix, textureMgr->getTexture(L"sand"), textureMgr->getTexture(L"bronzenormal"),shadowMaps, lights);
		lightShader->render(renderer->getDeviceContext(), debugSphere->getIndexCount());
	}
	*/

	renderer->setBackBufferRenderTarget();

}

void App1::finalPass()
{
	//begin the scene
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

	//get the world matrix
	XMMATRIX worldMatrix = renderer->getWorldMatrix();

	//render the renderTexture with either the drunk shader or default texture shader
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

	//post process shader controls

	ImGui::Checkbox("Drunk", &drunk);
	if (drunk) {
		ImGui::SliderFloat("Double Vision: ",&doubleVision,1.f,16.f);
		ImGui::SliderFloat("Intensity: ", &intensity, 0.f, 5.f);
	}

	//terrain tesselation controls
	ImGui::Text("Terrain Tesselation:");
	ImGui::SliderInt("Min T Tess Dist", &tTessValues.x, 0, 200);
	ImGui::SliderInt("Max T Tess Dist", &tTessValues.y, 0, 200);
	ImGui::SliderInt("Min T Tess Factor", &tTessValues.z, 2, 64);
	ImGui::SliderInt("Max T Tess Factor", &tTessValues.w, 2, 64);

	//water tesselation controls
	ImGui::Text("Water Tesselation:");
	ImGui::SliderInt("Min W Tess Dist", &wTessValues.x, 0, 200);
	ImGui::SliderInt("Max W Tess Dist", &wTessValues.y, 0, 200);
	ImGui::SliderInt("Min W Tess Factor", &wTessValues.z, 2, 64);
	ImGui::SliderInt("Max W Tess Factor", &wTessValues.w, 2, 64);


	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

