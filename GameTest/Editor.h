#pragma once

class CPoint;

enum EditorState
{
	eState_Edit,
	eState_Add,
	eState_Save,
};

namespace Editor
{
	void DrawBoxAroundPoint(const CPoint& point, float size);

	EditorState UpdateMoveLine(float x, float y);
	EditorState UpdateAddLine(float x, float y);
	EditorState Save(const char* fileName);
	void Load(const char* fileName);
}
