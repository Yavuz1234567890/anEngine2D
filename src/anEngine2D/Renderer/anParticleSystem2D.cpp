#include "anParticleSystem2D.h"
#include "Math/anMath.h"

anParticleSystem2D::anParticleSystem2D()
{
	mParticles.resize(1000);
}

anParticleSystem2D::~anParticleSystem2D()
{
}

void anParticleSystem2D::Update(float dt)
{
	for (auto& particle : mParticles)
	{
		if (!particle.Active)
			continue;

		if (particle.LifeRemaining <= 0.0f)
		{
			particle.Active = false;
			continue;
		}

		particle.LifeRemaining -= dt;
		particle.Position += particle.Velocity * dt;
		particle.Rotation += 0.01f * dt;
	}
}

void anParticleSystem2D::Render2D(anRenderer2D& renderer)
{
	for (auto& particle : mParticles)
	{
		if (!particle.Active)
			continue;

		float life = particle.LifeRemaining / particle.LifeTime;
		anColor color;
		color.R = particle.ColorEnd.R + int((particle.ColorBegin.R - particle.ColorEnd.R) * life);
		color.G = particle.ColorEnd.G + int((particle.ColorBegin.G - particle.ColorEnd.G) * life);
		color.B = particle.ColorEnd.B + int((particle.ColorBegin.B - particle.ColorEnd.B) * life);
		color.A = particle.ColorEnd.A + int((particle.ColorBegin.A - particle.ColorEnd.A) * life);
		
		float size = particle.SizeEnd + (particle.SizeBegin - particle.SizeEnd) * life;

		renderer.DrawTexture(anTexture::GetWhiteTexture(), particle.Position, { size, size }, particle.Rotation, color);

	}
}

void anParticleSystem2D::Add(const anParticle2DProps& props)
{
	anParticle2D& particle = mParticles[mParticleIndex];
	particle.Active = true;
	particle.Position = props.Position;
	particle.Rotation = anRandf(360.0f);

	particle.Velocity.x = props.Velocity.x * anRandf(props.VelocityXInterval.x, props.VelocityXInterval.y);
	particle.Velocity.y = props.Velocity.y * anRandf(props.VelocityYInterval.x, props.VelocityYInterval.y);

	particle.ColorBegin = props.ColorBegin;
	particle.ColorEnd = props.ColorEnd;

	particle.LifeTime = props.LifeTime;
	particle.LifeRemaining = props.LifeTime;
	particle.SizeBegin = props.SizeBegin;
	particle.SizeEnd = props.SizeEnd;

	mParticleIndex = --mParticleIndex % mParticles.size();
}
