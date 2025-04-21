#pragma once
#ifndef AUDIODB_H
#define AUDIODB_H

#include <string>
#include <vector>
#include "ThirdParty/SDL2_image/SDL_image.h"
#include "ThirdParty/SDL2/SDL.h"
#include "ThirdParty/SDL2_mixer/SDL_mixer.h"
#include <queue>
#include "lua.hpp"
#include "ThirdParty/LuaBridge/LuaBridge.h"
#include <utility>

class gameEngine;

class AudioDB
{
public:
	static AudioDB* pointerToSelf;

	AudioDB() {
		pointerToSelf = this;
	}

	std::unordered_map<std::string, Mix_Chunk*> loadedMusicFiles;

	std::vector< Mix_Chunk*> loadedMusic;
	void LoadMusic(gameEngine* Game, std::string loadSong); // Possibly delete
	Mix_Chunk* LoadSFX(std::string name);

	static void Play(int channel, std::string clipName, bool doesLoop);
	static void Halt(int channel);
	static void SetVolume(int channel, int volume);
};

#endif
