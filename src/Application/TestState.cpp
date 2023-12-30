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

void TestState::Render2D(anRenderer2D& renderer)
{
}

void TestState::OnImGuiRender()
{
}
