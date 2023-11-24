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

	OrthoMesh* playerView;

	//TessellationPlane* water;

	TerrainShader* terrainShader;
	LightShader* lightShader;
	WaterShader* waterShader;
	DepthShader* depthShader;
	TextureShader* textureShader;

	Light* light1;
	Light* light2;
	Light* light3;

	ShadowMap* shadowMap;

	float debuglightPos[3];

	float elapsedTime;
};

#endif