#pragma once

#ifndef ACTOR_H
#define ACTOR_H
#define GLM_ENABLE_EXPERIMENTAL

#include <string>
#include "ThirdParty/glm/glm.hpp"
#include "ThirdParty/SDL2_image/SDL_image.h"
#include "ThirdParty/SDL2/SDL.h"
#include <optional>
#include <unordered_set>
#include <unordered_map>
#include "ThirdParty/SDL2_mixer/SDL_mixer.h"
#include "lua.hpp"
#include "ThirdParty/LuaBridge/LuaBridge.h"
#include "ComponentDB.h"
#include <queue>
#include <utility>
#include "include/box2d/box2d.h"

class SceneDB;

class gameEngine;

struct typeStruct
{
	std::string type; 
	std::string key;
	std::shared_ptr<luabridge::LuaRef> component;
	// Used for getComponent to organize the types
};
struct keyStruct
{
	std::string type;
	std::string key;
	std::shared_ptr<luabridge::LuaRef> component;
	int uniqueIdentifier;
	// Used for getComponent to organize the types
};
struct componentsToAdd
{
	std::string type;
	std::string key;
	std::shared_ptr<luabridge::LuaRef> component;
};

struct collisionComponent
{
	Actor* other;
	b2Vec2 point;
	b2Vec2 relativeVelocity;
	b2Vec2 normal;
};

struct collisionData
{
	collisionComponent componentData;
	std::shared_ptr<luabridge::LuaRef> component;
	std::shared_ptr<luabridge::LuaRef> function;

};



class typeCompartor
{
public:
	bool operator()(const typeStruct& lhs, const typeStruct& rhs) {
		return lhs.type > rhs.type; // Compare based on the first element of the pair (priority)
	};
};

class keyCompartor
{
public:
	bool operator()(const keyStruct& lhs, const keyStruct& rhs) {
		if (lhs.uniqueIdentifier != rhs.uniqueIdentifier)
		{
			return lhs.uniqueIdentifier > rhs.uniqueIdentifier;
		}
		return lhs.key > rhs.key; // Compare based on the first element of the pair (priority)
	};
};

class Actor
{
public:

	Actor() = default;

	static inline int UUID = 0;
	static gameEngine* pointerToGameEngine;

	int uniqueIdentifier = -1;
	std::string name = "";

	bool destroyed = false;

	bool isPhantom = false;
	// Each actor will have components that are garunteed a key, and a type
	// Key then file loaded
	std::unordered_map<std::string, std::pair<std::string, std::shared_ptr<luabridge::LuaRef>>> ActorComponents;
	std::unordered_map<std::string, std::priority_queue<typeStruct, std::vector<typeStruct>, typeCompartor>> ComponentsByType;
	std::unordered_map<std::string, std::priority_queue<keyStruct, std::vector<keyStruct>, keyCompartor>> ComponentsByKey;

	std::vector<componentsToAdd> componentsToAddToActor;

	lua_State* currentLuaState;
	
	std::string GetName();
	int GetID();
	void InjectConvienienceReference(std::shared_ptr<luabridge::LuaRef> component_ref);
	luabridge::LuaRef GetComponentByKey(std::string keyToSearchFor);
	luabridge::LuaRef GetComponent(std::string typeNameToSearchFor);
	luabridge::LuaRef GetComponents(std::string typeNametoSearchFor);
	luabridge::LuaRef AddComponent(std::string componentType);
	void RemoveComponent(luabridge::LuaRef component);


	// Homework8 stuff
	// need a deque to stor on collision events. ComponentsByKey is already done for us
	std::deque<collisionData> OnCollisionEventQueue; // For OnCollisionEnter
	std::deque<collisionData> OnCollisionExitEventQueue;
};


#endif
