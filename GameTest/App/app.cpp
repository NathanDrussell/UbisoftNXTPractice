//---------------------------------------------------------------------------------
// App.cpp
// Implementation of the calls that are exposed via the App namespace.
//---------------------------------------------------------------------------------
//#include ""
#include "stdafx.h"
//---------------------------------------------------------------------------------
#include <string>
#include "main.h"
#include "app.h"
#include "SimpleSound.h"
#include "SimpleController.h"

//---------------------------------------------------------------------------------
// Utils and externals for system info.
#define APP_VIRTUAL_TO_NATIVE_COORDS(_x_,_y_)			_x_ = ((_x_ / APP_VIRTUAL_WIDTH )*2.0f) - 1.0f; _y_ = ((_y_ / APP_VIRTUAL_HEIGHT)*2.0f) - 1.0f;
#define APP_NATIVE_TO_VIRTUAL_COORDS(_x_,_y_)			_x_ = ((_x_ + 1.0f) * APP_VIRTUAL_WIDTH) / 2.0f; _y_ = ((_y_ + 1.0f) * APP_VIRTUAL_HEIGHT) / 2.0f;

namespace App
{	
	void DrawLine(float sx, float sy, float ex, float ey, float r, float g, float b)
	{
#if APP_USE_VIRTUAL_RES		
		APP_VIRTUAL_TO_NATIVE_COORDS(sx, sy);
		APP_VIRTUAL_TO_NATIVE_COORDS(ex, ey);
#endif
		glBegin(GL_LINES);
		glColor3f(r, g, b); // Yellow
		glVertex2f(sx, sy);
		glVertex2f(ex, ey);
		glEnd();
	}

	void DrawLine(POINT source, POINT end, float r, float g, float b)
	{
#if APP_USE_VIRTUAL_RES		
		APP_VIRTUAL_TO_NATIVE_COORDS(source.x, source.y);
		APP_VIRTUAL_TO_NATIVE_COORDS(end.x, end.y);
#endif
		glBegin(GL_LINES);
		glColor3f(r, g, b); // Yellow
		glVertex2f(source.x, source.y);
		glVertex2f(end.x, end.y);
		glEnd();
	}

	void DrawTri(float ax, float ay, float bx, float by, float cx, float cy, float r, float g, float b)
	{
#if APP_USE_VIRTUAL_RES		
		APP_VIRTUAL_TO_NATIVE_COORDS(ax, ay);
		APP_VIRTUAL_TO_NATIVE_COORDS(bx, by);
		APP_VIRTUAL_TO_NATIVE_COORDS(cx, cy);
#endif
		glBegin(GL_TRIANGLES);
		glColor3f(r, g, b); // Yellow
		glVertex2f(ax, ay);
		glVertex2f(bx, by);
		glVertex2f(cx, cy);
		glEnd();
	}

	bool IsKeyPressed(int key)
	{
		return ((GetAsyncKeyState(key) & 0x8000) != 0);
	}

	void GetMousePos(float &x, float &y)
	{
		POINT mousePos;
		GetCursorPos(&mousePos);	// Get the mouse cursor 2D x,y position			
		ScreenToClient(MAIN_WINDOW_HANDLE, &mousePos);
		x = (float)mousePos.x;
		y = (float)mousePos.y;
		x = (x * (2.0f / WINDOW_WIDTH) - 1.0f);
		y = -(y * (2.0f / WINDOW_HEIGHT) - 1.0f);

#if APP_USE_VIRTUAL_RES		
		APP_NATIVE_TO_VIRTUAL_COORDS(x, y);
#endif
	}
	void PlaySound(const char *fileName, bool looping)
	{
		DWORD flags = (looping) ? DSBPLAY_LOOPING : 0;
		CSimpleSound::GetInstance().PlaySound(fileName, flags);
	}
	void StopSound(const char *fileName)
	{
		CSimpleSound::GetInstance().StopSound(fileName);
	}
	bool IsSoundPlaying(const char *fileName)
	{
		return CSimpleSound::GetInstance().IsPlaying(fileName);
	}
	// This prints a string to the screen
	void Print(float x, float y, const char *st, float r, float g, float b, void *font)
	{
#if APP_USE_VIRTUAL_RES		
		APP_VIRTUAL_TO_NATIVE_COORDS(x, y);
#endif		
		// Set location to start printing text
		glColor3f(r, g, b); // Yellow
		glRasterPos2f(x, y);
		int l = (int)strlen(st);
		for (int i = 0; i < l; i++)
		{
			glutBitmapCharacter(font, st[i]); // Print a character on the screen
		}
	}
	const CController &GetController( int pad )
	{
		return CSimpleControllers::GetInstance().GetController(pad);
	}
}