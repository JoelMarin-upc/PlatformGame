#include "Engine.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Physics.h"
#include "Scene.h"
#include "Log.h"
#include "Entity.h"
#include "EntityManager.h"
#include "Player.h"
#include "Map.h"
#include "Item.h"

Scene::Scene() : Module()
{
	name = "scene";
}

// Destructor
Scene::~Scene()
{}

// Called before render is available
bool Scene::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	currentScene = 0;
	LoadAllScenes();
	LoadScene();

	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	helpTexture = Engine::GetInstance().textures->Load(configParameters.child("helpText").attribute("path").as_string());
	LoadMap();
	
	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_M) == KEY_DOWN) NextScene();
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_H) == KEY_DOWN) showHelp = !showHelp;
	ShowHelp();
	//L03 TODO 3: Make the camera movement independent of framerate
	/*float camSpeed = 1;

	if(Engine::GetInstance().input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		Engine::GetInstance().render->camera.y -= (int)ceil(camSpeed * dt);

	if(Engine::GetInstance().input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		Engine::GetInstance().render->camera.y += (int)ceil(camSpeed * dt);

	if(Engine::GetInstance().input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		Engine::GetInstance().render->camera.x -= (int)ceil(camSpeed * dt);
	
	if(Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		Engine::GetInstance().render.get()->camera.x += (int)ceil(camSpeed * dt);*/

	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	if(Engine::GetInstance().input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");

	return true;
}

void Scene::LoadScene()
{
	
	//L04: TODO 3b: Instantiate the player using the entity manager
	//if (player && player->pbody) Engine::GetInstance().physics->DestroyBody(player->pbody);
	Engine::GetInstance().physics->CreateWorld();
	Engine::GetInstance().entityManager->entities.clear();
	player = nullptr;
	
	auto playerEntity = Engine::GetInstance().entityManager->CreateEntity(EntityType::PLAYER);
	player = std::dynamic_pointer_cast<Player>(playerEntity);
	player->speed = configParameters.child("player").attribute("speed").as_float();
	player->texturePath = configParameters.child("player").attribute("texture").as_string();
	player->animationsPath = configParameters.child("player").attribute("animationSet").as_string();
	Engine::GetInstance().render->follow = playerEntity;
	//L08: TODO 4: Create a new item using the entity manager and set the position to (200, 672) to test
	//std::shared_ptr<Item> item = std::dynamic_pointer_cast<Item>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM));
	//item->position = Vector2D(200, 672);
	
}

void Scene::LoadMap()
{
	auto currentScene = GetCurrentScene();
	Engine::GetInstance().audio->PlayMusic(configParameters.child("music").attribute("path").as_string());
	//L06 TODO 3: Call the function to load the map. 
	Engine::GetInstance().map->CleanUp();
	Engine::GetInstance().map->Load(currentScene.mapPath, currentScene.mapName);
	player->Start();
}

void Scene::LoadAllScenes()
{
	for (pugi::xml_node mapNode = configParameters.child("maps").child("map"); mapNode != NULL; mapNode = mapNode.next_sibling("map")) {
		SceneData scene = SceneData();
		scene.mapPath = mapNode.attribute("path").as_string();
		scene.mapName = mapNode.attribute("name").as_string();
		scenes.push_back(scene);
	}
}

SceneData Scene::GetCurrentScene()
{
	return scenes[currentScene];
}

void Scene::NextScene()
{
	currentScene++;
	if (currentScene >= scenes.size()) currentScene = 0;
	LoadScene();
	LoadMap();
}

void Scene::ShowHelp()
{
	if (!showHelp) return;
	Engine::GetInstance().render->DrawTexture(helpTexture, -Engine::GetInstance().render->camera.x, -Engine::GetInstance().render->camera.y);
}
