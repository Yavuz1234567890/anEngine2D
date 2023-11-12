#include "anShaders.h"

static struct
{
	bool Initialized = false;

	anShader* ColorShader;
	anShader* BatchShader;
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
static const char* sBatchVertexShaderSource = R"(
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

static const char* sBatchFragmentShaderSource = R"(
#version 330 core

uniform sampler2D uSamplers[32];

in vec2 oTexCoord;
in vec4 oColor;
flat in int oTexIndex;

void main()
{
	vec4 sampled = oColor;

	switch(oTexIndex)
	{												
		case  0: sampled *= texture(uSamplers[ 0], oTexCoord); break;
		case  1: sampled *= texture(uSamplers[ 1], oTexCoord); break;
		case  2: sampled *= texture(uSamplers[ 2], oTexCoord); break;
		case  3: sampled *= texture(uSamplers[ 3], oTexCoord); break;
		case  4: sampled *= texture(uSamplers[ 4], oTexCoord); break;
		case  5: sampled *= texture(uSamplers[ 5], oTexCoord); break;
		case  6: sampled *= texture(uSamplers[ 6], oTexCoord); break;
		case  7: sampled *= texture(uSamplers[ 7], oTexCoord); break;
		case  8: sampled *= texture(uSamplers[ 8], oTexCoord); break;
		case  9: sampled *= texture(uSamplers[ 9], oTexCoord); break;
		case 10: sampled *= texture(uSamplers[10], oTexCoord); break;
		case 11: sampled *= texture(uSamplers[11], oTexCoord); break;
		case 12: sampled *= texture(uSamplers[12], oTexCoord); break;
		case 13: sampled *= texture(uSamplers[13], oTexCoord); break;
		case 14: sampled *= texture(uSamplers[14], oTexCoord); break;
		case 15: sampled *= texture(uSamplers[15], oTexCoord); break;
		case 16: sampled *= texture(uSamplers[16], oTexCoord); break;
		case 17: sampled *= texture(uSamplers[17], oTexCoord); break;
		case 18: sampled *= texture(uSamplers[18], oTexCoord); break;
		case 19: sampled *= texture(uSamplers[19], oTexCoord); break;
		case 20: sampled *= texture(uSamplers[20], oTexCoord); break;
		case 21: sampled *= texture(uSamplers[21], oTexCoord); break;
		case 22: sampled *= texture(uSamplers[22], oTexCoord); break;
		case 23: sampled *= texture(uSamplers[23], oTexCoord); break;
		case 24: sampled *= texture(uSamplers[24], oTexCoord); break;
		case 25: sampled *= texture(uSamplers[25], oTexCoord); break;
		case 26: sampled *= texture(uSamplers[26], oTexCoord); break;
		case 27: sampled *= texture(uSamplers[27], oTexCoord); break;
		case 28: sampled *= texture(uSamplers[28], oTexCoord); break;
		case 29: sampled *= texture(uSamplers[29], oTexCoord); break;
		case 30: sampled *= texture(uSamplers[30], oTexCoord); break;
		case 31: sampled *= texture(uSamplers[31], oTexCoord); break;
	}

	if (sampled.a == 0.0)
		discard;

	gl_FragColor = sampled;
}
)";

void anInitializeShaders()
{
	if (sShaders.Initialized)
		return;

	sShaders.ColorShader = new anShader(sColorVertexShaderSource, sColorFragmentShaderSource);
	sShaders.BatchShader = new anShader(sBatchVertexShaderSource, sBatchFragmentShaderSource);
	sShaders.Initialized = true;
}

anShader* anGetColorShader()
{
	return sShaders.ColorShader;
}

anShader* anGetBatchShader()
{
	return sShaders.BatchShader;
}
