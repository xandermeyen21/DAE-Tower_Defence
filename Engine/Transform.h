#pragma once
#include "base.h"
#include "Vector2f.h"

struct Transform
{
public:
	void ApplyTransformation() const
	{
		glPushMatrix();
		glTranslatef(Position.x, Position.y, 0);
		glRotatef(Rotation, 0, 0, 1);
		glScalef(Scale.x, Scale.y, 1);
	}

	void ResetTransformation() const
	{
		glPopMatrix();
	}

	Vector2f Position{};
	float Rotation{};
	Vector2f Scale{ 1.f, 1.f };
};
