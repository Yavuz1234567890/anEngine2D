#ifndef AN_MATRIX4_H_
#define AN_MATRIX4_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext/matrix_common.hpp>

typedef glm::mat4 anMatrix4;

#include "anFloat4.h"
#include "anFloat3.h"
#include "anFloat2.h"

struct anFloat16
{
	float F00;
	float F01;
	float F02;
	float F03;
	float F10;
	float F11;
	float F12;
	float F13;
	float F20;
	float F21;
	float F22;
	float F23;
	float F30;
	float F31;
	float F32;
	float F33;

	anFloat16()
	{
		F00 = 0.0f;
		F01 = 0.0f;
		F02 = 0.0f;
		F03 = 0.0f;
		F10 = 0.0f;
		F11 = 0.0f;
		F12 = 0.0f;
		F13 = 0.0f;
		F20 = 0.0f;
		F21 = 0.0f;
		F22 = 0.0f;
		F23 = 0.0f;
		F30 = 0.0f;
		F31 = 0.0f;
		F32 = 0.0f;
		F33 = 0.0f;
	}

	anFloat16(float diagonal)
	{
		F00 = diagonal;
		F01 = 0.0f;
		F02 = 0.0f;
		F03 = 0.0f;
		F10 = 0.0f;
		F11 = diagonal;
		F12 = 0.0f;
		F13 = 0.0f;
		F20 = 0.0f;
		F21 = 0.0f;
		F22 = diagonal;
		F23 = 0.0f;
		F30 = 0.0f;
		F31 = 0.0f;
		F32 = 0.0f;
		F33 = diagonal;
	}

	anFloat16 operator*(const anFloat16& mat) const
	{
		anFloat16 res;
	
		res.F00 = F00 * mat.F00 + F01 * mat.F10 + F02 * mat.F20 + F03 * mat.F30;
		res.F01 = F00 * mat.F01 + F01 * mat.F11 + F02 * mat.F21 + F03 * mat.F31;
		res.F02 = F00 * mat.F02 + F01 * mat.F12 + F02 * mat.F22 + F03 * mat.F32;
		res.F03 = F00 * mat.F03 + F01 * mat.F13 + F02 * mat.F23 + F03 * mat.F33;
		
		res.F10 = F10 * mat.F00 + F11 * mat.F10 + F12 * mat.F20 + F13 * mat.F30;
		res.F11 = F10 * mat.F01 + F11 * mat.F11 + F12 * mat.F21 + F13 * mat.F31;
		res.F12 = F10 * mat.F02 + F11 * mat.F12 + F12 * mat.F22 + F13 * mat.F32;
		res.F13 = F10 * mat.F03 + F11 * mat.F13 + F12 * mat.F23 + F13 * mat.F33;
		
		res.F20 = F20 * mat.F00 + F21 * mat.F10 + F22 * mat.F20 + F23 * mat.F30;
		res.F21 = F20 * mat.F01 + F21 * mat.F11 + F22 * mat.F21 + F23 * mat.F31;
		res.F22 = F20 * mat.F02 + F21 * mat.F12 + F22 * mat.F22 + F23 * mat.F32;
		res.F23 = F20 * mat.F03 + F21 * mat.F13 + F22 * mat.F23 + F23 * mat.F33;
		
		res.F30 = F30 * mat.F00 + F31 * mat.F10 + F32 * mat.F20 + F33 * mat.F30;
		res.F31 = F30 * mat.F01 + F31 * mat.F11 + F32 * mat.F21 + F33 * mat.F31;
		res.F32 = F30 * mat.F02 + F31 * mat.F12 + F32 * mat.F22 + F33 * mat.F32;
		res.F33 = F30 * mat.F03 + F31 * mat.F13 + F32 * mat.F23 + F33 * mat.F33;
	
		return res;
	}

	anFloat3 operator*(const anFloat4& f4) const
	{
		anFloat3 res;
		res.x = f4.x * F00 + f4.y * F01 + f4.z * F02 + f4.w * F03;
		res.y = f4.x * F10 + f4.y * F11 + f4.z * F12 + f4.w * F13;
		res.z = f4.x * F20 + f4.y * F21 + f4.z * F22 + f4.w * F23;
		return res;
	}
};

anFloat3 operator*(const anFloat4& f4, const anFloat16& mat);
anFloat16 anCreateTransformationMatrix(const anFloat3& pos, const anFloat3& size, float rot);

//
//struct anMatrix4
//{
//	anFloat4 Rows[4];
//
//	anMatrix4();
//	anMatrix4(float x);
//	anMatrix4(const anFloat4& row1, const anFloat4& row2, const anFloat4& row3, const anFloat4& row4);
//
//	void Reset();
//	anMatrix4 operator*(const anMatrix4& mat) const;
//	float* GetAddress();
//	const float* GetAddress() const;
//	anFloat4& operator[](anUInt32 index);
//	const anFloat4& operator[](anUInt32 index) const;
//	anMatrix4& operator=(const anMatrix4& rhs);
//
//	static anMatrix4 Translate(const anFloat3& pos);
//	static anMatrix4 Scale(const anFloat3& size);
//	static anMatrix4 Rotate(float angle, const anFloat3& axis);
//	static anMatrix4 Ortho(float left, float right, float bottom, float top, float near, float far);
//	static anMatrix4 Inverse(const anMatrix4& matrix);
//	static anMatrix4 LookAt(const anFloat3& camera, const anFloat3& object, const anFloat3& up);
//};
//
//anFloat4 operator*(const anFloat4& f, const anMatrix4& m);
//anFloat3 operator*(const anFloat3& f, const anMatrix4& m);
//anFloat2 operator*(const anFloat2& f, const anMatrix4& m);

#endif
