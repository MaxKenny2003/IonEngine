// gameEngine.cpp
#define GLM_ENABLE_EXPERIMENTAL
#include <string>
#include <iostream>
#include <filesystem>
#include "gameEngine.h"
#include "EngineUtils.h"
#include "SceneDB.h"  // Include here in the .cpp file
#include "TemplateDB.h"
#include "Helper.h"
#include "AudioHelper.h"
#include "ComponentDB.h"

using namespace std;



void gameEngine::getGameMessages(gameEngine *Game)
{
    std::string path = "resources/game.config";
    std::string dirPath = "resources/";
    EngineUtils engine;
    if (std::filesystem::exists(dirPath) && std::filesystem::is_directory(dirPath))
    {
        if (std::filesystem::exists(path))
        {
            engine.ReadJsonFile(path, Game->currentDoc);
        }
        else
        {
            cout << "error: resources/game.config missing";
            exit(0);
        }
    }
    else
    {
        cout << "error: resources/ missing";
        exit(0);
    }
    
    // Do Camera Changing
}

void gameEngine::getCameraSize(gameEngine* Game)
{
    std::string path = "resources/rendering.config";
    std::string dirPath = "resources/";
    EngineUtils engine;
    if (std::filesystem::exists(path))
    {
        engine.ReadJsonFile(path, Game->currentDoc);
    }
}

void gameEngine::getDocument(gameEngine* Game, std::string path)
{
    EngineUtils engine;
    if (std::filesystem::exists(path))
    {
        engine.ReadJsonFile(path, Game->currentDoc);
    }
}

void gameEngine::getScene(gameEngine* Game)
{
    std::string path = "resources/game.config";
    EngineUtils engine;
    engine.ReadJsonFile(path, Game->currentDoc);
    //engine.ReadInitialScene(Game->currentDoc, Game);
}

std::string gameEngine::obtain_word_after_phrase(const std::string& input, const std::string& phrase)
{
    size_t pos = input.find(phrase);

    if (pos == std::string::npos) return "";

    pos += phrase.length();

    while (pos < input.size() && std::isspace(input[pos]))
    {
        ++pos;
    }

    if (pos == input.size()) return "";

    size_t endPos = pos;
    while (endPos < input.size() && !std::isspace(input[endPos]))
    {
        ++endPos;
    }

    return input.substr(pos, endPos - pos);
}

void gameEngine::clearFunctionPQ()
{
    while (!functionsToRun.empty())
    {
        auto& currentPair = functionsToRun.top();
        luabridge::LuaRef component = *currentPair.component;
        luabridge::LuaRef luaFunction = *currentPair.function;

        if (component.isInstance<Rigidbody>())
        {
            Rigidbody& rb = component.cast<Rigidbody&>();
            rb.OnStart();
            rb.started = true;
        }
        else if (component.isInstance<ParticleDB>())
        {
            ParticleDB& particle = component.cast<ParticleDB&>();
            if (!particle.removed)
            {
                if (particle.started)
                {
                    // the particle has already started.
                    if (particle.enabled)
                    {
                        particle.OnUpdate();
                    }
                }
                else
                {
                    particle.OnStart();
                    particle.started = true;
                }
            }
        }
        else {
            if (component["enabled"] == true  && !component["removed"])
            {
                try {
                    luaFunction(component);
                }
                catch (const luabridge::LuaException& e) {
                    ReportErrorMessage(currentPair.actorName, e);
                    // need to implement this function 
                }
            }
        }
        functionsToRun.pop();
    }
}


void gameEngine::checkForFunctions(std::string key, std::pair<std::string, std::shared_ptr<luabridge::LuaRef>> currentComponent, int uniqueIdent, std::string actorName)
{
    std::string path = "resources/component_types/";
    std::string fileName = path + currentComponent.first + ".lua";
    if (currentComponent.first != "Rigidbody" && currentComponent.first != "ParticleSystem")
    {
        if (luaL_dofile(Components.currentLuaState, fileName.c_str()) != LUA_OK) {
            std::cout << "problem with lua file " << currentComponent.first;
            exit(0);
        }
    }
    luabridge::LuaRef component = *(currentComponent.second);
    luabridge::LuaRef onStartFunc = component["OnStart"];
    luabridge::LuaRef onUpdateFunc = component["OnUpdate"];
    luabridge::LuaRef onLateUpdate = component["OnLateUpdate"];
    luabridge::LuaRef onDestroy = component["OnDestroy"];


    std::shared_ptr<luabridge::LuaRef> componentPointer;
    std::shared_ptr<luabridge::LuaRef> functionPointer;
    PriorityQueueData Data;
    Data.actorName = actorName;
    Data.key = key;
    Data.UniqueIdentifier = uniqueIdent;
    if (onStartFunc.isFunction())
    {
        componentPointer = currentComponent.second;
        functionPointer = std::make_shared<luabridge::LuaRef>(onStartFunc);
        Data.component = componentPointer;
        Data.function = functionPointer;
        OnStartComponents.push_back(Data);
    }
    if (onUpdateFunc.isFunction())
    {
        componentPointer = currentComponent.second;
        functionPointer = std::make_shared<luabridge::LuaRef>(onUpdateFunc);
        Data.component = componentPointer;
        Data.function = functionPointer;
        OnUpdateComponents.push_back(Data);
    }
    if (onLateUpdate.isFunction())
    {
        componentPointer = currentComponent.second;
        functionPointer = std::make_shared<luabridge::LuaRef>(onLateUpdate);
        Data.component = componentPointer;
        Data.function = functionPointer;
        OnLateUpdateComponents.push_back(Data);
    }
    if (onDestroy.isFunction())
    {
        componentPointer = currentComponent.second;
        functionPointer = std::make_shared<luabridge::LuaRef>(onDestroy);
        Data.component = componentPointer;
        Data.function = functionPointer;
        OnDestroyComponents.push_back(Data);
    }

}
void gameEngine::addOnStart()
{
    for (auto i = 0; i < OnStartComponents.size(); i++)
    {
        luabridge::LuaRef component = *OnStartComponents[i].component;
        if (component["started"] == false)
        {
            if (!component.isInstance<ParticleDB>())
            {
                component["started"] = true;
            }
            functionsToRun.push(OnStartComponents[i]);
        }
    }
}

void gameEngine::addOnUpdate()
{
    for (auto i = 0; i < OnUpdateComponents.size(); i++)
    {
        luabridge::LuaRef component = *OnUpdateComponents[i].component;
        functionsToRun.push(OnUpdateComponents[i]);
    }
}

void gameEngine::addOnLateUpdate()
{
    for (auto i = 0; i < OnLateUpdateComponents.size(); i++)
    {
        luabridge::LuaRef component = *OnLateUpdateComponents[i].component;
        functionsToRun.push(OnLateUpdateComponents[i]);
    }
}

void gameEngine::InjectActorReferences()
{
    for(const auto& actorItr : SceneVector.at(currentScene).ActorsInScene)
    {
        // Going through all of the actors in the scene
        for (auto componentItr = actorItr->ActorComponents.begin(); componentItr != actorItr->ActorComponents.end(); ++componentItr)
        {
            if (componentItr->second.first != "Rigidbody" && componentItr->second.first != "ParticleSystem")
            {
                actorItr->InjectConvienienceReference(componentItr->second.second);
            }
        }
    }

}

Actor* gameEngine::FindActor(const std::string& actorName)
{
    for (auto i = 0; i < pointerToSelf->SceneVector.at(pointerToSelf->currentScene).ActorsInScene.size(); i++)
    {
        if (pointerToSelf->SceneVector.at(pointerToSelf->currentScene).ActorsInScene[i]->name == actorName)
        {
            if (!pointerToSelf->SceneVector.at(pointerToSelf->currentScene).ActorsInScene[i]->destroyed)
            {
                return pointerToSelf->SceneVector.at(pointerToSelf->currentScene).ActorsInScene[i];
            }
            return nullptr;
        }
    }
    for (auto i = 0; i < pointerToSelf->actorsToAdd.size(); i++)
    {
        if (pointerToSelf->actorsToAdd[i].name == actorName)
        {
            if (!pointerToSelf->actorsToAdd[i].destroyed)
            {
                return &pointerToSelf->actorsToAdd[i];
            }
            return nullptr;
        }
    }
    return nullptr;
}

luabridge::LuaRef gameEngine::FindAllActor(const std::string& actorName)
{
    luabridge::LuaRef lua_table = luabridge::newTable(pointerToSelf->Components.currentLuaState);
    int index = 1;
    for (auto i = 0; i < pointerToSelf->SceneVector.at(pointerToSelf->currentScene).ActorsInScene.size(); i++)
    {
        // Iterate through all actors in scene
        if (pointerToSelf->SceneVector.at(pointerToSelf->currentScene).ActorsInScene[i]->name == actorName)
        {
            // Actor has name we are looking for
            if (!pointerToSelf->SceneVector.at(pointerToSelf->currentScene).ActorsInScene[i]->destroyed)
            {
                lua_table[index] = pointerToSelf->SceneVector.at(pointerToSelf->currentScene).ActorsInScene[i];
                index++;
            }
        }
    }
    for (auto i = 0; i < pointerToSelf->actorsToAdd.size(); i++)
    {
        if (pointerToSelf->actorsToAdd[i].name == actorName)
        {
            if (!pointerToSelf->actorsToAdd[i].destroyed)
            {
                lua_table[index] = &pointerToSelf->actorsToAdd[i];
                index++;
            }
        }
    }
    return lua_table;
}

void gameEngine::ReportErrorMessage(const std::string& actorName, const luabridge::LuaException& e)
{
    std::string errorMsg = e.what();

    std::replace(errorMsg.begin(), errorMsg.end(), '\\', '/');

    std::cout << "\033[31m" << actorName << " : " << errorMsg << "\033[0m" << std::endl;
}

void gameEngine::addComponentsToActors()
{
    for (auto& it : SceneVector.at(currentScene).ActorsInScene)
    {
        for (auto& newComponentIt : it->componentsToAddToActor)
        {
            std::pair<std::string, std::shared_ptr<luabridge::LuaRef>> tempPair = std::make_pair("Rigidbody", newComponentIt.component);
            it->ActorComponents.insert({ newComponentIt.key, tempPair });
            typeStruct currentComponentType = { "Rigidbody", newComponentIt.key, newComponentIt.component};
            it->ComponentsByType[currentComponentType.type].push(currentComponentType);
            keyStruct currentComponentKey = {"Rigidbody", newComponentIt.key, newComponentIt.component, it->uniqueIdentifier };
            it->ComponentsByKey[currentComponentKey.key].push(currentComponentKey);
            checkForFunctions(newComponentIt.key, tempPair, it->uniqueIdentifier, it->name);
        }
        it->componentsToAddToActor.clear();
    }
}

Actor* gameEngine::Instantiate(std::string ActorTemplateName)
{
    Actor currentActor;
    EngineUtils engie;
    Rigidbody* newComponent = nullptr;
    ParticleDB* newParticle = nullptr;
    if (pointerToSelf->DefinedTemplates.currentTemplates.find(ActorTemplateName) != pointerToSelf->DefinedTemplates.currentTemplates.end())
    {
        currentActor = *pointerToSelf->DefinedTemplates.currentTemplates.at(ActorTemplateName);
    }
    else
    {
        engie.readTemplate(ActorTemplateName, pointerToSelf);
        currentActor = *pointerToSelf->DefinedTemplates.currentTemplates.at(ActorTemplateName);
    }
    currentActor.ActorComponents.clear();
    currentActor.ComponentsByType.clear();
    currentActor.ComponentsByKey.clear();
    for (auto componentItr = pointerToSelf->DefinedTemplates.currentTemplates.at(ActorTemplateName)->ActorComponents.begin(); componentItr != pointerToSelf->DefinedTemplates.currentTemplates.at(ActorTemplateName)->ActorComponents.end(); ++componentItr)
    {
        if (componentItr->second.first == "Rigidbody")
        {
            if (pointerToSelf->Components.components.find("Rigidbody") == pointerToSelf->Components.components.end())
            {
                // It is a regular Lua component
                Rigidbody temp;
                pointerToSelf->Components.InitializeComponents("Rigidbody");
                pointerToSelf->physics_world = new b2World(pointerToSelf->gravity);
                newContactListener* contactListener = new newContactListener();
                pointerToSelf->physics_world->SetContactListener(contactListener);
            }
            Rigidbody* parentComponent = componentItr->second.second->cast<Rigidbody*>();
            newComponent = new Rigidbody(*parentComponent);
            newComponent->associatedActorUUID = currentActor.uniqueIdentifier;
            newComponent->associatedActor = &currentActor;
            if (!newComponent->hasCollider && !newComponent->has_trigger)
            {
                currentActor.isPhantom = true;
            }
            //pointerToSelf->defineBody(*newComponent);
            //newComponent->defineShapeAndFixture();
            newComponent->Game = pointerToSelf;
            luabridge::LuaRef ref(pointerToSelf->Components.currentLuaState, newComponent);
            std::shared_ptr<luabridge::LuaRef> RigidBodyPointer = std::make_shared<luabridge::LuaRef>(ref);
            std::pair ComponentNameAndLuaRef = std::make_pair("Rigidbody", RigidBodyPointer);
            typeStruct currentComponentType = { "Rigidbody", componentItr->first, RigidBodyPointer };
            currentActor.ComponentsByType["Rigidbody"].push(currentComponentType);
            keyStruct currentComponentKey = { "Rigidbody", componentItr->first, RigidBodyPointer, currentActor.uniqueIdentifier };
            currentActor.ComponentsByKey["Rigidbody"].push(currentComponentKey);
            currentActor.ActorComponents.insert({ componentItr->first, ComponentNameAndLuaRef });
        }
        else if (componentItr->second.first == "ParticleSystem")
        {
            if (pointerToSelf->Components.components.find("ParticleSystem") == pointerToSelf->Components.components.end())
            {
                // It is a regular Lua component
                pointerToSelf->ImageHandler.CreateDefaultParticleTextureWithName("default");
                ParticleDB temp;
                pointerToSelf->Components.InitializeComponents("ParticleSystem");
            }
            ParticleDB* parentComponent = componentItr->second.second->cast<ParticleDB*>();
            newParticle = new ParticleDB(*parentComponent);
            newParticle->pointerToEngine = pointerToSelf;

            luabridge::LuaRef ref(pointerToSelf->Components.currentLuaState, newParticle);
            std::shared_ptr<luabridge::LuaRef> ParticlePointer = std::make_shared<luabridge::LuaRef>(ref);
            std::pair ComponentNameAndLuaRef = std::make_pair("ParticleSystem", ParticlePointer);
            typeStruct currentComponentType = { "ParticleSystem", componentItr->first, ParticlePointer };
            currentActor.ComponentsByType[currentComponentType.type].push(currentComponentType);
            keyStruct currentComponentKey = { "ParticleSystem", componentItr->first, ParticlePointer, currentActor.uniqueIdentifier };
            currentActor.ComponentsByKey[currentComponentKey.type].push(currentComponentKey);
            currentActor.ActorComponents.insert({ componentItr->first, ComponentNameAndLuaRef });
        }
        else
        {
            luabridge::LuaRef newEmptyTable = luabridge::newTable(pointerToSelf->Components.currentLuaState);
            pointerToSelf->Components.EstablishInheritance(newEmptyTable, *componentItr->second.second);
            std::shared_ptr<luabridge::LuaRef> luaTablePointer = std::make_shared<luabridge::LuaRef>(newEmptyTable);
            std::pair ComponentNameAndLuaRef = std::make_pair(componentItr->second.first, luaTablePointer);
            if (currentActor.ActorComponents.find(componentItr->first) != currentActor.ActorComponents.end())
            {
                currentActor.ActorComponents[componentItr->first] = ComponentNameAndLuaRef;
            }
            else
            {
                currentActor.ActorComponents.insert({ componentItr->first, ComponentNameAndLuaRef });
            }
            typeStruct currentComponentType = { componentItr->second.first, componentItr->first, luaTablePointer };
            currentActor.ComponentsByType[currentComponentType.type].push(currentComponentType);
            keyStruct currentComponentKey = { componentItr->second.first, componentItr->first, luaTablePointer, currentActor.uniqueIdentifier };
            currentActor.ComponentsByKey[currentComponentKey.type].push(currentComponentKey);

        }
    }
    currentActor.uniqueIdentifier = currentActor.UUID;
    currentActor.UUID++;		// Set unique identifier of the actor

    pointerToSelf->actorsToAdd.emplace_back(std::move(currentActor));
    return &pointerToSelf->actorsToAdd.back();

}

void gameEngine::addActors()
{
    for (int i = 0; i < actorsToAdd.size(); i++)
    {
        Actor* newActor = new Actor();
        *newActor = actorsToAdd[i];
        SceneVector.at(currentScene).ActorsInScene.push_back(newActor);
        for (const auto& newActorComponents : actorsToAdd[i].ActorComponents)
        {
            if (newActorComponents.second.second->isInstance<Rigidbody>())
            {
                Rigidbody& rb = newActorComponents.second.second->cast<Rigidbody&>();
                rb.associatedActor = newActor;
            }
            checkForFunctions(newActorComponents.first, newActorComponents.second, actorsToAdd[i].uniqueIdentifier, actorsToAdd[i].name);
            //SceneVector.at(currentScene).ActorsInScene.back().InjectConvienienceReference(newActorComponents.second.second);
        }
    }
    actorsToAdd.clear();
}

void gameEngine::Destroy(Actor actorToBeDestroyed)
{
    Actor dummy;
    Actor* currentActor = &dummy;
    for (auto& it : pointerToSelf->SceneVector.at(pointerToSelf->currentScene).ActorsInScene)
    {
        if (it->uniqueIdentifier == actorToBeDestroyed.uniqueIdentifier)
        {
            currentActor = it;
        }
    }
    for (auto& it : pointerToSelf->actorsToAdd)
    {
        if (it.uniqueIdentifier == actorToBeDestroyed.uniqueIdentifier)
        {
            currentActor = &it;
        }
    }
    // We should have the actor now. 
    for (auto& it : currentActor->ActorComponents)
    {
        if (it.second.second->isInstance<ParticleDB>())
        {
            ParticleDB& part = it.second.second->cast<ParticleDB&>();
            part.enabled = false;
            part.removed = true;
        }
        else
        {
            (*it.second.second)["enabled"] = false;
            (*it.second.second)["removed"] = true;
        }

    }
    // We should have the actor now. 

    currentActor->destroyed = true;
    pointerToSelf->actorsToBeDestroyed.push_back(*currentActor);
    for (auto i = 0; i < pointerToSelf->OnDestroyComponents.size(); i++)
    {
        if (pointerToSelf->OnDestroyComponents[i].UniqueIdentifier == currentActor->uniqueIdentifier)
        {
            pointerToSelf->OnDestryfunctionsToRun.push(pointerToSelf->OnDestroyComponents[i]);
        }
    }
}

void gameEngine::DestroyActors()
{
    auto& actorsInScene = pointerToSelf->SceneVector.at(pointerToSelf->currentScene).ActorsInScene;

    for (auto& i : pointerToSelf->actorsToBeDestroyed)
    {
        for (auto it = actorsInScene.begin(); it != actorsInScene.end(); )
        {
            if ((*it)->uniqueIdentifier == i.uniqueIdentifier)
            {
                delete* it;
                it = actorsInScene.erase(it);  // erase returns the next valid iterator
            }
            else
            {
                ++it;
            }
        }
    }

}

void gameEngine::LoadScene(std::string sceneName)
{
    pointerToSelf->switchScene = true;
    pointerToSelf->nextSceneName = sceneName;
    for (auto i : pointerToSelf->SceneVector.at(pointerToSelf->currentScene).ActorsInScene)
    {
        for (auto j = 0; j < pointerToSelf->OnDestroyComponents.size(); j++)
        {
            if (pointerToSelf->OnDestroyComponents[j].UniqueIdentifier == i->uniqueIdentifier)
            {
                pointerToSelf->OnDestryfunctionsToRun.push(pointerToSelf->OnDestroyComponents[j]);
            }
        }
    }
}

std::string gameEngine::GetCurrent()
{
    return pointerToSelf->currentScene;
}

void gameEngine::SwitchScenes()
{
    pointerToSelf->OnStartComponents.clear();
    pointerToSelf->OnUpdateComponents.clear();
    pointerToSelf->OnLateUpdateComponents.clear();
    pointerToSelf->OnDestroyComponents.clear();

    std::vector<Actor*> temp;
    for (auto& actors : pointerToSelf->ActorsToSave)
    {
        temp.push_back(actors);
    }

    pointerToSelf->SceneVector.clear();

    EngineUtils engine;
    std::string path = "resources/scenes/";
    path += nextSceneName;
    path += ".scene";
    if (std::filesystem::exists(path))
    {
        // The file does exit, and there is a path to it
        engine.ReadJsonFile(path, pointerToSelf->currentDoc);
    }
    engine.parseScene(pointerToSelf, pointerToSelf->nextSceneName);
    for (auto& savingActors : temp)
    {
        // For each actor to save
        pointerToSelf->SceneVector.at(pointerToSelf->nextSceneName).ActorsInScene.push_back(savingActors);
    }
    pointerToSelf->InjectActorReferences();
    for (auto i = 0; i < pointerToSelf->SceneVector.at(pointerToSelf->currentScene).ActorsInScene.size(); i++)
    {
        // loop through all of the actors
        for (const auto& j : pointerToSelf->SceneVector.at(pointerToSelf->currentScene).ActorsInScene[i]->ActorComponents)
        {
            // Loop through each actos components
            pointerToSelf->checkForFunctions(j.first, j.second, pointerToSelf->SceneVector.at(pointerToSelf->currentScene).ActorsInScene[i]->uniqueIdentifier, pointerToSelf->SceneVector.at(pointerToSelf->currentScene).ActorsInScene[i]->name);
        }
    }

    pointerToSelf->switchScene = false;
    //pointerToSelf->ActorsToSave.clear();
}

void gameEngine::DontDestroy(Actor actorToBeSaved)
{
    bool save = true;
    Actor* saveActor;
    for (auto& actors : pointerToSelf->SceneVector.at(pointerToSelf->currentScene).ActorsInScene)
    {
        if (actors->UUID == actorToBeSaved.UUID)
        {
            save = false;
            saveActor = actors;
        }
    }
    if (save)
    {
        pointerToSelf->ActorsToSave.push_back(saveActor);
    }

}

void gameEngine::defineBody(Rigidbody& newBody)
{
    b2BodyDef* bodyDef = new b2BodyDef();
    if (newBody.body_type == "dynamic")
    {
        bodyDef->type = b2_dynamicBody;
    }
    else if (newBody.body_type == "kinematic")
    {
        bodyDef->type = b2_kinematicBody;
    }
    else if (newBody.body_type == "static")
    {
        bodyDef->type = b2_staticBody;
    }

    bodyDef->gravityScale = newBody.gravity_scale;
    bodyDef->position = b2Vec2(newBody.x, newBody.y);
    bodyDef->angle = newBody.rotation * (b2_pi / 180.f);
    bodyDef->bullet = newBody.precise;
    bodyDef->angularDamping = newBody.angularFriction;

    newBody.body = physics_world->CreateBody(bodyDef);
}

HitResult* gameEngine::raycast(b2Vec2 pos, b2Vec2 dir, float distance)
{
    if (pointerToSelf->physics_world == nullptr || distance <= 0)
    {
        return nullptr;
    }
    b2Vec2 endpoint = pos + b2Vec2(dir.x * distance, dir.y * distance);
    MyRayCastCallback raycastCallback;
    pointerToSelf->physics_world->RayCast(&raycastCallback, pos, endpoint);
    return raycastCallback.GetClosestHit();
}

struct distanceCalc {
    HitResult* hit;
    float distance;
};

luabridge::LuaRef gameEngine::RaycastAll(b2Vec2 pos, b2Vec2 dir, float distance)
{
    if (pointerToSelf->physics_world == nullptr || distance <= 0)
    {
        return luabridge::LuaRef(pointerToSelf->Components.currentLuaState);
    }
    b2Vec2 endpoint = pos + b2Vec2(dir.x * distance, dir.y * distance);
    MyRayCastCallback raycastCallback;
    pointerToSelf->physics_world->RayCast(&raycastCallback, pos, endpoint);

    std::vector<HitResult*> allHits = raycastCallback.GetAllHits();
    std::vector<distanceCalc> newHits;
    // Put all the hits in another vector with the distance calculated
    for (auto i = 0; i < allHits.size(); i++)
    {
        float distance = (allHits[i]->point - pos).Length();
        distanceCalc newHit = { allHits[i], distance };
        newHits.push_back(newHit);
    }

    std::stable_sort(newHits.begin(), newHits.end(), [](const distanceCalc& a, const distanceCalc& b) {
        return a.distance < b.distance; });


    luabridge::LuaRef lua_table = luabridge::newTable(pointerToSelf->Components.currentLuaState);
    int index = 1;
    for (auto i = 0; i < allHits.size(); i++)
    {
        lua_table[index] = newHits[i].hit;
        index++;
    }

    return lua_table;

}

void gameEngine::clearOnDestryFunctionPQ()
{
    while (!OnDestryfunctionsToRun.empty())
    {
        auto& currentPair = OnDestryfunctionsToRun.top();
        luabridge::LuaRef component = *currentPair.component;
        luabridge::LuaRef luaFunction = *currentPair.function;
        if (component.isInstance<Rigidbody>())
        {
            Rigidbody& rb = component.cast<Rigidbody&>();
            rb.removed = true;
            rb.OnDestroy();
        }
        else if(component.isInstance<ParticleDB>())
        {
            ParticleDB& part = component.cast<ParticleDB&>();
            part.removed = true;
        }
        else
        {
            try {
                luaFunction(component);
            }
            catch (const luabridge::LuaException& e) {
                ReportErrorMessage(currentPair.actorName, e);
                // need to implement this function 
            }
        }
        OnDestryfunctionsToRun.pop();
    }
}