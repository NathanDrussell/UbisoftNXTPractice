//------------------------------------------------------------------------
// GameTest.cpp
//------------------------------------------------------------------------
#include "stdafx.h"
//------------------------------------------------------------------------
#include <windows.h> 
#include <math.h>  
#include <vector>
#include "Player.h"
//------------------------------------------------------------------------
#include "app\app.h"






Player player;
std::vector<POINT> bullets;
//------------------------------------------------------------------------
// Called before first update. Do any initial setup here.
//------------------------------------------------------------------------
void Init()
{

}


//------------------------------------------------------------------------
// Update your simulation here. deltaTime is the elapsed time since the last update in ms.
// This will be called at no greater frequency than the value of APP_MAX_FRAME_RATE
//------------------------------------------------------------------------
bool orint = false;
void Update(float deltaTime)
{
	player.arrow.rotateArrow();
	if (App::IsKeyPressed(VK_LBUTTON))
	{

	}
	if (App::IsKeyPressed('W'))
	{
		player.up();
	}
	if (App::IsKeyPressed('S'))
	{
		player.down();
	}
	if (App::IsKeyPressed('A'))
	{
		player.left();
	}
	if (App::IsKeyPressed('D'))
	{
		player.right();
	}

}


//------------------------------------------------------------------------
// Add your display calls here (DrawLine or Print) 
// See App.h 
//------------------------------------------------------------------------
void Render()
{	
	POINT p = player.position;
	App::DrawLine(p.x, p.y, p.x, p.y + 1, 0.0f, 1.0f, 0.0f);
	player.arrow.DrawArrow();
}

void Shutdown()
{
}