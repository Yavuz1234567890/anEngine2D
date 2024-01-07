#ifndef AN_GIZMO_SYSTEM_H_
#define AN_GIZMO_SYSTEM_H_

#include "State/anState.h"
#include "Scene/anScene.h"
#include "Scene/anEntity.h"

class anEditorState;

namespace anGizmoType
{
	enum : anUInt32
	{
		Translate,
		Scale,
		Rotate
	};
}

class anGizmoSystem
{
public:
	anGizmoSystem();
	~anGizmoSystem();

	void Initialize(anEditorState* editor);
	void UpdateGizmos(float dt, anFloat2 exactMousePosition);
	void OnEvent(const anEvent& event);
	anUInt32 GetGizmoType() const;
	void SetGizmoType(anUInt32 type);
	bool IsUsing() const;
private:
	anEditorState* mEditor;
	
	anFloat2 mExactViewportMousePosition;
	anFloat2 mLastExactViewportMousePosition;
	
	anFloat2 mLeftMouseTickedViewportMousePosition;
	anFloat2 mLeftMouseTickedGizmoPosition;

	bool mLeftMouseButtonPressed = false;

	anUInt32 mGizmoType = -1;
	anFloat2 mGizmoPosition;
	bool mGizmoHorizontalTick = false;
	bool mGizmoVerticalTick = false;
	bool mGizmoIsUsing = false;
	float mGizmoSize = 100.0f;
	float mGizmoRectSize = mGizmoSize * 0.2f;
	anFloat2 mGizmoBoxPosition;
	bool mGizmoBoxTick = false;
};

#endif
