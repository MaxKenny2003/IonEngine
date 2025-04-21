#pragma once

#ifndef TEMPLATEDB_H
#define TEMPLATEDB_H

#include <string>
#include <vector>
#include <unordered_map>
#include "Actor.h"
#include "ThirdParty/glm/glm.hpp"
#include "ThirdParty/glm/gtx/hash.hpp"


using namespace std;

class Actor;

class TemplateDB
{
public:
	std::unordered_map<std::string, Actor*> currentTemplates;

	TemplateDB() {};
};

#endif

