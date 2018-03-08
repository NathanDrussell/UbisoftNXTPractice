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
class Bullet
{
public:
	void rotate(POINT &point, double radians, POINT origin)
	{
		float cosResult = cos(radians);
		float sinResult = sin(radians);

		point.x -= origin.x;
		point.y -= origin.y;

		float xRes = (cosResult * point.x) - (sinResult * point.y);
		float yRes = (sinResult * point.x) + (cosResult * point.y);

		point.x = xRes +origin.x;
		point.y = yRes +origin.y;
	}
	void onUpdate()
	{
		float dy = end.y - position.y;
		float dx = end.x - position.x;
		position.x += dx;
		position.y += dy;
		end = position;
		end.x += BULLET_LENGTH;
		rotate(end, rotation, position);		
	}
	Bullet(POINT p, float angle)
	{
		position = p;
		rotation = angle;
		end = p;
		end.x = position.x + BULLET_LENGTH;
		rotate(end, rotation, position);
	}
	POINT position;
	POINT end;
	float rotation;
};


Player player;
std::vector<Bullet> bullets;
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
		bullets.push_back(Bullet(player.arrow.arrowLeftSidePoints[0], player.arrow.rotation));
		time = 0.0f;
	}
	else
	{
		time += deltaTime / 1000.0f;
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
	for (std::vector<Bullet>::iterator it = bullets.begin(); it != bullets.end(); ++it)
	{
		(*it).onUpdate();
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

	for (std::vector<Bullet>::iterator it = bullets.begin(); it != bullets.end(); ++it)
	{
		App::DrawLine((*it).position.x, (*it).position.y,(*it).end.x, (*it).end.y);
	}
}

void Shutdown()
{
}