#include "anShaders.h"

static struct
{
	bool Initialized = false;

	anShader* ColorShader;
	anShader* TextureShader;
	anShader* FontShader;
} sShaders;

// Color shader
static const char* sColorVertexShaderSource = R"(
#version 330 core
	
layout(location=0) in vec3 aPos;
layout(location=1) in vec4 aColor;

uniform mat4 uMatrix;

out vec4 oColor;

void main()
{
	gl_Position = uMatrix * vec4(aPos, 1.0);

	oColor = aColor;
}

)";

static const char* sColorFragmentShaderSource = R"(
#version 330 core

in vec4 oColor;

void main()
{
	gl_FragColor = oColor;
}
)";

// Texture shader
static const char* sTextureVertexShaderSource = R"(
#version 330 core

layout(location=0) in vec3 aPos;
layout(location=1) in vec2 aTexCoord;
layout(location=2) in vec4 aColor;
layout(location=3) in int aTexIndex;

uniform mat4 uMatrix;

out vec2 oTexCoord;
out vec4 oColor;
flat out int oTexIndex;

void main()
{
	gl_Position = uMatrix * vec4(aPos, 1.0f);

	oTexCoord = aTexCoord;
	oColor = aColor;
	oTexIndex = aTexIndex;
}
)";

static const char* sTextureFragmentShaderSource = R"(
#version 330 core

uniform sampler2D uSamplers[32];

in vec2 oTexCoord;
in vec4 oColor;
flat in int oTexIndex;

void main()
{
	gl_FragColor = texture(uSamplers[oTexIndex], oTexCoord) * oColor;
}
)";

void anInitializeShaders()
{
	if (sShaders.Initialized)
		return;

	sShaders.ColorShader = new anShader(sColorVertexShaderSource, sColorFragmentShaderSource);
	sShaders.TextureShader = new anShader(sTextureVertexShaderSource, sTextureFragmentShaderSource);
	sShaders.Initialized = true;
}

anShader* anGetColorShader()
{
	return sShaders.ColorShader;
}

anShader* anGetTextureShader()
{
	return sShaders.TextureShader;
}
