#ifndef AN_CAMERA_2D_H_
#define AN_CAMERA_2D_H_

#include "Math/anMatrix4.h"

class anCamera2D
{
public:
	anCamera2D();
	anCamera2D(float left, float right, float top, float bottom);

	void SetOrtho(float left, float right, float top, float bottom);
	void SetPosition(const anFloat2& pos);
	void SetRotation(float rot);
	void Move(const anFloat2& p);
	void Rotate(float r);
	const anFloat2& GetPosition() const;
	float GetRotation() const;
	void CalculateViewMatrix();
	void CalculateProjectionMatrix();
	void SetZoomLevel(float zoomLevel);
	void IncreaseZoomLevel(float incVal);
	const anMatrix4& GetProjectionMatrix() const;
	const anMatrix4& GetViewMatrix() const;
	float GetZoomLevel() const;
private:
	anMatrix4 mProjectionMatrix;
	anMatrix4 mViewMatrix;

	anFloat2 mPosition = { 0.0f, 0.0f };
	float mRotation = 0.0f;
	float mZoom = 1.0f;

	float mLeft = 0.0f;
	float mRight = 0.0f;
	float mTop = 0.0f;
	float mBottom = 0.0f;
};

#endif
