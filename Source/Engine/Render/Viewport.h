#pragma once
#include "ThirdParty.h"


class Viewport
{
public:
	Viewport() = default;
	~Viewport() = default;

	void CreateViewport(const char* appName, u32 winWidth, u32 winHeight);
	Window* GetWindow() { return m_pWindow; }
	void DestroyViewport();

private:
	Window* m_pWindow = nullptr;
	const char* m_Name = "No Viewport Name Specified";
	u32 m_Width = 0;
	u32 m_Height = 0;

public:
	// Set window limits between 480p and 16k res. Raise Max later when surveying if hardware supports it
	static const u32 minWindowWidth = 640;
	static const u32 minWindowHeight = 480;
	static const u32 initialMaxWindowWidth = 4096 * 4; 
	static const u32 initialMaxWindowHeight = 4096 * 4;
};

