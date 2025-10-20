#include "Spear.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include "EntityManager.h"


Spear::Spear() : Entity(EntityType::SPEAR)
{
	name = "Spear";
}

Spear::~Spear() {

}

bool Spear::Awake() {
	return true;
}

bool Spear::Start() {
	return true;
}

bool Spear::Update(float dt) {
	if (!active) return true;
	//LOG("%f %f", velocity.x, velocity.y);
	Move();
	ApplyPhysics();
	GetPhysicsValues();
	Draw(dt);
	return true;
}

bool Spear::CleanUp()
{
	Engine::GetInstance().textures->UnLoad(texture);
	Engine::GetInstance().physics->DeletePhysBody(pbody);
	return true;
}

bool Spear::Destroy()
{
	LOG("Destroying item");
	active = false;
	Engine::GetInstance().entityManager->DestroyEntity(shared_from_this());
	return true;
}

void Spear::Move() {
	velocity.y += spearGravity;
	anims.SetCurrent("move");
}

void Spear::GetPhysicsValues() {
	//velocity = Engine::GetInstance().physics->GetLinearVelocity(pbody);
}

void Spear::ApplyPhysics() {
	Engine::GetInstance().physics->SetLinearVelocity(pbody, velocity);
}

void Spear::Draw(float dt) {
	anims.Update(dt);
	const SDL_Rect& animFrame = anims.GetCurrentFrame();
	int x, y;
	pbody->GetPosition(x, y);
	position.setX((float)x);
	position.setY((float)y);
	Engine::GetInstance().render->DrawTexture(texture, x - texW / 2, y - texH / 2, &animFrame);
	
}

void Spear::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		velocity = { 0.0f,0.0f };
		spearGravity = 0.0f;
		break;
	case ColliderType::UNKNOWN:
		break;
	default:
		break;
	}

}

void Spear::Initialize(float angle) {
	initialAngle = angle;
	texture = Engine::GetInstance().textures->Load("Assets/Textures/Water Effect and Bullet 16x16.png");
	std::unordered_map<int, std::string> aliases = { {262,"move"} };
	anims.LoadFromTSX("Assets/Textures/Water Effect and Bullet 16x16.tsx", aliases);
	pbody = Engine::GetInstance().physics->CreateCircle((int)position.getX() + texH / 2, (int)position.getY() + texH / 2, texH / 2, bodyType::DYNAMIC);
	pbody->ctype = ColliderType::SPEAR;
	pbody->listener = this;
	velocity = { velocityMagnitude * cos(initialAngle),-velocityMagnitude * sin(initialAngle) };
	LOG("x = %f y = %f angle = %f", velocityMagnitude * cos(initialAngle), velocityMagnitude * sin(initialAngle),initialAngle/ 3.14159265f*180.f);
}