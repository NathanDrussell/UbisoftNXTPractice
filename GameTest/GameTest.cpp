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
void inputHandling(float deltaTime)
{
	static float time = 0.0f;
	if (App::IsKeyPressed(VK_LBUTTON) && time >= FIRE_DELAY)
	{
		bullets.push_back(POINT{ 0, 0 });
		time = 0.0f;
	}
	else
	{
		time += deltaTime;
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

void Update(float deltaTime)
{
	player.arrow.rotateArrow();
	inputHandling(deltaTime);
	for (std::vector<POINT>::iterator it = bullets.begin(); it != bullets.end(); ++it)
	{
		(*it).x += 1;
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
	for (std::vector<POINT>::iterator it = bullets.begin(); it != bullets.end(); ++it)
	{
		POINT temp = *it;
		App::DrawLine(temp.x, temp.y, temp.x + BULLET_LENGTH, temp.y + BULLET_LENGTH);
		ZeroMemory(&temp, sizeof(temp));
	}
}

void Shutdown()
{
}