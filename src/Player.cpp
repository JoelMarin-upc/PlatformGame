#include "Player.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Map.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include "EntityManager.h"

Player::Player() : Entity(EntityType::PLAYER)
{
	name = "Player";
}

Player::~Player() {

}

bool Player::Awake() {

	//L03: TODO 2: Initialize Player parameters
	
	return true;
}

bool Player::Start() {

	// load
	std::unordered_map<int, std::string> aliases = { {0,"idle"},{11,"move"},{22,"jump"} };
	anims.LoadFromTSX("Assets/Textures/PLayer2_Spritesheet.tsx", aliases);
	anims.SetCurrent("idle");

	//L03: TODO 2: Initialize Player parameters
	texture = Engine::GetInstance().textures->Load("Assets/Textures/player2_spritesheet.png");

	// L08 TODO 5: Add physics to the player - initialize physics body
	//Engine::GetInstance().textures->GetSize(texture, texW, texH);
	texW = 32;
	texH = 32;
	Respawn();

	//initialize audio effect
	pickCoinFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/coin-collision-sound-342335.wav");

	return true;
}

bool Player::Update(float dt)
{
	GodMode();
	CheckTimers();
	GetPhysicsValues();
	CheckGround();
	Move();
	Jump();
	Throw();
	Dash();
	ApplyPhysics();
	Draw(dt);

	return true;
}

void Player::GodMode()
{
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)
	{
		godMode = !godMode;
		if (godMode) b2Body_Disable(pbody->body);
		else b2Body_Enable(pbody->body);
	}
}

void Player::CheckTimers() {
	if (!canThrow && (throwTimer.ReadMSec() >= throwMS || spearCol)) {
		spear->Destroy();
		canThrow = true;
		spearCol = false;
	}
	if (isDashing && dashTimer.ReadMSec() >= dashMS) {
		isDashing = false;
	}
	

}

void Player::CheckGround()
{
	float verticalVel = std::abs(velocity.y);
	if (verticalVel < 0.3)
	{
		b2Vec2 feetPos{ position.getX(), position.getY() + 20 + texH / 2 };
		float _;
		int dist = pbody->RayCast(position.getX(), position.getY(), feetPos.x, feetPos.y, _, _);
		int distLeft = pbody->RayCast(position.getX() - texW / 2, position.getY(), feetPos.x - texW / 2, feetPos.y, _, _);
		int distRight = pbody->RayCast(position.getX() + texW / 2, position.getY(), feetPos.x + texW / 2, feetPos.y, _, _);
		if (dist != -1) {
			isJumping = false;
			//isDashing = false;
			anims.SetCurrent("idle");
		}
	}
	else {
		isJumping = true;
		//isDashing = true;
	}
}

void Player::GetPhysicsValues() {
	// Read current velocity
	velocity = Engine::GetInstance().physics->GetLinearVelocity(pbody);
	velocity = { 0, velocity.y };
	//if (!isJumping && !isDashing) velocity = { 0, velocity.y }; // Reset horizontal velocity by default, this way the player stops when no key is pressed
}

void Player::Move() {
	//if (isJumping || isDashing) return;
	// Move left/right
	if (godMode) {
		b2Transform t = Engine::GetInstance().physics->GetTransform(pbody);
		if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
			Engine::GetInstance().physics->MoveBody(pbody, b2Vec2{ t.p.x - godModeSpeed, t.p.y }, t.q);
			anims.SetCurrent("move");
		}
		if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
			Engine::GetInstance().physics->MoveBody(pbody, b2Vec2{ t.p.x + godModeSpeed, t.p.y }, t.q);
			anims.SetCurrent("move");
		}
		if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
			Engine::GetInstance().physics->MoveBody(pbody, b2Vec2{ t.p.x, t.p.y - godModeSpeed }, t.q);
			anims.SetCurrent("move");
		}
		if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
			Engine::GetInstance().physics->MoveBody(pbody, b2Vec2{ t.p.x, t.p.y + godModeSpeed }, t.q);
			anims.SetCurrent("move");
		}
	}
	else {
		if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
			velocity.x = -speed;
			anims.SetCurrent("move");
		}
		if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
			velocity.x = speed;
			anims.SetCurrent("move");
		}
	}
}

void Player::Jump() {
	// This function can be used for more complex jump logic if needed
	if (godMode) return;
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && isJumping == false) {
		Engine::GetInstance().physics->ApplyLinearImpulseToCenter(pbody, 0.0f, -jumpForce, true);
		anims.SetCurrent("jump");
		isJumping = true;
	}
}

void Player::Throw() {
	if (canThrow == true && Engine::GetInstance().input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_DOWN) {
		canThrow = false;
		throwTimer = Timer();
		spear = std::dynamic_pointer_cast<Spear>(Engine::GetInstance().entityManager->CreateEntity(EntityType::SPEAR));
		float angle = 0;
		Vector2D initialPos = Vector2D(50, 0);
		if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
			if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
				angle = PI/4;
				initialPos = Vector2D(0,-50);
			}
			else if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
				angle = 7 * PI / 4;
				initialPos = Vector2D(50,0);
			}
			else {
				angle = 0;
				initialPos = Vector2D(50,0);
			}
		}
		else if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
			if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
				angle = 3*PI/4;
				initialPos = Vector2D(0,-50);
			}
			else if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
				angle = 5*PI/4;
				initialPos = Vector2D(0,50);
			}
			else {
				angle = PI;
				initialPos = Vector2D(-50,0);
			}
		}
		else if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
			angle = PI/2;
			initialPos = Vector2D(0, -50);
		}
		else if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
			angle = 3*PI/2;
			initialPos = Vector2D(0, 50);
		}
		spear->position = position + initialPos + Vector2D{(float) - texW / 2, (float)-texH / 2};
		spear->Initialize(angle);
		LOG("angle");
	}
}

void Player::Dash() {
	if (isDashing == false && canThrow == false && Engine::GetInstance().input->GetKey(SDL_SCANCODE_LCTRL) == KEY_DOWN) {
		float x = spear->position.getX() - position.getX();
		float y = spear->position.getY() - position.getY();
		float aux = sqrt(pow(x, 2) + pow(y, 2));
		x = x / aux;
		y = y / aux;
		Engine::GetInstance().physics->SetLinearVelocity(pbody,0, 0);
;		Engine::GetInstance().physics->ApplyLinearImpulseToCenter(pbody, x * dashForce, y * dashForce, true);
		anims.SetCurrent("dash");
		isDashing = true;
		dashTimer = Timer();
	}
}

void Player::ApplyPhysics() {
	// Preserve vertical speed while jumping
	if (isJumping == true || isDashing == true) {
		velocity.y = Engine::GetInstance().physics->GetYVelocity(pbody);
	}

	if (isDashing == true) {
		velocity.x = Engine::GetInstance().physics->GetXVelocity(pbody);
	}

	// Apply velocity via helper
	Engine::GetInstance().physics->SetLinearVelocity(pbody, velocity);
}

void Player::Respawn() {
	Vector2D* startPos = Engine::GetInstance().map->playerStartPos;
	if (startPos) position = *startPos;
	else position = Vector2D(90, 90);
	Engine::GetInstance().physics->DestroyBody(pbody);
	pbody = nullptr;
	pbody = Engine::GetInstance().physics->CreateCircle((int)position.getX(), (int)position.getY(), texW / 2, bodyType::DYNAMIC);	
	pbody->listener = this;
	pbody->ctype = ColliderType::PLAYER;
}

void Player::Draw(float dt) {

	anims.Update(dt);
	const SDL_Rect& animFrame = anims.GetCurrentFrame();

	// Update render position using your PhysBody helper
	int x, y;
	pbody->GetPosition(x, y);
	position.setX((float)x);
	position.setY((float)y);
	Engine::GetInstance().render->DrawTexture(texture, x - texW / 2, y - texH / 2, &animFrame);
}

bool Player::CleanUp()
{
	LOG("Cleanup player");
	Engine::GetInstance().textures->UnLoad(texture);
	return true;
}

// L08 TODO 6: Define OnCollision function for the player. 
void Player::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
		//reset the jump flag when touching the ground
		/*isJumping = false;
		anims.SetCurrent("idle");*/
		break;
	case ColliderType::ITEM:
		LOG("Collision ITEM");
		Engine::GetInstance().audio->PlayFx(pickCoinFxId);
		physB->listener->Destroy();
		break;
	case ColliderType::SPEAR:
		spearCol = true;
		break;
	case ColliderType::DEATHZONE:
		Respawn();
		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	default:
		break;
	}
}

void Player::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		LOG("End Collision PLATFORM");
		break;
	case ColliderType::ITEM:
		LOG("End Collision ITEM");
		break;
	case ColliderType::UNKNOWN:
		LOG("End Collision UNKNOWN");
		break;
	default:
		break;
	}
}

