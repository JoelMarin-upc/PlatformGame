#pragma once

#include "Module.h"
#include "Player.h"

struct SDL_Texture;

struct SceneData {
	std::string mapPath;
	std::string mapName;
};

class Scene : public Module
{
public:

	Scene();

	// Destructor
	virtual ~Scene();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	void LoadScene();
	void LoadMap();
	void LoadAllScenes();
	SceneData GetCurrentScene();
	void NextScene();
	void ShowHelp();

private:

	//L03: TODO 3b: Declare a Player attribute
	std::shared_ptr<Player> player;
	std::vector<SceneData> scenes;
	int currentScene;
	bool showHelp = false;
	SDL_Texture* helpTexture;
};