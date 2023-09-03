#ifndef AN_SHADER_H_
#define AN_SHADER_H_

#include "Core/anTypes.h"
#include "Math/anMatrix4.h"

class anShader
{
public:
	anShader(const char* vertSrc, const char* fragSrc);
	~anShader();

	void Bind() const;
	void Unbind() const;
	void SetUniformFloat(const anString& name, float val);
	void SetUniformFloat2(const anString& name, const anFloat2& val);
	void SetUniformFloat3(const anString& name, const anFloat3& val);
	void SetUniformFloat4(const anString& name, const anFloat4& val);
	void SetUniformMatrix4(const anString& name, const anMatrix4& val);
	void SetUniformIntArray(const anString& name, anUInt32 size, int* data);
	void SetUniformInt(const anString& name, int val);
	anUInt32 GetID() const;
private:
	int GetUniformLocation(const anString& name);
private:
	anUInt32 mID;
	anUInt32 mVertexShader;
	anUInt32 mFragmentShader;
};

#endif
