#define GLM_ENABLE_EXPERIMENTAL

#include <iostream>
#include "gameEngine.h"
#include "Renderer.h"
#include "Input.h"
#include "EngineUtils.h"
#include "ThirdParty/glm/gtx/hash.hpp"
#include "ThirdParty/SDL2/SDL.h"
#include "Helper.h"
#include "AudioHelper.h"
#include "ThirdParty/SDL2_mixer/SDL_mixer.h"
#include "ThirdParty/SDL2_ttf/SDL_ttf.h"

#include "lua.hpp"
#include "ThirdParty/LuaBridge/LuaBridge.h"

#include "include/box2d/box2d.h"
#include "Rigidbody.h"

#include "Editor.h"


using namespace std;

enum currentMode {
	Intro,    // 0
	Gameplay,  // 1
	Ending  // 3
};

gameEngine* gameEngine::pointerToSelf = nullptr;
Input* Input::pointerToSelf = nullptr;
float Input::mouseWheelDelta = 0.0f;
gameEngine* Actor::pointerToGameEngine = nullptr;
ImageDB* ImageDB::pointerToSelf = nullptr;
AudioDB* AudioDB::pointerToSelf = nullptr;
gameEngine* ImageDB::pointerToEngine = nullptr;

const float TIME_STEP = 1.0f / 60.0f;
const int VELOCITY_ITERATIONS = 8;
const int POSITION_ITERATIONS = 3;


int main(int argc, char* argv[])
{
	// Before all of my game logic, I want to open and use the editor:
	Editor currentEditor;
	currentEditor.EditorBegin();





	gameEngine Game;
	EngineUtils engine;
	Input inputHandler;
	Game.ImageHandler.pointerToEngine = &Game;



	inputHandler.Init();

	// Declare information for the window creation
	SDL_Window* window = Helper::SDL_CreateWindow(Game.gameTitle.c_str(), 100, 100, static_cast<int>(Game.WindowXResolution), static_cast<int>(Game.WindowYResolution), SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = Helper::SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
	SDL_SetRenderDrawColor(renderer, Game.ClearColorRed, Game.ClearColorGreen, Game.ClearColorBlue, 255);
	SDL_RenderClear(renderer);
	Game.ImageHandler.renderer = renderer;

	std::string pathToResource = "resources/game.config";
	Game.getDocument(&Game, pathToResource);			// Should have game.config now 
	engine.parseGameConfig(&Game);
	// Read in the initial Scene :)
	Game.Components.Initialize();
	engine.ReadInitialScene(Game.currentDoc, &Game);

	pathToResource = "resources/rendering.config";
	Game.getDocument(&Game, pathToResource);
	engine.parseRenderingConfig(&Game);



	AudioHelper::Mix_AllocateChannels(50);


	// Load the images into an array that can be used later

	// Load in the font
	TTF_Init();

	// Parse information about the rendere

	// Set and clear one of the display buffers


	// Load in the audio to where it needs to be 
	AudioHelper::Mix_OpenAudio(44100, AUDIO_S16LSB, 0, 2048);
	Game.getScene(&Game);			// Loads in the initial scene and the actors


	// Now we need to add need to parse it
	pathToResource = "resources/rendering.config";
	Game.getDocument(&Game, pathToResource);
	engine.parseCameraOffset(&Game);

	// Now we need to get ending images
	pathToResource = "resources/game.config";
	Game.getDocument(&Game, pathToResource);			// Should have renderConfig now 


	SDL_Event nextEvent;
	/*Game.ImageHandler.CameraPosition.x = -Game.WindowXResolution * 0.5f;
	Game.ImageHandler.CameraPosition.y = -Game.WindowYResolution * 0.5f;*/
	// Homework 7 stuff 
	Game.InjectActorReferences();
	for (auto i = 0; i < Game.SceneVector.at(Game.currentScene).ActorsInScene.size(); i++)
	{
		// loop through all of the actors
		for (const auto& j : Game.SceneVector.at(Game.currentScene).ActorsInScene[i]->ActorComponents)
		{
			Game.checkForFunctions(j.first, j.second, Game.SceneVector.at(Game.currentScene).ActorsInScene[i]->uniqueIdentifier, Game.SceneVector.at(Game.currentScene).ActorsInScene[i]->name);
		}
	}
	while (true)
	{
		Game.addComponentsToActors();
		if (Game.switchScene)
		{
			Game.SwitchScenes();
		}
		Game.InjectActorReferences();
		Game.addOnStart();
		Game.clearFunctionPQ();
		SDL_SetRenderDrawColor(renderer, Game.ClearColorRed, Game.ClearColorGreen, Game.ClearColorBlue, 255);
		SDL_RenderClear(renderer);
		// This will be the game update loop 
		while (Helper::SDL_PollEvent(&nextEvent))
		{
			inputHandler.ProcessEvent(nextEvent);
			if (nextEvent.type == SDL_QUIT)
			{
				Game.addOnUpdate();
				Game.clearFunctionPQ();
				Game.addOnLateUpdate();
				Game.clearFunctionPQ();
				Game.ImageHandler.RenderAndClearAllImages(Game.ImageHandler.DrawRequests);
				Game.ImageHandler.RenderAndClearAllUIImages(Game.ImageHandler.ImagesToDraw);
				Game.ImageHandler.ClearTextQueue();
				Game.ImageHandler.RenderAndClearPixels();
				Helper::SDL_RenderPresent(renderer);
				exit(0);
			}
			// If we press space, or click, or 
		}
		Game.addOnUpdate();
		Game.clearFunctionPQ();
		Game.addOnLateUpdate();
		Game.clearFunctionPQ();
		Game.clearOnDestryFunctionPQ();
		Game.ImageHandler.RenderAndClearAllImages(Game.ImageHandler.DrawRequests);
		Game.ImageHandler.RenderAndClearAllUIImages(Game.ImageHandler.ImagesToDraw);
		Game.ImageHandler.ClearTextQueue();
		Game.ImageHandler.RenderAndClearPixels();
		inputHandler.LateUpdate();
		Game.addActors();
		Game.DestroyActors();
		if (Game.Components.components.find("Rigidbody") != Game.Components.components.end())
		{
			// There is a rigidbody loaded 
			Game.physics_world->Step(TIME_STEP, VELOCITY_ITERATIONS, POSITION_ITERATIONS);
		}
		Helper::SDL_RenderPresent(renderer);
	}
	return 0;
}
