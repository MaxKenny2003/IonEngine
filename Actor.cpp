#define GLM_ENABLE_EXPERIMENTAL

#include <iostream>
#include <unordered_set>
#include "ThirdParty/glm/glm.hpp"
#include "Actor.h"
#include "gameEngine.h"
#include "Helper.h"
#include "AudioHelper.h"



std::string Actor::GetName()
{
	return name;
}

int Actor::GetID()
{
	return uniqueIdentifier;
}

void Actor::InjectConvienienceReference(std::shared_ptr<luabridge::LuaRef> component_ref)
{
	(*component_ref)["actor"] = this;
}

luabridge::LuaRef Actor::GetComponentByKey(std::string keyToSearchFor)
{
	if (ActorComponents.find(keyToSearchFor) != ActorComponents.end())
	{
		return *ActorComponents.at(keyToSearchFor).second;
	}
	
	return luabridge::LuaRef(currentLuaState);
}

luabridge::LuaRef Actor::GetComponent(std::string typeNameToSearchFor)
{
	//cout << typeNameToSearchFor << endl;
	//cout << "Actor name " << name << endl;
	if (ComponentsByType.find(typeNameToSearchFor) != ComponentsByType.end())
	{
		if (ComponentsByType.at(typeNameToSearchFor).top().component->isInstance<Rigidbody>())
		{
			Rigidbody& rb = ComponentsByType.at(typeNameToSearchFor).top().component->cast<Rigidbody&>();
			if (rb.removed)
			{
				//whomp whomp
			}
			else
			{
				return *ComponentsByType.at(typeNameToSearchFor).top().component;
			}
		}
		else if (ComponentsByType.at(typeNameToSearchFor).top().component->isInstance<ParticleDB>())
		{
			ParticleDB& part = ComponentsByType.at(typeNameToSearchFor).top().component->cast<ParticleDB&>();
			if (part.removed)
			{

			}
			else
			{
				return *ComponentsByType.at(typeNameToSearchFor).top().component;
			}
		}
		else if (!(*ComponentsByType[typeNameToSearchFor].top().component)["removed"])
		{
			return *ComponentsByType[typeNameToSearchFor].top().component;
		}
	}
	return luabridge::LuaRef(pointerToGameEngine->Components.currentLuaState);
}

luabridge::LuaRef Actor::GetComponents(std::string typeNametoSearchFor)
{
	luabridge::LuaRef lua_table = luabridge::newTable(currentLuaState);
	if (ComponentsByKey.find(typeNametoSearchFor) != ComponentsByKey.end())
	{
		// It exists
		std::vector<keyStruct> tempVector;
		std::priority_queue<keyStruct, std::vector<keyStruct>, keyCompartor > tempQueue = ComponentsByKey[typeNametoSearchFor];
		while (!tempQueue.empty())
		{
			tempVector.push_back(tempQueue.top());
			tempQueue.pop();
		}
		int index = 1;
		for (auto& i : tempVector)
		{
			lua_table[index] = *i.component;
			index++;
		}
	}
	return lua_table;
}

luabridge::LuaRef Actor::AddComponent(std::string componentType)
{
	if (componentType != "Rigidbody")
	{
		componentsToAdd temp;
		temp.type = componentType;
		temp.key = "r" + std::to_string(pointerToGameEngine->numTimesAddComponent);
		pointerToGameEngine->numTimesAddComponent++;
		if (pointerToGameEngine->Components.components.find(temp.type) == pointerToGameEngine->Components.components.end())
		{
			pointerToGameEngine->Components.InitializeComponents(componentType);
		}
		luabridge::LuaRef newEmptyTable = luabridge::newTable(pointerToGameEngine->Components.currentLuaState);
		pointerToGameEngine->Components.EstablishInheritance(newEmptyTable, *pointerToGameEngine->Components.components.at(temp.type));
		std::shared_ptr<luabridge::LuaRef> luaTablePointer = std::make_shared<luabridge::LuaRef>(newEmptyTable);
		temp.component = luaTablePointer;
		componentsToAddToActor.push_back(temp);
		return newEmptyTable;
	}
	else if (componentType == "ParticleSystem")
	{
		ParticleDB* newParticle = nullptr;
		componentsToAdd temp;
		temp.key = "ParticleSystem";
		temp.key = "r" + std::to_string(pointerToGameEngine->numTimesAddComponent);
		if (pointerToGameEngine->Components.components.find("ParticleSystem") == pointerToGameEngine->Components.components.end())
		{
			// It is a regular Lua component
			pointerToGameEngine->ImageHandler.CreateDefaultParticleTextureWithName("default");
			ParticleDB temp;
			pointerToGameEngine->Components.InitializeComponents("ParticleSystem");
		}
		ParticleDB* parentComponent = pointerToGameEngine->Components.components.at(name)->cast<ParticleDB*>();
		newParticle = new ParticleDB(*parentComponent);
		newParticle->pointerToEngine = pointerToGameEngine;

		luabridge::LuaRef ref(pointerToGameEngine->Components.currentLuaState, newParticle);
		std::shared_ptr<luabridge::LuaRef> particlePointer = std::make_shared<luabridge::LuaRef>(ref);
		temp.component = particlePointer;
		componentsToAddToActor.push_back(temp);
		return ref;
	}
	else
	{
		Rigidbody* newComponent = nullptr;
		componentsToAdd temp;
		temp.type = "Rigidbody";
		temp.key = "r" + std::to_string(pointerToGameEngine->numTimesAddComponent);
		if (pointerToGameEngine->Components.components.find(name) == pointerToGameEngine->Components.components.end())
		{
			// It is a regular Lua component
			Rigidbody temp;
			pointerToGameEngine->Components.InitializeComponents(name);
			pointerToGameEngine->physics_world = new b2World(pointerToGameEngine->gravity);
			newContactListener* contactListener = new newContactListener();
			pointerToGameEngine->physics_world->SetContactListener(contactListener);
		}
		Rigidbody* parentComponent = pointerToGameEngine->Components.components.at(name)->cast<Rigidbody*>();
		newComponent = new Rigidbody(*parentComponent);

		newComponent->associatedActorUUID = uniqueIdentifier;
		newComponent->associatedActor = this;
		if (!newComponent->hasCollider && !newComponent->has_trigger)
		{
			isPhantom = true;
		}
		//pointerToGameEngine->defineBody(*newComponent);
		//newComponent->defineShapeAndFixture();
		newComponent->Game = pointerToGameEngine;
		luabridge::LuaRef ref(pointerToGameEngine->Components.currentLuaState, newComponent);
		std::shared_ptr<luabridge::LuaRef> RigidBodyPointer = std::make_shared<luabridge::LuaRef>(ref);
		temp.component = RigidBodyPointer;
		componentsToAddToActor.push_back(temp);
		return ref;
	}

}

void Actor::RemoveComponent(luabridge::LuaRef component)
{
	// Note that I'm not actually removing the component, rather I'm just turning it off. 
	// If performance is needed in the future look into turning this off
	if (component.isInstance<Rigidbody>())
	{
		Rigidbody& rb = component.cast<Rigidbody&>();
		rb.OnDestroy();
		rb.removed = true;
	}
	else if (component.isInstance<ParticleDB>())
	{
		ParticleDB& part = component.cast<ParticleDB&>();
		part.removed = true;
	}
	else
	{
		component["enabled"] = false;
		component["removed"] = true;
		for (auto i = 0; i < pointerToGameEngine->OnDestroyComponents.size(); i++)
		{
			if (pointerToGameEngine->OnDestroyComponents[i].UniqueIdentifier == uniqueIdentifier)
			{
				pointerToGameEngine->OnDestryfunctionsToRun.push(pointerToGameEngine->OnDestroyComponents[i]);
			}
		}
	}
}
