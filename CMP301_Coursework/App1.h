// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "DXF.h"	// include dxframework
#include "LightShader.h"
#include "TerrainShader.h"
#include "WaterShader.h"
#include "TesselationPlane.h"


class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);

	bool frame();

protected:


	bool render();

	void firstPass();
	void finalPass();
	
	void gui();

private:
	
	PlaneMesh* terrain;
	PlaneMesh* water;
	//TessellationPlane* water;

	TerrainShader* terrainShader;
	LightShader* lightShader;
	WaterShader* waterShader;

	Light* light1;
	Light* light2;
	Light* light3;

	float elapsedTime;
};

#endif