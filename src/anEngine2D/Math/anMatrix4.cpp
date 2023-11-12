#include "anMatrix4.h"
#include "anMath.h"

anMatrix4::anMatrix4()
{
}

anMatrix4::anMatrix4(float x)
{
	Rows[0][0] = x;
	Rows[1][1] = x;
	Rows[2][2] = x;
	Rows[3][3] = x;
}

anMatrix4::anMatrix4(const anFloat4& row1, const anFloat4& row2, const anFloat4& row3, const anFloat4& row4)
{
	Rows[0] = row1;
	Rows[1] = row2;
	Rows[2] = row3;
	Rows[3] = row4;
}

void anMatrix4::Reset()
{
	Rows[0] = {};
	Rows[1] = {};
	Rows[2] = {};
	Rows[3] = {};
}

anMatrix4 anMatrix4::operator*(const anMatrix4& mat) const
{
	anMatrix4 res;
	
	res[0][0] = Rows[0][0] * mat[0][0] + Rows[0][1] * mat[1][0] + Rows[0][2] * mat[2][0] + Rows[0][3] * mat[3][0];
	res[0][1] = Rows[0][0] * mat[0][1] + Rows[0][1] * mat[1][1] + Rows[0][2] * mat[2][1] + Rows[0][3] * mat[3][1];
	res[0][2] = Rows[0][0] * mat[0][2] + Rows[0][1] * mat[1][2] + Rows[0][2] * mat[2][2] + Rows[0][3] * mat[3][2];
	res[0][3] = Rows[0][0] * mat[0][3] + Rows[0][1] * mat[1][3] + Rows[0][2] * mat[2][3] + Rows[0][3] * mat[3][3];
	
	res[1][0] = Rows[1][0] * mat[0][0] + Rows[1][1] * mat[1][0] + Rows[1][2] * mat[2][0] + Rows[1][3] * mat[3][0];
	res[1][1] = Rows[1][0] * mat[0][1] + Rows[1][1] * mat[1][1] + Rows[1][2] * mat[2][1] + Rows[1][3] * mat[3][1];
	res[1][2] = Rows[1][0] * mat[0][2] + Rows[1][1] * mat[1][2] + Rows[1][2] * mat[2][2] + Rows[1][3] * mat[3][2];
	res[1][3] = Rows[1][0] * mat[0][3] + Rows[1][1] * mat[1][3] + Rows[1][2] * mat[2][3] + Rows[1][3] * mat[3][3];
	
	res[2][0] = Rows[2][0] * mat[0][0] + Rows[2][1] * mat[1][0] + Rows[2][2] * mat[2][0] + Rows[2][3] * mat[3][0];
	res[2][1] = Rows[2][0] * mat[0][1] + Rows[2][1] * mat[1][1] + Rows[2][2] * mat[2][1] + Rows[2][3] * mat[3][1];
	res[2][2] = Rows[2][0] * mat[0][2] + Rows[2][1] * mat[1][2] + Rows[2][2] * mat[2][2] + Rows[2][3] * mat[3][2];
	res[2][3] = Rows[2][0] * mat[0][3] + Rows[2][1] * mat[1][3] + Rows[2][2] * mat[2][3] + Rows[2][3] * mat[3][3];
	
	res[3][0] = Rows[3][0] * mat[0][0] + Rows[3][1] * mat[1][0] + Rows[3][2] * mat[2][0] + Rows[3][3] * mat[3][0];
	res[3][1] = Rows[3][0] * mat[0][1] + Rows[3][1] * mat[1][1] + Rows[3][2] * mat[2][1] + Rows[3][3] * mat[3][1];
	res[3][2] = Rows[3][0] * mat[0][2] + Rows[3][1] * mat[1][2] + Rows[3][2] * mat[2][2] + Rows[3][3] * mat[3][2];
	res[3][3] = Rows[3][0] * mat[0][3] + Rows[3][1] * mat[1][3] + Rows[3][2] * mat[2][3] + Rows[3][3] * mat[3][3];

	return res;
}

float* anMatrix4::GetAddress()
{
	return &Rows[0][0];
}

const float* anMatrix4::GetAddress() const
{
	return &Rows[0][0];
}

anFloat4& anMatrix4::operator[](anUInt32 index)
{
	return Rows[index];
}

const anFloat4& anMatrix4::operator[](anUInt32 index) const
{
	return Rows[index];
}

anMatrix4& anMatrix4::operator=(const anMatrix4& rhs)
{
	Rows[0] = rhs.Rows[0];
	Rows[1] = rhs.Rows[1];
	Rows[2] = rhs.Rows[2];
	Rows[3] = rhs.Rows[3];
	return *this;
}

anMatrix4 anMatrix4::Translate(const anFloat3& pos)
{
	anMatrix4 res(1.0f);
	res[0][3] = pos.X;
	res[1][3] = pos.Y;
	res[2][3] = pos.Z;
	return res;
}

anMatrix4 anMatrix4::Scale(const anFloat3& size)
{
	anMatrix4 res(1.0f);
	res[0][0] = size.X;
	res[1][1] = size.Y;
	res[2][2] = size.Z;
	return res;
}

anMatrix4 anMatrix4::Rotate(float angle, const anFloat3& axis)
{
	anMatrix4 result(1.0f);

	float r = anDegreesToRadians(angle);
	float c = anCos(r);
	float s = anSin(r);
	float omc = 1.0f - c;

	float x = axis.X;
	float y = axis.Y;
	float z = axis.Z;

	result[0][0] = x * x * omc + c;
	result[0][1] = y * x * omc + z * s;
	result[0][2] = x * z * omc - y * s;

	result[1][0] = x * y * omc - z * s;
	result[1][1] = y * y * omc + c;
	result[1][2] = y * z * omc + x * s;

	result[2][0] = x * z * omc + y * s;
	result[2][1] = y * z * omc - x * s;
	result[2][2] = z * z * omc + c;

	return result;
}

anMatrix4 anMatrix4::Ortho(float left, float right, float bottom, float top, float near, float far)
{
	anMatrix4 res(1.0f);
	
	res[0][0] = 2.0f / (right - left);
	res[1][1] = 2.0f / (top - bottom);
	res[2][2] = 2.0f / (near - far);
	res[3][0] = (left + right) / (left - right);
	res[3][1] = (bottom + top) / (bottom - top);
	res[3][2] = (far + near) / (far - near);

	return res;
}

anMatrix4 anMatrix4::Inverse(const anMatrix4& m)
{
	const float coef00 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
	const float coef02 = m[1][2] * m[3][3] - m[3][2] * m[1][3];
	const float coef03 = m[1][2] * m[2][3] - m[2][2] * m[1][3];

	const float coef04 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
	const float coef06 = m[1][1] * m[3][3] - m[3][1] * m[1][3];
	const float coef07 = m[1][1] * m[2][3] - m[2][1] * m[1][3];

	const float coef08 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
	const float coef10 = m[1][1] * m[3][2] - m[3][1] * m[1][2];
	const float coef11 = m[1][1] * m[2][2] - m[2][1] * m[1][2];

	const float coef12 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
	const float coef14 = m[1][0] * m[3][3] - m[3][0] * m[1][3];
	const float coef15 = m[1][0] * m[2][3] - m[2][0] * m[1][3];

	const float coef16 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
	const float coef18 = m[1][0] * m[3][2] - m[3][0] * m[1][2];
	const float coef19 = m[1][0] * m[2][2] - m[2][0] * m[1][2];

	const float coef20 = m[2][0] * m[3][1] - m[3][0] * m[2][1];
	const float coef22 = m[1][0] * m[3][1] - m[3][0] * m[1][1];
	const float coef23 = m[1][0] * m[2][1] - m[2][0] * m[1][1];

	const anFloat4 fac0(coef00, coef00, coef02, coef03);
	const anFloat4 fac1(coef04, coef04, coef06, coef07);
	const anFloat4 fac2(coef08, coef08, coef10, coef11);
	const anFloat4 fac3(coef12, coef12, coef14, coef15);
	const anFloat4 fac4(coef16, coef16, coef18, coef19);
	const anFloat4 fac5(coef20, coef20, coef22, coef23);

	const anFloat4 vec0(m[1][0], m[0][0], m[0][0], m[0][0]);
	const anFloat4 vec1(m[1][1], m[0][1], m[0][1], m[0][1]);
	const anFloat4 vec2(m[1][2], m[0][2], m[0][2], m[0][2]);
	const anFloat4 vec3(m[1][3], m[0][3], m[0][3], m[0][3]);

	const anFloat4 inv0(vec1 * fac0 - vec2 * fac1 + vec3 * fac2);
	const anFloat4 inv1(vec0 * fac0 - vec2 * fac3 + vec3 * fac4);
	const anFloat4 inv2(vec0 * fac1 - vec1 * fac3 + vec3 * fac5);
	const anFloat4 inv3(vec0 * fac2 - vec1 * fac4 + vec2 * fac5);

	const anFloat4 signA(+1, -1, +1, -1);
	const anFloat4 signB(-1, +1, -1, +1);
	const anMatrix4 inverse(inv0 * signA, inv1 * signB, inv2 * signA, inv3 * signB);

	const anFloat4 row0(inverse[0][0], inverse[1][0], inverse[2][0], inverse[3][0]);

	const anFloat4 dot0(m[0] * row0);
	const float dot1 = (dot0[0] + dot0[1]) + (dot0[2] + dot0[3]);

	const float d = 1 / dot1;

	anMatrix4 res;
	res[0][0] = inverse[0][0] * d;
	res[0][1] = inverse[0][1] * d;
	res[0][2] = inverse[0][2] * d;
	res[0][3] = inverse[0][3] * d;
	res[1][0] = inverse[1][0] * d;
	res[1][1] = inverse[1][1] * d;
	res[1][2] = inverse[1][2] * d;
	res[1][3] = inverse[1][3] * d;
	res[2][0] = inverse[2][0] * d;
	res[2][1] = inverse[2][1] * d;
	res[2][2] = inverse[2][2] * d;
	res[2][3] = inverse[2][3] * d;
	res[3][0] = inverse[3][0] * d;
	res[3][1] = inverse[3][1] * d;
	res[3][2] = inverse[3][2] * d;
	res[3][3] = inverse[3][3] * d;
	return res;
}

anMatrix4 anMatrix4::LookAt(const anFloat3& camera, const anFloat3& object, const anFloat3& up)
{
	anMatrix4 result(1.0f);
	anFloat3 f = (object - camera).Normalize();
	anFloat3 s = f.Cross(up.Normalize());
	anFloat3 u = s.Cross(f);

	result[0][0] = s.X;
	result[1][0] = s.Y;
	result[2][0] = s.Z;
	
	result[0][1] = u.X;
	result[1][1] = u.Y;
	result[2][1] = u.Z;
	
	result[0][2] = -f.X;
	result[1][2] = -f.Y;
	result[2][2] = -f.Z;

	return result * Translate(anFloat3(-camera.X, -camera.Y, -camera.Z));
}

anFloat4 operator*(const anFloat4& f, const anMatrix4& m)
{
	anFloat4 res;
	res.X = f.X * m[0][0] + f.Y * m[0][1] + f.Z * m[0][2] + f.W * m[0][3];
	res.Y = f.X * m[1][0] + f.Y * m[1][1] + f.Z * m[1][2] + f.W * m[1][3];
	res.Z = f.X * m[2][0] + f.Y * m[2][1] + f.Z * m[2][2] + f.W * m[2][3];
	res.W = f.X * m[3][0] + f.Y * m[3][1] + f.Z * m[3][2] + f.W * m[3][3];
	return res;
}

anFloat3 operator*(const anFloat3& f, const anMatrix4& m)
{
	anFloat3 res;
	res.X = f.X * m[0][0] + f.Y * m[0][1] + f.Z * m[0][2] + m[0][3];
	res.Y = f.X * m[1][0] + f.Y * m[1][1] + f.Z * m[1][2] + m[1][3];
	res.Z = f.X * m[2][0] + f.Y * m[2][1] + f.Z * m[2][2] + m[2][3];
	return res;
}

anFloat2 operator*(const anFloat2& f, const anMatrix4& m)
{
	anFloat2 res;
	res.X = f.X * m[0][0] + f.Y * m[0][1] + m[0][2] + m[0][3];
	res.Y = f.X * m[1][0] + f.Y * m[1][1] + m[1][2] + m[1][3];
	return res;
}
