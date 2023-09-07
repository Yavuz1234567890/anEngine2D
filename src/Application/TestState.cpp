#include "TestState.h"

TestState::TestState(anApplication* app)
	: anState(app)
{
}

TestState::~TestState()
{
}

void TestState::Initialize()
{
}

void TestState::Update(float dt)
{
}

void TestState::OnEvent(const anEvent& event)
{
}

void TestState::Render(anRenderer& renderer)
{
	renderer.DrawLine({ 0.0f, 0.0f }, { 100.0f, 100.0f }, { 255, 255, 255 });
}
