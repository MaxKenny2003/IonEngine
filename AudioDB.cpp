#include "AudioDB.h"
#include "ImageDB.h"
#include "Helper.h"
#include <filesystem>
#include "gameEngine.h"
#include "Helper.h"
#include "AudioHelper.h"



void AudioDB::LoadMusic(gameEngine* Game, std::string loadSong)
{
	// Need to load in the song
	Mix_Chunk* currentSong = nullptr;
	if (loadSong != "")
	{
		std::string path = "resources/audio/" + loadSong;
		currentSong = AudioHelper::Mix_LoadWAV((path + ".wav").c_str());
		if (!currentSong)
		{
			currentSong = AudioHelper::Mix_LoadWAV((path + ".ogg").c_str());
			if (!currentSong)
			{
				cout << "error: failed to play audio clip " << loadSong;
				exit(0);
			}
		}
	}
	loadedMusic.back() = currentSong;
}


Mix_Chunk* AudioDB::LoadSFX(std::string name)
{
	Mix_Chunk* temp = nullptr;
	if (name != "")
	{
		std::string path = "resources/audio/" + name;
		temp = AudioHelper::Mix_LoadWAV((path + ".wav").c_str());
		if (!temp)
		{
			temp = AudioHelper::Mix_LoadWAV((path + ".ogg").c_str());
			if (!temp)
			{
				cout << "error: failed to play audio clip " << name;
				exit(0);
			}
		}
	}
	return temp;
}

void AudioDB::Play(int channel, std::string clipName, bool doesLoop)
{
	if (pointerToSelf->loadedMusicFiles.find(clipName) == pointerToSelf->loadedMusicFiles.end())
	{
		Mix_Chunk* temp = pointerToSelf->LoadSFX(clipName);
		pointerToSelf->loadedMusicFiles.insert({ clipName, temp });
	}
	// we are garunteed to have the music file after this point
	
	if (doesLoop)
	{
		AudioHelper::Mix_PlayChannel(channel, pointerToSelf->loadedMusicFiles.at(clipName), -1);
	}
	else
	{
		AudioHelper::Mix_PlayChannel(channel, pointerToSelf->loadedMusicFiles.at(clipName), 0);
	}
}

void AudioDB::Halt(int channel)
{
	AudioHelper::Mix_HaltChannel(channel);
}

void AudioDB::SetVolume(int channel, int volume)
{
	AudioHelper::Mix_Volume(channel, volume);
}
