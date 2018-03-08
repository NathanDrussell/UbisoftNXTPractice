#ifndef _ARROW_H_
#define _ARROW_H_

#include <Windows.h>
#include <math.h>
#include "App\app.h"
class Arrow
{
private:
	POINT playerPosition;
	POINT origRight[2];
	POINT origLeft[2];
public:
	float rotation;
	POINT arrowLeftSidePoints[2];
	POINT arrowRightSidePoints[2];

	Arrow(POINT pos)
	{
		playerPosition = pos;

		setArrays();

	}
	void setArrays()
	{
		arrowLeftSidePoints[0] = { playerPosition.x + 10, playerPosition.y };
		arrowLeftSidePoints[1] = { playerPosition.x + 5, playerPosition.y + 5 };

		arrowRightSidePoints[0] = { playerPosition.x + 10, playerPosition.y };
		arrowRightSidePoints[1] = { playerPosition.x + 5, playerPosition.y - 5 };

		memcpy(&origLeft, &arrowLeftSidePoints, sizeof(arrowLeftSidePoints));
		memcpy(&origRight, &arrowRightSidePoints, sizeof(arrowRightSidePoints));

	}
	void updatePosition(POINT pos)
	{
		playerPosition = pos;
		setArrays();
	}
	void resetRotation()
	{
		memcpy(&arrowLeftSidePoints, &origLeft, sizeof(origLeft));
		memcpy(&arrowRightSidePoints, &origRight, sizeof(origRight));
	}
	void DrawArrow()
	{
		App::DrawLine(arrowLeftSidePoints[0].x, arrowLeftSidePoints[0].y, arrowLeftSidePoints[1].x, arrowLeftSidePoints[1].y);
		App::DrawLine(arrowRightSidePoints[0].x, arrowRightSidePoints[0].y, arrowRightSidePoints[1].x, arrowRightSidePoints[1].y);
		resetRotation();

	}
	void rotate(POINT &point, double radians, POINT origin)
	{
		float cosResult = cos(radians);
		float sinResult = sin(radians);

		point.x -= origin.x;
		point.y -= origin.y;

		float xRes = (cosResult * point.x) - (sinResult * point.y);
		float yRes = (sinResult * point.x) + (cosResult * point.y);

		point.x = xRes + origin.x;
		point.y = yRes + origin.y;
	}
public:
	void rotateArrow()
	{
		float mouseX = 0;
		float mouseY = 0;

		App::GetMousePos(mouseX, mouseY);

		float deltaX = mouseX - playerPosition.x;
		float deltaY = mouseY - playerPosition.y;

		rotation = atan2(deltaY, deltaX);

		rotate(arrowLeftSidePoints[0], rotation, playerPosition);
		rotate(arrowLeftSidePoints[1], rotation, playerPosition);

		rotate(arrowRightSidePoints[0], rotation, playerPosition);
		rotate(arrowRightSidePoints[1], rotation, playerPosition);
	}
};

#endif // !_ARROW_H_
