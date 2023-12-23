#include "anCamera2D.h"

anCamera2D::anCamera2D()
	: mViewMatrix(1.0f)
{
}

anCamera2D::anCamera2D(float left, float right, float top, float bottom)
	: mViewMatrix(1.0f)
{
	SetOrtho(left, right, top, bottom);
}

void anCamera2D::SetOrtho(float left, float right, float top, float bottom)
{
	mLeft = left;
	mRight = right;
	mTop = top;
	mBottom = bottom;

	mProjectionMatrix = glm::ortho(left * mZoom, right * mZoom, bottom * mZoom, top * mZoom);
}

void anCamera2D::SetPosition(const anFloat2& pos)
{
	mPosition = pos;
	CalculateViewMatrix();
}

void anCamera2D::SetRotation(float rot)
{
	mRotation = rot;
	CalculateViewMatrix();
}

void anCamera2D::Move(const anFloat2& p)
{
	mPosition += p;
	CalculateViewMatrix();
}

void anCamera2D::Rotate(float r)
{
	mRotation += r;
	CalculateViewMatrix();
}

const anFloat2& anCamera2D::GetPosition() const
{
	return mPosition;
}

float anCamera2D::GetRotation() const
{
	return mRotation;
}

void anCamera2D::CalculateViewMatrix()
{
	anMatrix4 transform = glm::translate(anMatrix4(1.0f), { mPosition.x, mPosition.y, 0.0f }) *
							glm::rotate(anMatrix4(1.0f), glm::radians(mRotation), { 0.0f, 0.0f, 1.0f });

	mViewMatrix = glm::inverse(transform);
}

void anCamera2D::CalculateProjectionMatrix()
{
	mProjectionMatrix = glm::ortho(mLeft * mZoom, mRight * mZoom, mBottom * mZoom, mTop * mZoom);
}

void anCamera2D::SetZoomLevel(float zoomLevel)
{
	if (zoomLevel > 0.0f)
		mZoom = zoomLevel;

	CalculateProjectionMatrix();
}

void anCamera2D::IncreaseZoomLevel(float incVal)
{
	if (mZoom + incVal > 0.0f)
		mZoom += incVal;

	CalculateProjectionMatrix();
}

const anMatrix4& anCamera2D::GetProjectionMatrix() const
{
	return mProjectionMatrix;
}

const anMatrix4& anCamera2D::GetViewMatrix() const
{
	return mViewMatrix;
}

float anCamera2D::GetZoomLevel() const
{
	return mZoom;
}
