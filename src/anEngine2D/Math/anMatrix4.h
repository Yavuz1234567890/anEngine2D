#ifndef AN_MATRIX4_H_
#define AN_MATRIX4_H_

#include "anFloat4.h"
#include "anFloat3.h"
#include "anFloat2.h"

struct anMatrix4
{
	anFloat4 Rows[4];

	anMatrix4();
	anMatrix4(float x);
	anMatrix4(const anFloat4& row1, const anFloat4& row2, const anFloat4& row3, const anFloat4& row4);

	void Reset();
	anMatrix4 operator*(const anMatrix4& mat) const;
	float* GetAddress();
	const float* GetAddress() const;
	anFloat4& operator[](anUInt32 index);
	const anFloat4& operator[](anUInt32 index) const;
	anMatrix4& operator=(const anMatrix4& rhs);

	static anMatrix4 Translate(const anFloat3& pos);
	static anMatrix4 Scale(const anFloat3& size);
	static anMatrix4 Rotate(float angle, const anFloat3& axis);
	static anMatrix4 Ortho(float left, float right, float bottom, float top, float near, float far);
	static anMatrix4 Inverse(const anMatrix4& matrix);
	static anMatrix4 LookAt(const anFloat3& camera, const anFloat3& object, const anFloat3& up);
};

anFloat4 operator*(const anFloat4& f, const anMatrix4& m);
anFloat3 operator*(const anFloat3& f, const anMatrix4& m);
anFloat2 operator*(const anFloat2& f, const anMatrix4& m);

#endif
