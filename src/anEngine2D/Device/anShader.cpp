#include "anShader.h"
#include "Core/anMessage.h"

#include <GL/glew.h>

anShader::anShader(const char* vertSrc, const char* fragSrc)
{
	mVertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(mVertexShader, 1, &vertSrc, NULL);

	// compiles shader and checks is compiled
	{
		glCompileShader(mVertexShader);

		GLint is_compiled = 0;
		glGetShaderiv(mVertexShader, GL_COMPILE_STATUS, &is_compiled);
		if (is_compiled == GL_FALSE)
		{
			GLchar err_str[1024];
			glGetShaderInfoLog(mVertexShader, 1024, nullptr, err_str);
			anMessage("Vertex shader error: " + anString(err_str));
		}
	}

	mFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(mFragmentShader, 1, &fragSrc, NULL);

	// compiles shader and checks is compiled
	{
		glCompileShader(mFragmentShader);

		GLint is_compiled = 0;
		glGetShaderiv(mFragmentShader, GL_COMPILE_STATUS, &is_compiled);
		if (is_compiled == GL_FALSE)
		{
			GLchar err_str[1024];
			glGetShaderInfoLog(mFragmentShader, 1024, nullptr, err_str);
			anMessage("Fragment shader error: " + anString(err_str));
		}
	}

	mID = glCreateProgram();

	glAttachShader(mID, mVertexShader);
	glAttachShader(mID, mFragmentShader);

	// links program and checks is linked
	{
		glLinkProgram(mID);

		GLint is_linked = 0;
		glGetProgramiv(mID, GL_LINK_STATUS, &is_linked);
		if (is_linked == GL_FALSE)
		{
			GLchar err_str[1024];
			glGetProgramInfoLog(mID, 1024, nullptr, err_str);
			anMessage("Shader program linking error: " + anString(err_str));
		}
	}
}

anShader::~anShader()
{
	glDeleteProgram(mID);
	glDeleteShader(mVertexShader);
	glDeleteShader(mFragmentShader);
}

void anShader::Bind() const
{
	glUseProgram(mID);
}

void anShader::Unbind() const
{
	glUseProgram(0);
}

void anShader::SetUniformFloat(const anString& name, float val)
{
	glUniform1f(GetUniformLocation(name), val);
}

void anShader::SetUniformFloat2(const anString& name, const anFloat2& val)
{
	glUniform2f(GetUniformLocation(name), val.X, val.Y);
}

void anShader::SetUniformFloat3(const anString& name, const anFloat3& val)
{
	glUniform3f(GetUniformLocation(name), val.X, val.Y, val.Z);
}

void anShader::SetUniformFloat4(const anString& name, const anFloat4& val)
{
	glUniform4f(GetUniformLocation(name), val.X, val.Y, val.Z, val.W);
}

void anShader::SetUniformMatrix4(const anString& name, const anMatrix4& val)
{
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, val.GetAddress());
}

void anShader::SetUniformIntArray(const anString& name, anUInt32 size, int* data)
{
	glUniform1iv(GetUniformLocation(name), size, data);
}

void anShader::SetUniformInt(const anString& name, int val)
{
	glUniform1i(GetUniformLocation(name), val);
}

int anShader::GetUniformLocation(const anString& name)
{
	int res = glGetUniformLocation(mID, name.c_str());
	if (res < 0)
		anMessage("Unknown uniform variable: " + name);

	return res;
}

anUInt32 anShader::GetID() const
{
	return mID;
}
