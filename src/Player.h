#pragma once

#include "Entity.h"
#include "Animation.h"
#include "Timer.h"
#include "Spear.h"
#include <box2d/box2d.h>
#include <SDL3/SDL.h>

constexpr auto PI = 3.14159265;
struct SDL_Texture;

class Player : public Entity
{
public:

	Player();
	
	virtual ~Player();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	// L08 TODO 6: Define OnCollision function for the player. 
	void OnCollision(PhysBody* physA, PhysBody* physB);
	void OnCollisionEnd(PhysBody* physA, PhysBody* physB);

private:
	void CheckTimers();
	void CheckGround();
	void GetPhysicsValues();
	void Move();
	void Jump();
	void Throw();
	void Dash();
	void ApplyPhysics();
	void Draw(float dt);

public:

	//Declare player parameters
	float speed = 4.0f;
	SDL_Texture* texture = NULL;

	int texW, texH;

	//Audio fx
	int pickCoinFxId;

	// L08 TODO 5: Add physics to the player - declare a Physics body
	PhysBody* pbody;
	float jumpForce = 1.5f; // The force to apply when jumping
	bool isJumping = false; // Flag to check if the player is currently jumping
	float dashForce = 3.0f; // The force to apply when dashing
	bool isDashing = false; // Flag to check if the player is currently dashing
	Timer dashTimer; // Dash duration until movement stops
	float dashMS = 1000.0f;
	bool canThrow = true; // Flag to see if the player can throw the object for dashing
	Timer throwTimer; // Throw duration until object is destroyed
	float throwMS = 1000.0f;
	bool spearCol = false;
	

private:
	std::shared_ptr<Spear> spear;
	b2Vec2 velocity;
	AnimationSet anims;

};