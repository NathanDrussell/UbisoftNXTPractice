#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "stdafx.h"
#include <Windows.h>
#include "Arrow.h"
#include "App\AppSettings.h"


class Player
{
public:
	POINT position = { APP_VIRTUAL_WIDTH / 2, APP_VIRTUAL_HEIGHT / 2 };
public:
	Arrow arrow = Arrow(position);
	Player()
	{
	}
	void up()
	{
		position.y += 1;
		arrow.updatePosition(position);
		arrow.rotateArrow();
	}
	void down()
	{
		position.y -= 1;
		arrow.updatePosition(position);
		arrow.rotateArrow();
	}
	void left()
	{
		position.x -= 1;
		arrow.updatePosition(position);
		arrow.rotateArrow();
	}
	void right()
	{
		position.x += 1;
		arrow.updatePosition(position);
		arrow.rotateArrow();
	}
};
#endif // !1
