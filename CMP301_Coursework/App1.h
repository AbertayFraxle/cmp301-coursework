// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "DXF.h"	// include dxframework
#include "LightShader.h"
#include "WaterShader.h"
#include "TextureShader.h"
#include "TessellationPlane.h"
#include "DepthShader.h"
#include "TerrainDepthShader.h"
#include "DrunkShader.h"
#include "TerrainTessellationShader.h"
#include "WaterTessellationShader.h"
#include "SkyShader.h"


class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);

	bool frame();

protected:


	bool render();

	void depthPass();
	void firstPass();
	void finalPass();
	
	void gui();

private:
	
	bool drunk;

	TessellationPlane* terrain;
	TessellationPlane* water;

	AModel* cube;
	SphereMesh* debugSphere;

	AModel* beachHut;
	AModel* light;
	AModel* stool;
	AModel* turtle;
	AModel* shelf;
	AModel* bottle1;
	AModel* bottle2;

	RenderTexture* sceneTexture;
	OrthoMesh* playerView;

	LightShader* lightShader;
	WaterShader* waterShader;
	DepthShader* depthShader;
	TextureShader* textureShader;
	TerrainDepthShader* terrainDepthShader;
	DrunkShader* drunkShader;
	TerrainTessellationShader* terrainTessellationShader;
	WaterTessellationShader* waterTessellationShader;
	SkyShader* skyShader;

	Light* lights[LIGHTCOUNT];
	ShadowMap* shadowMaps[LIGHTCOUNT];

	ShadowMap* cameraDepth;

	float debuglightPos[3];

	float elapsedTime;

	int tessAmount;
};

#endif