#pragma once

#ifndef ENGINEUTILS_H
#define ENGINEUTILS_H
#define GLM_ENABLE_EXPERIMENTAL

#include <string>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <iostream>
#include <filesystem>

#include "ThirdParty/rapidjson/include/rapidjson/filereadstream.h"
#include "ThirdParty/rapidjson/include/rapidjson/document.h"
#include "ThirdParty/glm/gtx/hash.hpp"
#include "gameEngine.h"
#include "Actor.h"
#include "SceneDB.h"
#include "TemplateDB.h"
#include "lua.hpp"
#include "ThirdParty/LuaBridge/LuaBridge.h"
#include "include/box2d/box2d.h"
#include "Rigidbody.h"
#include "ParticleDB.h"

using namespace std;

class EngineUtils
{
public:
	static void ReadJsonFile(const std::string& path, rapidjson::Document& out_document)
	{
		FILE* file_pointer = nullptr;
#ifdef _WIN32
		fopen_s(&file_pointer, path.c_str(), "rb");
#else
		file_pointer = fopen(path.c_str(), "rb");
#endif
		char buffer[65536];
		rapidjson::FileReadStream stream(file_pointer, buffer, sizeof(buffer));
		out_document.ParseStream(stream);
		std::fclose(file_pointer);

		if (out_document.HasParseError()) {
			rapidjson::ParseErrorCode errorCode = out_document.GetParseError();
			std::cout << "error parsing json at [" << path << "]" << std::endl;
			exit(0);

		}
	}

	static void ReadInitialScene(rapidjson::Document& out_document, gameEngine* Game)
	{
		if (out_document.HasMember("initial_scene"))
		{
			std::string initialScene = out_document["initial_scene"].GetString();
			std::string path = "resources/scenes/";
			path += initialScene;
			path += ".scene";
			if (std::filesystem::exists(path))
			{
				// The file does exit, and there is a path to it
				ReadJsonFile(path, Game->currentDoc);
				parseScene(Game, initialScene);
			}
			else
			{
				cout << "error: scene " << initialScene << " is missing";
				exit(0);
			}
		}
		else
		{
			// Error if the initial scene is not specified
			cout << "error: initial_scene unspecified";
			exit(0);
		}
	}

	static void parseScene(gameEngine* Game, std::string SceneName)
	{
		SceneDB currentScene;
		currentScene.currentLuaState = Game->Components.currentLuaState;
		Game->currentScene = SceneName;
		currentScene.sceneName = SceneName;
		string path = "resources/component_types/";
		string fileType = ".lua";
		const rapidjson::Value& actors = Game->currentDoc["actors"];
		for (int i = 0; i < actors.Size(); i++)
		{
			Actor* currentActor = new Actor();
			const rapidjson::Value& actor = actors[i];
			// Check for any template
			if (actor.HasMember("template"))
			{
				if (Game->DefinedTemplates.currentTemplates.find(actor["template"].GetString()) != Game->DefinedTemplates.currentTemplates.end())
				{
					currentActor = Game->DefinedTemplates.currentTemplates.at(actor["template"].GetString());
				}
				else
				{
					readTemplate(actor["template"].GetString(), Game);
					currentActor = Game->DefinedTemplates.currentTemplates.at(actor["template"].GetString());
				}
				for (auto componentItr = currentActor->ActorComponents.begin(); componentItr != currentActor->ActorComponents.end(); ++componentItr)
				{
					luabridge::LuaRef newEmptyTable = luabridge::newTable(Game->Components.currentLuaState);
					Game->Components.EstablishInheritance(newEmptyTable, *componentItr->second.second);
					std::shared_ptr<luabridge::LuaRef> luaTablePointer = std::make_shared<luabridge::LuaRef>(newEmptyTable);
					std::pair ComponentNameAndLuaRef = std::make_pair(componentItr->second.first, luaTablePointer);
					if (currentActor->ActorComponents.find(componentItr->first) != currentActor->ActorComponents.end())
					{
						currentActor->ActorComponents[componentItr->first] = ComponentNameAndLuaRef;
					}
					else
					{
						currentActor->ActorComponents.insert({ componentItr->first, ComponentNameAndLuaRef });
					}
				}
			}
			if (actor.HasMember("name"))
			{
				currentActor->name = actor["name"].GetString();
			}
			currentActor->uniqueIdentifier = currentActor->UUID++;		// Set unique identifier of the actor
			Rigidbody* newComponent = nullptr;
			ParticleDB* newParticle = nullptr;
			if (actor.HasMember("components") && actor["components"].IsObject())
			{
				bool isRigid = false;
				bool isParticle = false;
				const rapidjson::Value& components = actor["components"];
				for (rapidjson::Value::ConstMemberIterator itr = components.MemberBegin(); itr != components.MemberEnd(); ++itr) {
					// Loop the components
					string key = itr->name.GetString();	// This is correct

					const auto& fields = itr->value;

					for (rapidjson::Value::ConstMemberIterator itr2 = fields.MemberBegin(); itr2 != fields.MemberEnd(); ++itr2)
					{
						if (itr2->name == "type")
						{
							string name = fields["type"].GetString();
							if (name == "Rigidbody")
							{
								isRigid = true;
								// It is a rigidbody
								if (Game->Components.components.find(name) == Game->Components.components.end())
								{
									// It is a regular Lua component
									Rigidbody temp;
									Game->Components.InitializeComponents(name);
									Game->physics_world = new b2World(Game->gravity);
									newContactListener* contactListener = new newContactListener();
									Game->physics_world->SetContactListener(contactListener);
								}
								Rigidbody* parentComponent = Game->Components.components.at(name)->cast<Rigidbody*>();
								newComponent = new Rigidbody(*parentComponent);
							}
							else if(name == "ParticleSystem")
							{
								isParticle = true;
								if (Game->Components.components.find("ParticleSystem") == Game->Components.components.end())
								{
									// It is a regular Lua component
									Game->ImageHandler.CreateDefaultParticleTextureWithName("default");
									ParticleDB temp;
									Game->Components.InitializeComponents("ParticleSystem");
								}
								ParticleDB* parentComponent = Game->Components.components.at(name)->cast<ParticleDB*>();
								newParticle = new ParticleDB(*parentComponent);
								newParticle->pointerToEngine = Game;
							}
							else if (std::filesystem::exists(path + name + fileType))
							{
								if (Game->Components.components.find(name) == Game->Components.components.end())
								{
										// It is a regular Lua component
										Game->Components.InitializeComponents(name);
								}
								//Do inheritence so we always have an empty table. 
								luabridge::LuaRef newEmptyTable = luabridge::newTable(Game->Components.currentLuaState);
								Game->Components.EstablishInheritance(newEmptyTable, *Game->Components.components.at(name));
								newEmptyTable["key"] = key;
								std::shared_ptr<luabridge::LuaRef> luaTablePointer = std::make_shared<luabridge::LuaRef>(newEmptyTable);
								std::pair ComponentNameAndLuaRef = std::make_pair(name, luaTablePointer);
								currentActor->pointerToGameEngine = Game;
								if (currentActor->ActorComponents.find(key) != currentActor->ActorComponents.end())
								{
									currentActor->ActorComponents[key] = ComponentNameAndLuaRef;
								}
								else
								{
									currentActor->ActorComponents.insert({ key, ComponentNameAndLuaRef });
								}

								typeStruct currentComponentType = { name, key, luaTablePointer };
								currentActor->ComponentsByType[currentComponentType.type].push(currentComponentType);
								keyStruct currentComponentKey = { name, key, luaTablePointer, currentActor->uniqueIdentifier };
								currentActor->ComponentsByKey[currentComponentKey.type].push(currentComponentKey);
							}
							else
							{
								cout << "error: failed to locate component " << name;
								exit(0);
							}
						}
						else
						{
							if (isRigid)
							{
								std::string updateThis = itr2->name.GetString();

								if (updateThis == "x")
								{
									newComponent->x = itr2->value.GetFloat();
								}
								if (updateThis == "y")
								{
									newComponent->y = itr2->value.GetFloat();
								}
								if (updateThis == "body_type")
								{
									newComponent->body_type = itr2->value.GetString();
								}
								if (updateThis == "precis")
								{
									newComponent->precise = itr2->value.GetBool();
								}
								if (updateThis == "gravity_scale")
								{
									newComponent->gravity_scale = itr2->value.GetFloat();
								}
								if (updateThis == "density")
								{
									newComponent->density = itr2->value.GetFloat();
								}
								if (updateThis == "angular_friction")
								{
									newComponent->angularFriction = itr2->value.GetFloat();
								}
								if (updateThis == "rotation")
								{
									newComponent->rotation = itr2->value.GetFloat();
								}
								if (updateThis == "has_collider")
								{
									newComponent->hasCollider = itr2->value.GetBool();
								}
								if (updateThis == "has_trigger")
								{
									newComponent->has_trigger = itr2->value.GetBool();
								}
								if (updateThis == "collider_type")
								{
									newComponent->colliderType = itr2->value.GetString();
								}
								if (updateThis == "width")
								{
									newComponent->width = itr2->value.GetFloat();
								}
								if (updateThis == "height")
								{
									newComponent->height = itr2->value.GetFloat();
								}
								if (updateThis == "radius")
								{
									newComponent->radius = itr2->value.GetFloat();
								}
								if (updateThis == "friction")
								{
									newComponent->friction = itr2->value.GetFloat();
								}
								if (updateThis == "bounciness")
								{
									newComponent->bounciness = itr2->value.GetFloat();
								}
								if (updateThis == "trigger_type")
								{
									newComponent->triggerType = itr2->value.GetString();
								}
								if (updateThis == "trigger_width")
								{
									newComponent->triggerWidth = itr2->value.GetFloat();
								}
								if (updateThis == "trigger_height")
								{
									newComponent->triggerHeight = itr2->value.GetFloat();
								}
								if (updateThis == "trigger_radius")
								{
									newComponent->triggerRadius = itr2->value.GetFloat();
								}
							}
							else if (isParticle)
							{
								std::string updateThis = itr2->name.GetString();
								if (updateThis == "x")
								{
									newParticle->startingX = itr2->value.GetFloat();
								}
								if (updateThis == "y")
								{
									newParticle->startingY = itr2->value.GetFloat();
								}
								if (updateThis == "frames_between_bursts")
								{
									int temp = itr2->value.GetInt();
									if (temp < 1)
									{
										temp = 1;
									}
									newParticle->frames_between_bursts = temp;
								}
								if (updateThis == "burst_quantity")
								{
									int temp = itr2->value.GetInt();
									if (temp < 1)
									{
										temp = 1;
									}
									newParticle->burst_quantity = temp;
								}
								if (updateThis == "start_scale_min")
								{
									newParticle->start_scale_min = itr2->value.GetFloat();
								}
								if (updateThis == "start_scale_max")
								{
									newParticle->start_scale_max = itr2->value.GetFloat();
								}
								if (updateThis == "rotation_min")
								{
									newParticle->rotation_min = itr2->value.GetFloat();
								}
								if (updateThis == "rotation_max")
								{
									newParticle->rotation_max = itr2->value.GetFloat();
								}
								if (updateThis == "start_color_r")
								{
									newParticle->r = itr2->value.GetInt();
								}
								if (updateThis == "start_color_g")
								{
									newParticle->g = itr2->value.GetInt();
								}
								if (updateThis == "start_color_b")
								{
									newParticle->b = itr2->value.GetInt();
								}
								if (updateThis == "start_color_a")
								{
									newParticle->a = itr2->value.GetInt();
								}
								if (updateThis == "emit_radius_min")
								{
									newParticle->emit_radius_min = itr2->value.GetFloat();
								}
								if (updateThis == "emit_radius_max")
								{
									newParticle->emit_radius_max = itr2->value.GetFloat();
								}
								if (updateThis == "emit_angle_min")
								{
									newParticle->emit_angle_min = itr2->value.GetFloat();
								}
								if (updateThis == "emit_angle_max")
								{
									newParticle->emit_angle_max = itr2->value.GetFloat();
								}
								if (updateThis == "image")
								{
									newParticle->textureName = itr2->value.GetString();
									Game->ImageHandler.CheckForImage(newParticle->textureName);
								}
								if (updateThis == "sorting_order")
								{
									newParticle->sortingOrder = itr2->value.GetInt();
								}
								if (updateThis == "duration_frames")
								{
									int temp = itr2->value.GetInt();
									if (temp < 1) temp = 1;
									newParticle->duration_frames = temp;
								}
								if (updateThis == "start_speed_min")
								{
									newParticle->start_speed_min = itr2->value.GetFloat();
								}
								if (updateThis == "start_speed_max")
								{
									newParticle->start_speed_max = itr2->value.GetFloat();
								}
								if (updateThis == "rotation_speed_min")
								{
									newParticle->rotation_speed_min = itr2->value.GetFloat();
								}
								if (updateThis == "rotation_speed_max")
								{
									newParticle->rotation_speed_max = itr2->value.GetFloat();
								}
								if (updateThis == "gravity_scale_x")
								{
									newParticle->gravity_scale_x = itr2->value.GetFloat();
								}
								if (updateThis == "gravity_scale_y")
								{
									newParticle->gravity_scale_y = itr2->value.GetFloat();
								}
								if (updateThis == "drag_factor")
								{
									newParticle->drag_factor = itr2->value.GetFloat();
								}
								if (updateThis == "angular_drag_factor")
								{
									newParticle->angular_drag_factor = itr2->value.GetFloat();
								}
								if (updateThis == "end_scale")
								{
									newParticle->end_scale = itr2->value.GetFloat();
								}
								if (updateThis == "end_color_r")
								{
									newParticle->end_color_r = itr2->value.GetInt();
								}
								if (updateThis == "end_color_g")
								{
									newParticle->end_color_g = itr2->value.GetInt();
								}
								if (updateThis == "end_color_b")
								{
									newParticle->end_color_b = itr2->value.GetInt();
								}
								if (updateThis == "end_color_a")
								{
									newParticle->end_color_a = itr2->value.GetInt();
								}
								
							}
							else
							{
								// There are variables that need to be overwritten
								string name;
								name = currentActor->ActorComponents.at(key).first;
								luabridge::LuaRef currentTable = *currentActor->ActorComponents.at(key).second;  // Now have to the table

								if (itr2->value.IsString())
								{
									currentTable[itr2->name.GetString()] = itr2->value.GetString();
								}
								else if (itr2->value.IsInt())
								{
									currentTable[itr2->name.GetString()] = itr2->value.GetInt();
								}
								else if (itr2->value.IsFloat())
								{
									currentTable[itr2->name.GetString()] = itr2->value.GetFloat();
								}
								else if (itr2->value.IsBool())
								{
									currentTable[itr2->name.GetString()] = itr2->value.GetBool();
								}

								//currentTable["enabled"] = true;


								std::shared_ptr<luabridge::LuaRef> luaTablePointer = std::make_shared<luabridge::LuaRef>(currentTable);
								std::pair ComponentNameAndLuaRef = std::make_pair(name, luaTablePointer);
								if (currentActor->ActorComponents.find(key) != currentActor->ActorComponents.end())
								{
									currentActor->ActorComponents[key] = ComponentNameAndLuaRef;
								}
								else
								{
									currentActor->ActorComponents.insert({ key, ComponentNameAndLuaRef });
								}
							}
						}
					}
					if (isRigid)
					{
						isRigid = false;
						newComponent->associatedActorUUID = currentActor->uniqueIdentifier;
						newComponent->associatedActor = currentActor;
						if (!newComponent->hasCollider && !newComponent->has_trigger)
						{
							currentActor->isPhantom = true;
						}
						//Game->defineBody(*newComponent);
						//newComponent->defineShapeAndFixture();
						newComponent->Game = Game;
						luabridge::LuaRef ref(Game->Components.currentLuaState, newComponent);
						std::shared_ptr<luabridge::LuaRef> RigidBodyPointer = std::make_shared<luabridge::LuaRef>(ref);
						std::pair ComponentNameAndLuaRef = std::make_pair("Rigidbody", RigidBodyPointer);
						typeStruct currentComponentType = { "Rigidbody", key, RigidBodyPointer};
						currentActor->ComponentsByType[currentComponentType.type].push(currentComponentType);
						keyStruct currentComponentKey = { "Rigidbody", key, RigidBodyPointer, currentActor->uniqueIdentifier };
						currentActor->ComponentsByKey[currentComponentKey.type].push(currentComponentKey);
						currentActor->ActorComponents.insert({ key, ComponentNameAndLuaRef });
					}
					else if (isParticle)
					{
						isParticle = false;
						luabridge::LuaRef ref(Game->Components.currentLuaState, newParticle);
						std::shared_ptr<luabridge::LuaRef> ParticlePointer = std::make_shared<luabridge::LuaRef>(ref);
						std::pair ComponentNameAndLuaRef = std::make_pair("ParticleSystem", ParticlePointer);
						typeStruct currentComponentType = { "ParticleSystem", key, ParticlePointer };
						currentActor->ComponentsByType[currentComponentType.type].push(currentComponentType);
						keyStruct currentComponentKey = { "ParticleSystem", key, ParticlePointer, currentActor->uniqueIdentifier };
						currentActor->ComponentsByKey[currentComponentKey.type].push(currentComponentKey);
						currentActor->ActorComponents.insert({ key, ComponentNameAndLuaRef });
					}
				}
			}

			// All of the information has now been loaded in. 
			// sceneMap2  is a map of positions of actors
			// sceneMap is a 2D vector map of the whole scene 
			// uniqueIdentifiermap is a map with the unique identifier highlighted

			
			currentActor->currentLuaState = Game->Components.currentLuaState;
			currentScene.ActorsInScene.push_back(currentActor);		// Add the currentActor to the Scene actor array
			
		}
		Game->SceneVector.insert({ currentScene.sceneName, currentScene });	//insert the sceneName and the currentScene into the SceneVector

		for (int i = 0; i < Game->SceneVector.at(Game->currentScene).ActorsInScene.size(); i++)
		{
			// For loop for creating the correct pointers
			Actor* storedActor = Game->SceneVector.at(Game->currentScene).ActorsInScene.at(i);						// Initialize a pointer to the ith actor in the actor array
			
		}
	}

	static void readTemplate(std::string path, gameEngine* Game)
	{
		std::string currentPath = "resources/actor_templates/" + path + ".template";

		if (std::filesystem::exists(currentPath))
		{
			ReadJsonFile(currentPath, Game->currentTempDoc);
			parseTemplate(Game, path);
		}
		else
		{
			cout << "error: template " << path << " is missing";
			exit(0);
		}
	}

	static void parseTemplate(gameEngine* Game, std::string path)
	{
		std::string templateName = path;
		Actor* templateActor = new Actor();
		string Componentpath = "resources/component_types/";
		string fileType = ".lua";

		if (Game->currentTempDoc.HasMember("name"))
		{
			templateActor->name = Game->currentTempDoc["name"].GetString();
		}
		if (Game->currentTempDoc.HasMember("components") && Game->currentTempDoc["components"].IsObject())
		{
			Rigidbody* newComponent = nullptr;
			ParticleDB* newParticle = nullptr;
			bool isRigid = false;
			bool isParticle = false;
			const rapidjson::Value& components = Game->currentTempDoc["components"];
			for (rapidjson::Value::ConstMemberIterator itr = components.MemberBegin(); itr != components.MemberEnd(); ++itr) {
				// Loop the components
				string key = itr->name.GetString();

				const auto& fields = itr->value;

				for (rapidjson::Value::ConstMemberIterator itr2 = fields.MemberBegin(); itr2 != fields.MemberEnd(); ++itr2)
				{
					if (itr2->name == "type")
					{
						string name;
						if (fields.HasMember("type"))
						{
							name = fields["type"].GetString();
						}
						if (name == "Rigidbody")
						{
							isRigid = true;
							// It is a rigidbody
							if (Game->Components.components.find(name) == Game->Components.components.end())
							{
								// It is a regular Lua component
								Rigidbody temp;
								Game->Components.InitializeComponents(name);
								Game->physics_world = new b2World(Game->gravity);
								newContactListener* contactListener = new newContactListener();
								Game->physics_world->SetContactListener(contactListener);
							}
							Rigidbody* parentComponent = Game->Components.components.at(name)->cast<Rigidbody*>();
							newComponent = new Rigidbody(*parentComponent);
						}
						else if (name == "ParticleSystem")
						{
							isParticle = true;
							if (Game->Components.components.find("ParticleSystem") == Game->Components.components.end())
							{
								// It is a regular Lua component
								Game->ImageHandler.CreateDefaultParticleTextureWithName("default");
								ParticleDB temp;
								Game->Components.InitializeComponents("ParticleSystem");
							}
							ParticleDB* parentComponent = Game->Components.components.at(name)->cast<ParticleDB*>();
							newParticle = new ParticleDB(*parentComponent);
							newParticle->pointerToEngine = Game;
						}
						else if (std::filesystem::exists(Componentpath + name + fileType))
						{
							if (Game->Components.components.find(name) == Game->Components.components.end())
							{
								// It is a regular Lua component
								Game->Components.InitializeComponents(name);
							}
							//Do inheritence so we always have an empty table. 

							luabridge::LuaRef newEmptyTable = luabridge::newTable(Game->Components.currentLuaState);
							Game->Components.EstablishInheritance(newEmptyTable, *Game->Components.components.at(name));
							newEmptyTable["key"] = key;
							std::shared_ptr<luabridge::LuaRef> luaTablePointer = std::make_shared<luabridge::LuaRef>(newEmptyTable);
							std::pair ComponentNameAndLuaRef = std::make_pair(name, luaTablePointer);
							if (templateActor->ActorComponents.find(key) != templateActor->ActorComponents.end())
							{
								templateActor->ActorComponents[key] = ComponentNameAndLuaRef;
							}
							else
							{
								templateActor->ActorComponents.insert({ key, ComponentNameAndLuaRef });
							}
							typeStruct currentComponentType = { name, key, luaTablePointer };
							templateActor->ComponentsByType[currentComponentType.type].push(currentComponentType);
							keyStruct currentComponentKey = { name, key, luaTablePointer, templateActor->uniqueIdentifier };
							templateActor->ComponentsByKey[currentComponentKey.type].push(currentComponentKey);
						}
						else
						{
							cout << "error: failed to locate component " << name;
							exit(0);
						}
					}
					else
					{
						if (isRigid)
						{
							std::string updateThis = itr2->name.GetString();

							if (updateThis == "x")
							{
								newComponent->x = itr2->value.GetFloat();
							}
							if (updateThis == "y")
							{
								newComponent->y = itr2->value.GetFloat();
							}
							if (updateThis == "body_type")
							{
								newComponent->body_type = itr2->value.GetString();
							}
							if (updateThis == "precis")
							{
								newComponent->precise = itr2->value.GetBool();
							}
							if (updateThis == "gravity_scale")
							{
								newComponent->gravity_scale = itr2->value.GetFloat();
							}
							if (updateThis == "density")
							{
								newComponent->density = itr2->value.GetFloat();
							}
							if (updateThis == "angular_friction")
							{
								newComponent->angularFriction = itr2->value.GetFloat();
							}
							if (updateThis == "rotation")
							{
								newComponent->rotation = (itr2->value.GetFloat());
							}
							if (updateThis == "has_collider")
							{
								newComponent->hasCollider = itr2->value.GetBool();
							}
							if (updateThis == "has_trigger")
							{
								newComponent->has_trigger = itr2->value.GetBool();
							}
							if (updateThis == "collider_type")
							{
								newComponent->colliderType = itr2->value.GetString();
							}
							if (updateThis == "width")
							{
								newComponent->width = itr2->value.GetFloat();
							}
							if (updateThis == "height")
							{
								newComponent->height = itr2->value.GetFloat();
							}
							if (updateThis == "radius")
							{
								newComponent->radius = itr2->value.GetFloat();
							}
							if (updateThis == "friction")
							{
								newComponent->friction = itr2->value.GetFloat();
							}
							if (updateThis == "bounciness")
							{
								newComponent->bounciness = itr2->value.GetFloat();
							}
							if (updateThis == "trigger_type")
							{
								newComponent->triggerType = itr2->value.GetString();
							}
							if (updateThis == "trigger_width")
							{
								newComponent->triggerWidth = itr2->value.GetFloat();
							}
							if (updateThis == "trigger_height")
							{
								newComponent->triggerHeight = itr2->value.GetFloat();
							}
							if (updateThis == "trigger_radius")
							{
								newComponent->triggerRadius = itr2->value.GetFloat();
							}
						}
						else if (isParticle)
						{
							std::string updateThis = itr2->name.GetString();
							if (updateThis == "x")
							{
								newParticle->startingX = itr2->value.GetFloat();
							}
							if (updateThis == "y")
							{
								newParticle->startingY = itr2->value.GetFloat();
							}
							if (updateThis == "frames_between_bursts")
							{
								int temp = itr2->value.GetInt();
								if (temp < 1)
								{
									temp = 1;
								}
								newParticle->frames_between_bursts = temp;
							}
							if (updateThis == "burst_quantity")
							{
								int temp = itr2->value.GetInt();
								if (temp < 1)
								{
									temp = 1;
								}
								newParticle->burst_quantity = temp;
							}
							if (updateThis == "start_scale_min")
							{
								newParticle->start_scale_min = itr2->value.GetFloat();
							}
							if (updateThis == "start_scale_max")
							{
								newParticle->start_scale_max = itr2->value.GetFloat();
							}
							if (updateThis == "rotation_min")
							{
								newParticle->rotation_min = itr2->value.GetFloat();
							}
							if (updateThis == "rotation_max")
							{
								newParticle->rotation_max = itr2->value.GetFloat();
							}
							if (updateThis == "start_color_r")
							{
								newParticle->r = itr2->value.GetInt();
							}
							if (updateThis == "start_color_g")
							{
								newParticle->g = itr2->value.GetInt();
							}
							if (updateThis == "start_color_b")
							{
								newParticle->b = itr2->value.GetInt();
							}
							if (updateThis == "start_color_a")
							{
								newParticle->a = itr2->value.GetInt();
							}
							if (updateThis == "emit_radius_min")
							{
								newParticle->emit_radius_min = itr2->value.GetFloat();
							}
							if (updateThis == "emit_radius_max")
							{
								newParticle->emit_radius_max = itr2->value.GetFloat();
							}
							if (updateThis == "emit_angle_min")
							{
								newParticle->emit_angle_min = itr2->value.GetFloat();
							}
							if (updateThis == "emit_angle_max")
							{
								newParticle->emit_angle_max = itr2->value.GetFloat();
							}
							if (updateThis == "image")
							{
								newParticle->textureName = itr2->value.GetString();
								Game->ImageHandler.CheckForImage(newParticle->textureName);
							}
							if (updateThis == "sorting_order")
							{
								newParticle->sortingOrder = itr2->value.GetInt();
							}
							if (updateThis == "duration_frames")
							{
								int temp = itr2->value.GetInt();
								if (temp < 1) temp = 1;
								newParticle->duration_frames = temp;
							}
							if (updateThis == "start_speed_min")
							{
								newParticle->start_speed_min = itr2->value.GetFloat();
							}
							if (updateThis == "start_speed_max")
							{
								newParticle->start_speed_max = itr2->value.GetFloat();
							}
							if (updateThis == "rotation_speed_min")
							{
								newParticle->rotation_speed_min = itr2->value.GetFloat();
							}
							if (updateThis == "rotation_speed_max")
							{
								newParticle->rotation_speed_max = itr2->value.GetFloat();
							}
							if (updateThis == "gravity_scale_x")
							{
								newParticle->gravity_scale_x = itr2->value.GetFloat();
							}
							if (updateThis == "gravity_scale_y")
							{
								newParticle->gravity_scale_y = itr2->value.GetFloat();
							}
							if (updateThis == "drag_factor")
							{
								newParticle->drag_factor = itr2->value.GetFloat();
							}
							if (updateThis == "angular_drag_factor")
							{
								newParticle->angular_drag_factor = itr2->value.GetFloat();
							}
							if (updateThis == "end_scale")
							{
								newParticle->end_scale = itr2->value.GetFloat();
							}
							if (updateThis == "end_color_r")
							{
								newParticle->end_color_r = itr2->value.GetInt();
							}
							if (updateThis == "end_color_g")
							{
								newParticle->end_color_g = itr2->value.GetInt();
							}
							if (updateThis == "end_color_b")
							{
								newParticle->end_color_b = itr2->value.GetInt();
							}
							if (updateThis == "end_color_a")
							{
								newParticle->end_color_a = itr2->value.GetInt();
							}
						}
						else
						{
							// There are variables that need to be overwritten
							string name = fields["type"].GetString();
							luabridge::LuaRef currentTable = *templateActor->ActorComponents.at(key).second;  // Now have to the table

							if (itr2->value.IsString())
							{
								currentTable[itr2->name.GetString()] = itr2->value.GetString();
							}
							else if (itr2->value.IsInt())
							{
								currentTable[itr2->name.GetString()] = itr2->value.GetInt();
							}
							else if (itr2->value.IsFloat())
							{
								currentTable[itr2->name.GetString()] = itr2->value.GetFloat();
							}
							else if (itr2->value.IsBool())
							{
								currentTable[itr2->name.GetString()] = itr2->value.GetBool();
							}
							//currentTable["enabled"] = true;

							std::shared_ptr<luabridge::LuaRef> luaTablePointer = std::make_shared<luabridge::LuaRef>(currentTable);
							std::pair ComponentNameAndLuaRef = std::make_pair(name, luaTablePointer);
							if (templateActor->ActorComponents.find(key) != templateActor->ActorComponents.end())
							{
								templateActor->ActorComponents[key] = ComponentNameAndLuaRef;
							}
							else
							{
								templateActor->ActorComponents.insert({ key, ComponentNameAndLuaRef });
							}
							/*typeStruct currentComponentType = { name, key, luaTablePointer };
							templateActor.ComponentsByType[currentComponentType.type].push(currentComponentType);
							keyStruct currentComponentKey = { name, key, luaTablePointer, templateActor.uniqueIdentifier };
							templateActor.ComponentsByKey[currentComponentKey.type].push(currentComponentKey);*/
						}
					}
				}
				if (isRigid)
				{
					isRigid = false;
					//Game->defineBody(*newComponent);
					//newComponent->defineShapeAndFixture();
					//newComponent->Game = Game;
					luabridge::LuaRef ref(Game->Components.currentLuaState, newComponent);
					std::shared_ptr<luabridge::LuaRef> RigidBodyPointer = std::make_shared<luabridge::LuaRef>(ref);
					std::pair ComponentNameAndLuaRef = std::make_pair("Rigidbody", RigidBodyPointer);
					typeStruct currentComponentType = { "Rigidbody", key, RigidBodyPointer};
					templateActor->ComponentsByType[currentComponentType.type].push(currentComponentType);
					keyStruct currentComponentKey = { "Rigidbody", key, RigidBodyPointer, templateActor->uniqueIdentifier };
					templateActor->ComponentsByKey[currentComponentKey.type].push(currentComponentKey);
					templateActor->ActorComponents.insert({ key, ComponentNameAndLuaRef });

				}
				else if (isParticle)
				{
					isParticle = false;
					luabridge::LuaRef ref(Game->Components.currentLuaState, newParticle);
					std::shared_ptr<luabridge::LuaRef> ParticlePointer = std::make_shared<luabridge::LuaRef>(ref);
					std::pair ComponentNameAndLuaRef = std::make_pair("ParticleSystem", ParticlePointer);
					typeStruct currentComponentType = { "ParticleSystem", key, ParticlePointer };
					templateActor->ComponentsByType[currentComponentType.type].push(currentComponentType);
					keyStruct currentComponentKey = { "ParticleSystem", key, ParticlePointer, templateActor->uniqueIdentifier };
					templateActor->ComponentsByKey[currentComponentKey.type].push(currentComponentKey);
					templateActor->ActorComponents.insert({ key, ComponentNameAndLuaRef });
				}
			}
		}
		Game->DefinedTemplates.currentTemplates.insert({ templateName, templateActor });
	}

	static void parseGameConfig(gameEngine* Game)
	{
		if (Game->currentDoc.HasMember("game_title"))
		{
			Game->gameTitle = Game->currentDoc["game_title"].GetString();
		}
		if (Game->currentDoc.HasMember("x_resolution"))
		{
			Game->WindowXResolution = Game->currentDoc["x_resolution"].GetFloat();
		}
		if (Game->currentDoc.HasMember("y_resolution"))
		{
			Game->WindowYResolution = Game->currentDoc["y_resolution"].GetFloat();
		}
	}

	static void parseRenderingConfig(gameEngine* Game)
	{
		if (Game->currentDoc.HasMember("clear_color_r"))
		{
			Game->ClearColorRed = Game->currentDoc["clear_color_r"].GetInt();
		}
		if (Game->currentDoc.HasMember("clear_color_g"))
		{
			Game->ClearColorGreen = Game->currentDoc["clear_color_g"].GetInt();
		}
		if (Game->currentDoc.HasMember("clear_color_b"))
		{
			Game->ClearColorBlue = Game->currentDoc["clear_color_b"].GetInt();
		}
		if (Game->currentDoc.HasMember("cam_ease_factor"))
		{
			Game->cam_ease_factor = Game->currentDoc["cam_ease_factor"].GetFloat();
		}
		if (Game->currentDoc.HasMember("x_resolution"))
		{
			Game->WindowXResolution = Game->currentDoc["x_resolution"].GetFloat();
		}
		if (Game->currentDoc.HasMember("y_resolution"))
		{
			Game->WindowYResolution = Game->currentDoc["y_resolution"].GetFloat();
		}
	}


	static void parseCameraOffset(gameEngine* Game)
	{
		if (Game->currentDoc.HasMember("cam_offset_x"))
		{
			Game->cameraOffsetX = Game->currentDoc["cam_offset_x"].GetFloat();
		}
		if (Game->currentDoc.HasMember("cam_offset_y"))
		{
			Game->cameraOffsetY = Game->currentDoc["cam_offset_y"].GetFloat();
		}
		if (Game->currentDoc.HasMember("zoom_factor"))
		{
			Game->cameraZoom = Game->currentDoc["zoom_factor"].GetFloat();
		}
	}
};

#endif 
