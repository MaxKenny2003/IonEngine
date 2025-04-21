#pragma once

#ifndef COMPONENTDB_H
#define COMPONENTDB_H
#define GLM_ENABLE_EXPERIMENTAL

#include <map>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <memory>
#include "lua.hpp"
#include "ThirdParty/LuaBridge/LuaBridge.h"
#include <thread>
#include <cstdlib>

class gameEngine;
class Input;
class Actor;

class ComponentDB
{
public:
	// need to have a map of components
	lua_State* currentLuaState;
	
	std::string key;
	// These are defined components that are referenced
	std::map<std::string, std::shared_ptr<luabridge::LuaRef>> components;

	void Initialize();
	static void CppLog(const std::string& message);

	void InitializeStates();
	void InitializeFunctions();
	void InitializeComponents(std::string componentName);

	void EstablishInheritance(luabridge::LuaRef& instanceTable, luabridge::LuaRef& parentTable);


	static int GetFrame();
	static void CppQuit();
	static void CppSleep(const int& milliseconds);
	static void CppOpenURL(const std::string& URL);
};

#endif

