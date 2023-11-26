// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "DXF.h"	// include dxframework
#include "LightShader.h"
#include "TerrainShader.h"
#include "WaterShader.h"
#include "TextureShader.h"
#include "TesselationPlane.h"
#include "DepthShader.h"
#include "TerrainDepthShader.h"


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
	
	PlaneMesh* terrain;
	PlaneMesh* water;

	CubeMesh* cube;

	AModel* beachHut;
	AModel* light;

	OrthoMesh* playerView;

	//TessellationPlane* water;

	TerrainShader* terrainShader;
	LightShader* lightShader;
	WaterShader* waterShader;
	DepthShader* depthShader;
	TextureShader* textureShader;
	TerrainDepthShader* terrainDepthShader;

	Light* lights[LIGHTCOUNT];
	ShadowMap* shadowMaps[LIGHTCOUNT];

	float debuglightPos[3];

	float elapsedTime;
};

#endif