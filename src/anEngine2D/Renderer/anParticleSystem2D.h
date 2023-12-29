#ifndef AN_PARTICLE_SYSTEM_2D_H_
#define AN_PARTICLE_SYSTEM_2D_H_

#include "Math/anFloat2.h" 
#include "Renderer/anRenderer2D.h"

struct anParticle2D
{
	anFloat2 Position;
	anFloat2 Velocity;
	anColor ColorBegin;
	anColor ColorEnd;
	float Rotation = 0.0f;
	float SizeBegin;
	float SizeEnd;
	float LifeTime = 1.0f;
	float LifeRemaining = 0.0f;
	bool Active = false;
};

struct anParticle2DProps
{
	anFloat2 Position;
	anFloat2 Velocity;
	anFloat2 VelocityXInterval;
	anFloat2 VelocityYInterval;
	anColor ColorBegin;
	anColor ColorEnd;
	float SizeBegin;
	float SizeEnd;
	float LifeTime = 1.0f;
};

class anParticleSystem2D
{
public:
	anParticleSystem2D();
	~anParticleSystem2D();

	void Update(float dt);
	void Render2D(anRenderer2D& renderer);

	void Add(const anParticle2DProps& props);
private:
	anVector<anParticle2D> mParticles;
	anUInt32 mParticleIndex = 999;
};

#endif
