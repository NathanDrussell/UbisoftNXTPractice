//------------------------------------------------------------------------
// Editor.cpp
//------------------------------------------------------------------------
#include "stdafx.h"
//------------------------------------------------------------------------
#include <windows.h> 
#include <math.h>  
#include <algorithm>
#include <assert.h>
#include <fstream>
//------------------------------------------------------------------------
#include "app\app.h"
#include "table.h"
#include "Editor.h"

CTable* gTable;
LineType gDefaultLineType = eLine_Hard;
const char* gControlsMessage = "Controls\n"
	"Left-Click - Select Line, drag points\n"
	"Shift - Hold to disable snap to point\n"
	"Shift - Hold to drag just this point\n"
	"Ctrl - Create new line with each click\n"
	"Ctrl-S Save data\n"
	"Delete - delete selected line\n"
	"1-6 change type of selected line";
const char* gStatusMessage = gControlsMessage;
const char* gDefaultFileName = "table.txt";
const int VERSION = 1;
const float PICK_TOLERANCE = 5.0f;
const float SNAP_TOLERANCE = 8.0f;
const float POINT_EPSILON = 0.01f;

//------------------------------------------------------------------------
// Called before first update. Do any initial setup here.
//------------------------------------------------------------------------
void EditorInit()
{
	gTable = new CTable;
	Editor::Load(gDefaultFileName);


}

//------------------------------------------------------------------------
// Update your simulation here. deltaTime is the elapsed time since the last update in ms.
// This will be called at no greater frequency than the value of APP_MAX_FRAME_RATE
//------------------------------------------------------------------------
void EditorUpdate(float deltaTime)
{
	static EditorState sState = eState_Edit;

	// Controls
	// Left-Click - Select Line, drag points
	// Shift - Hold to disable snap to point
	// Shift - Hold to drag just this point
	// Ctrl - Create new line with each click
	// Ctrl-S Save data
	// Delete - delete selected line
	// 1-6 change type of selected line

	float x, y;
	App::GetMousePos(x, y);

	switch (sState)
	{
	case eState_Edit:
		sState = Editor::UpdateMoveLine(x, y);
		break;
	case eState_Add:
		sState = Editor::UpdateAddLine(x, y);
		break;
	case eState_Save:
		sState = Editor::Save(gDefaultFileName);
		break;
	default:
		assert(false);
	}
}

EditorState Editor::UpdateMoveLine(float x, float y)
{
	if (App::IsKeyPressed(VK_CONTROL))
	{
		if (App::IsKeyPressed('S'))
		{
			return eState_Save;
		}
		return eState_Add;
	}

	CLineSegment* selectedLine = NULL;
	for (auto& line : gTable->m_lines)
	{
		if (line.m_selected)
		{
			selectedLine = &line;
		}
	}

	if (App::IsKeyPressed(VK_DELETE))
	{
		gTable->m_lines.erase(
			std::remove_if(
				gTable->m_lines.begin(), gTable->m_lines.end(),
				[](CLineSegment x) {return x.m_selected;}),
			gTable->m_lines.end());
		gStatusMessage = gControlsMessage;
		return eState_Edit;
	}

	if (selectedLine)
	{
		for (int i = 0; i < eLine_END; ++i)
		{
			if (App::IsKeyPressed('0' + i))
			{
				selectedLine->m_type = static_cast<LineType>(i);
				selectedLine->m_selected = false;
				selectedLine = NULL;
				gDefaultLineType = static_cast<LineType>(i);
			}
		}
	}

	if (App::IsKeyPressed(VK_LBUTTON))
	{
		CPoint* selectedPoint = NULL;
		if (selectedLine)
		{
			if (selectedLine->m_start.m_selected)
			{
				selectedPoint = &selectedLine->m_start;
			}
			else if (selectedLine->m_end.m_selected)
			{
				selectedPoint = &selectedLine->m_end;
			}
		}

		// We have a selected point while we are dragging
		if (selectedLine && selectedPoint)
		{
			float snappedX = x;
			float snappedY = y;

			// Snap moving point to close points (on any other line)
			if (!App::IsKeyPressed(VK_LSHIFT))
			{
				for (auto& testLine : gTable->m_lines)
				{
					if (!testLine.m_selected)
					{
						if (!testLine.m_start.m_selected && testLine.m_start.IsOnPoint(x, y, SNAP_TOLERANCE))
						{
							snappedX = testLine.m_start.m_x;
							snappedY = testLine.m_start.m_y;
							continue;
						}
						if (!testLine.m_end.m_selected &&testLine.m_end.IsOnPoint(x, y, SNAP_TOLERANCE))
						{
							snappedX = testLine.m_end.m_x;
							snappedY = testLine.m_end.m_y;
							continue;
						}
					}
				}
			}

			// move all selected points
			for (auto& line : gTable->m_lines)
			{
				if (line.m_start.m_selected)
				{
					line.m_start.m_x = snappedX;
					line.m_start.m_y = snappedY;
				}
				if (line.m_end.m_selected)
				{
					line.m_end.m_x = snappedX;
					line.m_end.m_y = snappedY;
				}
				gStatusMessage = gControlsMessage;
			}
		}
		else if (selectedLine)
		{
			// no point selected, select a point close to the cursor
			if (!App::IsKeyPressed(VK_LSHIFT))
			{
				// select all points that are close
				for (auto& line : gTable->m_lines)
				{
					if (line.m_start.IsOnPoint(x, y, PICK_TOLERANCE))
					{
						line.m_start.m_selected = true;
						if (&line == selectedLine)
						{
							selectedPoint = &selectedLine->m_start;
						}
					}
					else if (line.m_end.IsOnPoint(x, y, PICK_TOLERANCE))
					{
						line.m_end.m_selected = true;
						if (&line == selectedLine)
						{
							selectedPoint = &selectedLine->m_end;
						}
					}
				}
			}
			else
			{
				// Shift stops dragging both points
				if (selectedLine->m_start.IsOnPoint(x, y, PICK_TOLERANCE))
				{
					selectedLine->m_start.m_selected = true;
					selectedPoint = &selectedLine->m_start;
				}
				else if (selectedLine->m_end.IsOnPoint(x, y, PICK_TOLERANCE))
				{
					selectedLine->m_end.m_selected = true;
					selectedPoint = &selectedLine->m_end;
				}
			}
		}

		if (!selectedPoint)
		{
			// Select closest line
			CLineSegment* closestLine = NULL;
			float closestDist = 10.0f;
			for (auto& line : gTable->m_lines)
			{
				float distance = line.DistanceToLine(x, y);
				if (distance < closestDist)
				{
					closestDist = distance;
					closestLine = &line;
				}
			}
			if (closestLine)
			{
				for (auto& line : gTable->m_lines)
				{
					line.m_selected = false;
				}
				closestLine->m_selected = true;
			}
		}
	}
	else
	{
		// Remove all point selection on mouse-up
		for (auto& line : gTable->m_lines)
		{
			if (line.m_start.m_selected || line.m_end.m_selected)
			{
				line.m_start.m_selected = false;
				line.m_end.m_selected = false;
			}
		}
	}
	return eState_Edit;
}

//------------------------------------------------

EditorState Editor::UpdateAddLine(float x, float y)
{
	CLineSegment* selectedLine = NULL;
	CPoint* selectedPoint = NULL;
	for (auto& line : gTable->m_lines)
	{
		if (line.m_selected)
		{
			selectedLine = &line;
		}

		if (line.m_start.m_selected)
		{
			selectedPoint = &line.m_start;
		}
		else if (line.m_end.m_selected)
		{
			selectedPoint = &line.m_end;
		}
	}

	// Find a snapped point
	float snappedX = x;
	float snappedY = y;
	for (auto& testLine : gTable->m_lines)
	{
		if (!testLine.m_selected)
		{
			if (!testLine.m_start.m_selected && testLine.m_start.IsOnPoint(x, y, SNAP_TOLERANCE))
			{
				snappedX = testLine.m_start.m_x;
				snappedY = testLine.m_start.m_y;
				continue;
			}
			if (!testLine.m_end.m_selected &&testLine.m_end.IsOnPoint(x, y, SNAP_TOLERANCE))
			{
				snappedX = testLine.m_end.m_x;
				snappedY = testLine.m_end.m_y;
				continue;
			}
		}
	}

	if (!App::IsKeyPressed(VK_CONTROL) || App::IsKeyPressed('S'))
	{
		// When releasing CTRL, the user may have a 'dangling' zero length line
		if (selectedPoint)
		{
			CLineSegment* zeroLengthLine = &gTable->m_lines.back();
			if (zeroLengthLine->m_start.IsOnPoint(zeroLengthLine->m_end.m_x, zeroLengthLine->m_end.m_y, POINT_EPSILON))
			{
				gTable->m_lines.pop_back();
			}
		}
		if (App::IsKeyPressed('S'))
		{
			return eState_Save;
		}
		else
		{
			return eState_Edit;
		}
	}

	for (int i = 0; i < eLine_END; ++i)
	{
		if (App::IsKeyPressed('0' + i))
		{
			gDefaultLineType = static_cast<LineType>(i);
		}
	}


	if (App::IsKeyPressed(VK_LBUTTON))
	{
		if (!selectedLine)
		{
			// add new line
			if (selectedPoint)
			{
				selectedPoint->m_selected = false;
				gTable->m_lines.push_back(CLineSegment(selectedPoint->m_x, selectedPoint->m_y, x, y, gDefaultLineType));
			}
			else
			{
				gTable->m_lines.push_back(CLineSegment(snappedX, snappedY, x, y, gDefaultLineType));
			}
			selectedLine = &(gTable->m_lines.back());
			selectedLine->m_selected = true;
			selectedLine->m_end.m_selected = true;
		}
	}
	else
	{
		if (selectedLine)
		{
			selectedLine->m_selected = false;
		}
	}

	if (selectedPoint)
	{
		// Snap moving point to close points (on any other line)
		if (!App::IsKeyPressed(VK_LSHIFT))
		{
			selectedPoint->m_x = snappedX;
			selectedPoint->m_y = snappedY;
		}
		else
		{
			selectedPoint->m_x = x;
			selectedPoint->m_y = y;
		}
		gStatusMessage = gControlsMessage;
	}

	return eState_Add;
}

EditorState Editor::Save(const char* fileName)
{
	if (!App::IsKeyPressed('S'))
	{
		std::fstream file;
		file.open(fileName, std::fstream::out);
		if (file.is_open())
		{
			file << VERSION;
			for (auto& line : gTable->m_lines)
			{
				file << '\n';
				file << line.m_type << ' ';
				file << line.m_start.m_x << ' ';
				file << line.m_start.m_y << ' ';
				file << line.m_end.m_x << ' ';
				file << line.m_end.m_y;
			}

			file.close();
			gStatusMessage = "SAVED";
		}
		else
		{
			// error message!
			gStatusMessage = "SAVE FAILED";
		}

		return eState_Edit;
	}
	return eState_Save;
}


void Editor::Load(const char* fileName)
{
	std::fstream file;
	file.open(fileName, std::fstream::in);
	if (file.is_open())
	{
		int version = 0;
		file >> version;
		if (version <= 1)
		{
			gTable->m_lines.clear();
			while (!file.eof())
			{
				CLineSegment line;
				int type;
				file >> type;
				line.m_type = static_cast<LineType>(type);
				file >> line.m_start.m_x;
				file >> line.m_start.m_y;
				file >> line.m_end.m_x;
				file >> line.m_end.m_y;
				gTable->m_lines.push_back(line);
			}

			gStatusMessage = "LOADED OK";
		}
		else
		{
			gStatusMessage = "LOAD FAILED: UNHANDLED VERSION";
		}

		file.close();
	}
	else
	{
		// error message!
		gStatusMessage = "LOAD FAILED";
	}

}

//------------------------------------------------------------------------
// Add your display calls here (DrawLine or Print) 
// See App.h 
//------------------------------------------------------------------------
void EditorRender()
{
	float y = 740.0f;
	float dy = -12.0f;
	char buffer[256];
	char* displayString = buffer;
	char* test = buffer;
	strcpy_s(buffer, 255, gStatusMessage);
	int length = static_cast<int>(strlen(gStatusMessage));
	while (test - buffer <= length)
	{
		if (*test == '\n' || *test == 0)
		{
			*test = 0;
			App::Print(10.0f, y, displayString, 1.0f, 1.0f, 1.0f, GLUT_BITMAP_8_BY_13);
			displayString = test + 1;
			y += dy;
		}
		++test;
	}
	


//	y = 700.0f;

	for (auto& line : gTable->m_lines)
	{
		if (line.m_selected)
		{
			App::DrawLine(line.m_start.m_x, line.m_start.m_y, line.m_end.m_x, line.m_end.m_y, 0.0f, 0.0f);
			Editor::DrawBoxAroundPoint(line.m_start, 6.0f);
			Editor::DrawBoxAroundPoint(line.m_end, 6.0f);

//			char textBuffer[256];
//			sprintf(textBuffer, "%0.1f, %0.1f - %0.1f, %0.1f", line.m_start.m_x, line.m_start.m_y, line.m_end.m_x, line.m_start.m_y);
//			App::Print(10.0f, y, textBuffer, 0.0f, 0.0f, 1.0f, GLUT_BITMAP_8_BY_13);
		}
		else
		{
			CLineDefinition& def = gTable->m_lineDefs[line.m_type];
			App::DrawLine(line.m_start.m_x, line.m_start.m_y, line.m_end.m_x, line.m_end.m_y, def.m_Red, def.m_Green, def.m_Blue);
//			char textBuffer[256];
//			sprintf(textBuffer, "%0.1f, %0.1f - %0.1f, %0.1f", line.m_start.m_x, line.m_start.m_y, line.m_end.m_x, line.m_start.m_y);
//			App::Print(10.0f, y, textBuffer, 1.0f, 1.0f, 1.0f, GLUT_BITMAP_8_BY_13);
		}
		y += dy;
	}
}



//------------------------------------------------------------------------
// Add your shutdown code here. Called when the APP_QUIT_KEY is pressed.
// Just before the app exits.
//------------------------------------------------------------------------
void EditorShutdown()
{
	if (strcmp(gStatusMessage, "SAVED") != 0)
	{
		Editor::Save("backup.txt");
	}
	delete gTable;
}

void Editor::DrawBoxAroundPoint(const CPoint & point, float size)
{
	float blueGreen = point.m_selected ? 0.0f : 1.0f;
	float radius = size / 2.0f;
	float x = point.m_x;
	float y = point.m_y;
	App::DrawLine(x - radius, y - radius, x + radius, y - radius, 1.0f, blueGreen, blueGreen);
	App::DrawLine(x + radius, y - radius, x + radius, y + radius, 1.0f, blueGreen, blueGreen);
	App::DrawLine(x + radius, y + radius, x - radius, y + radius, 1.0f, blueGreen, blueGreen);
	App::DrawLine(x - radius, y + radius, x - radius, y - radius, 1.0f, blueGreen, blueGreen);
}
