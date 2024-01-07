#include "anGizmoSystem.h"
#include "anEditorState.h"
#include "Math/anMath.h"

anGizmoSystem::anGizmoSystem()
{
}

anGizmoSystem::~anGizmoSystem()
{
}

void anGizmoSystem::Initialize(anEditorState* editor)
{
	mEditor = editor;
}

void anGizmoSystem::UpdateGizmos(float dt, anFloat2 exactMousePosition)
{
	mLastExactViewportMousePosition = mExactViewportMousePosition;
	mExactViewportMousePosition = exactMousePosition;

	anEntity& selectedEntity = mEditor->GetSelectedEntity();
	if (selectedEntity.GetHandle() != entt::null && mGizmoType != -1)
	{
		auto& tc = selectedEntity.GetComponent<anTransformComponent>();
		mGizmoPosition = tc.Position;

		mGizmoBoxPosition = { mGizmoPosition.x + 0.5f + mGizmoRectSize * 0.5f, mGizmoPosition.y - 0.5f - mGizmoRectSize * 0.5f };

		anRenderer2D::Get().DrawLine(mGizmoPosition, { mGizmoPosition.x, mGizmoPosition.y - mGizmoSize }, { 0, 255, 0 });
		anRenderer2D::Get().DrawLine(mGizmoPosition, { mGizmoPosition.x + mGizmoSize, mGizmoPosition.y }, { 255, 0, 0 });
		anRenderer2D::Get().DrawTexture(anTexture::GetWhiteTexture(), mGizmoBoxPosition, { mGizmoRectSize, mGizmoRectSize }, { 255, 100, 100 });

		if (!mGizmoHorizontalTick && !mGizmoVerticalTick)
		{
			if (mExactViewportMousePosition.x >= mGizmoPosition.x + 0.5f &&
				mExactViewportMousePosition.x <= mGizmoPosition.x + 0.5f + mGizmoRectSize &&
				mExactViewportMousePosition.y <= mGizmoPosition.y - 0.5f &&
				mExactViewportMousePosition.y >= mGizmoPosition.y - 0.5f - mGizmoRectSize || mGizmoBoxTick)
			{
				if (mLeftMouseButtonPressed)
				{
					mGizmoBoxTick = true;
					if (mGizmoType == anGizmoType::Translate)
						tc.Position = mExactViewportMousePosition + (mLeftMouseTickedGizmoPosition - mLeftMouseTickedViewportMousePosition);
					if (mGizmoType == anGizmoType::Scale)
						tc.Size += (mLastExactViewportMousePosition - mExactViewportMousePosition) * anFloat2(-1.0f, 1.0f);
					if (mGizmoType == anGizmoType::Rotate)
					{
						anFloat2 delta = mGizmoPosition - mExactViewportMousePosition;

						tc.Rotation = anRadiansToDegrees(anAtan2(delta.y, delta.x));
					}
				}
			}
		}

		if (!mGizmoBoxTick)
		{
			if (!mGizmoHorizontalTick)
			{
				if (mExactViewportMousePosition.x >= mGizmoPosition.x - 2.0f && mExactViewportMousePosition.x <= mGizmoPosition.x || mGizmoVerticalTick)
				{
					if (mExactViewportMousePosition.y <= mGizmoPosition.y - 0.5f && mExactViewportMousePosition.y >= mGizmoPosition.y - mGizmoSize || mGizmoVerticalTick)
					{
						if (mLeftMouseButtonPressed)
						{
							mGizmoVerticalTick = true;
							if (mGizmoType == anGizmoType::Translate)
								tc.Position.y = mExactViewportMousePosition.y + (mLeftMouseTickedGizmoPosition.y - mLeftMouseTickedViewportMousePosition.y);
							if (mGizmoType == anGizmoType::Scale)
								tc.Size.y += mLastExactViewportMousePosition.y - mExactViewportMousePosition.y;
							if (mGizmoType == anGizmoType::Rotate)
								tc.Rotation += mLastExactViewportMousePosition.y - mExactViewportMousePosition.y;
						}
					}
				}
			}

			if (!mGizmoVerticalTick)
			{
				if (mExactViewportMousePosition.y >= mGizmoPosition.y && mExactViewportMousePosition.y <= mGizmoPosition.y + 2.0f || mGizmoHorizontalTick)
				{
					if (mExactViewportMousePosition.x >= mGizmoPosition.x + 0.5f && mExactViewportMousePosition.x <= mGizmoPosition.x + mGizmoSize || mGizmoHorizontalTick)
					{
						if (mLeftMouseButtonPressed)
						{
							mGizmoHorizontalTick = true;
							if (mGizmoType == anGizmoType::Translate)
								tc.Position.x = mExactViewportMousePosition.x + (mLeftMouseTickedGizmoPosition.x - mLeftMouseTickedViewportMousePosition.x);
							if (mGizmoType == anGizmoType::Scale)
								tc.Size.x += mExactViewportMousePosition.x - mLastExactViewportMousePosition.x;
							if (mGizmoType == anGizmoType::Rotate)
								tc.Rotation += mExactViewportMousePosition.x - mLastExactViewportMousePosition.x;
						}
					}
				}
			}
		}

		mGizmoIsUsing = mGizmoHorizontalTick || mGizmoVerticalTick || mGizmoBoxTick;
	}
}

void anGizmoSystem::OnEvent(const anEvent& event)
{
	if (event.Type == anEvent::MouseDown)
	{
		if (event.MouseButton == 0)
		{
			mLeftMouseButtonPressed = true;
			mLeftMouseTickedGizmoPosition = mGizmoPosition;
			mLeftMouseTickedViewportMousePosition = mExactViewportMousePosition;
		}
	}

	if (event.Type == anEvent::MouseUp)
	{
		if (event.MouseButton == 0)
		{
			mLeftMouseButtonPressed = false;
			mLeftMouseTickedGizmoPosition = { 0.0f, 0.0f };
			mLeftMouseTickedViewportMousePosition = { 0.0f, 0.0f };
			mGizmoHorizontalTick = false;
			mGizmoVerticalTick = false;
			mGizmoBoxTick = false;
		}
	}
}

bool anGizmoSystem::IsUsing() const
{
	return mGizmoIsUsing;
}

anUInt32 anGizmoSystem::GetGizmoType() const
{
	return mGizmoType;
}

void anGizmoSystem::SetGizmoType(anUInt32 type)
{
	mGizmoType = type;
}
