#pragma once


#ifndef SCENEDB_H
#define SCENEDB_H
#define GLM_ENABLE_EXPERIMENTAL

#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include "Actor.h"
#include "ThirdParty/glm/glm.hpp"
#include "ThirdParty/glm/gtx/hash.hpp"
#include "lua.hpp"
#include "ThirdParty/LuaBridge/LuaBridge.h"

using namespace std;

class Actor;

class SceneDB
{
public:
	std::string sceneName;

	lua_State* currentLuaState;

	std::vector<Actor*> ActorsInScene;
	std::unordered_map<glm::vec2, std::map<int, Actor*>> sceneMap2;		// sceneMap2  is a map of positions of actors
	std::vector<vector<std::string>> sceneMap;				// sceneMap is a 2D vector map of the whole scene 


};

#endif

