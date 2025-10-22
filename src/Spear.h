#pragma once

#include "Entity.h"
#include "Animation.h"
#include "Timer.h"
#include <box2d/box2d.h>
#include <SDL3/SDL.h>
#include <cmath>

struct SDL_Texture;

class Spear : public Entity
{
public:

	Spear();

	virtual ~Spear();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	bool Destroy();

	

private:
	
	void Move();
	void GetPhysicsValues();
	void ApplyPhysics();
	void Draw(float dt);
	void OnCollision(PhysBody* physA, PhysBody* physB);


	SDL_Texture* texture;
	int texW=16, texH=16;

public:
	void Initialize(float angle);

	float initialAngle;
	float spearGravity = 0.4f;

private:
	float velocityMagnitude = 12.0f;
	b2Vec2 velocity;

	PhysBody* pbody;
	AnimationSet anims;
};