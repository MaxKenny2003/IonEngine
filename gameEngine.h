// gameEngine.h
#pragma once
#ifndef GAMEENGINE_H
#define GAMEENGINE_H
#define GLM_ENABLE_EXPERIMENTAL

#include <string>
#include <unordered_map>
#include "ThirdParty/glm/glm.hpp"
#include "ThirdParty/glm/gtx/hash.hpp"
#include "ThirdParty/rapidjson/include/rapidjson/document.h"
#include "ThirdParty/rapidjson/include/rapidjson/filereadstream.h"
#include "SceneDB.h"
#include "TemplateDB.h"
#include "ImageDB.h"
#include "AudioDB.h"
#include "ThirdParty/SDL2/SDL.h"
#include "ThirdParty/SDL2_image/SDL_image.h"
#include <queue>
#include "lua.hpp"
#include "ThirdParty/LuaBridge/LuaBridge.h"
#include "ComponentDB.h"
#include <utility>
#include "Actor.h"
#include <vector>
#include "include/box2d/box2d.h"
#include "Rigidbody.h"
#include <memory>
#include "ParticleDB.h"



struct PriorityQueueData {
    int UniqueIdentifier;
    std::string key; 
    std::shared_ptr<luabridge::LuaRef> component;
    std::shared_ptr<luabridge::LuaRef> function;
    std::string actorName;
};

struct HitResult {
    Actor* actor;
    b2Vec2 point;
    b2Vec2 normal;
    bool isTrigger;
};


class SceneDB;

class TemplateDB;

class ImageDB;

class ComponentDB;

class Rigidbody;


class comparator {
public:
    bool operator()(const PriorityQueueData& lhs, const PriorityQueueData& rhs) {
        if (lhs.UniqueIdentifier != rhs.UniqueIdentifier)
        {
            return lhs.UniqueIdentifier > rhs.UniqueIdentifier;
        }
        return lhs.key > rhs.key; // Compare based on the first element of the pair (priority)
    };
    // needs to be ordered by actor ID, then ordered by alphabetical
};

class gameEngine
{
public:
    bool isRunning = true;

    rapidjson::Document currentDoc;
    rapidjson::Document currentTempDoc;
    std::string currentScene;

    std::unordered_map<std::string, SceneDB> SceneVector;

    TemplateDB DefinedTemplates;

    float WindowXResolution = 640.0f;
    float WindowYResolution = 360.0f;
    std::string gameTitle = "";

    int ClearColorRed = 255;
    int ClearColorGreen = 255;
    int ClearColorBlue = 255;

    ImageDB ImageHandler;

    AudioDB AudioHandler;

    float cameraOffsetX = 0.0f;
    float cameraOffsetY = 0.0f;
    float cameraZoom = 1.0f;

    // Homework 6 Variables
    glm::vec2 cameraLocation = glm::vec2(0.0f, 0.0f);

    // Homework 7 Variables
    ComponentDB Components;
    //component, function
    std::priority_queue<PriorityQueueData, std::vector<PriorityQueueData>, comparator> functionsToRun;
    std::priority_queue<PriorityQueueData, std::vector<PriorityQueueData>, comparator> OnDestryfunctionsToRun;
    bool isOnStart = true;
    static gameEngine* pointerToSelf;
    gameEngine() {
        pointerToSelf = this;  // Initialize pointerToSelf in the constructor
    }
    static inline int numTimesAddComponent = 0;

    glm::vec2 CameraPosition = glm::vec2(0.0f, 0.0f);


    // Homework 7 Functions
    //void findFunctionsToRun();

    float cam_ease_factor = 1.0f;

    void getGameMessages(gameEngine* Game);  // Function declaration, no body here

    void getCameraSize(gameEngine* Game);

    void getScene(gameEngine* Game);


    static std::string obtain_word_after_phrase(const std::string& input, const std::string& phrase);

    void getDocument(gameEngine* Game, std::string path);

    void clearFunctionPQ();
    void clearOnDestryFunctionPQ();

    void InjectActorReferences();

    void checkForFunctions(std::string key, std::pair<std::string, std::shared_ptr<luabridge::LuaRef>> currentComponent, int uniqueIdent, std::string actorName);

    std::vector<PriorityQueueData> OnStartComponents;
    std::vector<PriorityQueueData> OnUpdateComponents;
    std::vector<PriorityQueueData> OnLateUpdateComponents;
    std::vector<PriorityQueueData> OnDestroyComponents;


    void addOnStart();
    void addOnUpdate();
    void addOnLateUpdate();
    void addOnDestroy();

    static Actor* FindActor(const std::string& actorName);
    static luabridge::LuaRef FindAllActor(const std::string& actorName);
    static Actor* Instantiate(std::string ActorTemplateName);
    static void Destroy(Actor actorToBeDestroyed);
    void ReportErrorMessage(const std::string& actorName, const luabridge::LuaException& e);

    void addComponentsToActors();

    std::vector<Actor> actorsToAdd;
    std::vector<Actor> actorsToBeDestroyed;
    void addActors();
    void DestroyActors();

    // Stuff for scene switching
    bool switchScene = false;
    int uniqueIdentifierStart = 0;
    std::string nextSceneName = "";
    static void LoadScene(std::string sceneName);
    static std::string GetCurrent();
    void SwitchScenes();
    
    std::vector<Actor*> ActorsToSave;
    static void DontDestroy(Actor actorToBeSaved);


    // Time for Homework8 Stuff :) 
    b2Vec2 gravity = b2Vec2(0.0f, 9.8f);

    b2World* physics_world = nullptr;

    void defineBody(Rigidbody& newBody);

    static HitResult* raycast(b2Vec2 pos, b2Vec2 dir, float distance);
    static luabridge::LuaRef RaycastAll(b2Vec2 pos, b2Vec2 dir, float distance);

    //newContactListener* contactListener = new newContactListener();
    
};

class newContactListener : public b2ContactListener
{
public:
    void BeginContact(b2Contact* contact) override
    {
        b2Fixture* FixtureA = contact->GetFixtureA();
        b2Fixture* FixtureB = contact->GetFixtureB();
        collisionComponent collisionA;
        collisionComponent collisionB;
        // Get the other Actor
        collisionA.other = reinterpret_cast<Actor*>(FixtureB->GetUserData().pointer);
        collisionB.other = reinterpret_cast<Actor*>(FixtureA->GetUserData().pointer);
        // Set the other properties
        b2WorldManifold worldManifold;
        contact->GetWorldManifold(&worldManifold);
        collisionA.point = worldManifold.points[0];
        collisionB.point = worldManifold.points[0];
        // Get the relative Velocity
        b2Vec2 relative_velocity = FixtureA->GetBody()->GetLinearVelocity() - FixtureB->GetBody()->GetLinearVelocity();
        collisionA.relativeVelocity = relative_velocity;
        collisionB.relativeVelocity = relative_velocity;
        // Get the normals
        collisionA.normal = worldManifold.normal;
        collisionB.normal = worldManifold.normal;

        for (auto& currentComponentFromA : collisionB.other->ComponentsByKey)
        {
            // Get the top component from the priority queue
            luabridge::LuaRef component = *currentComponentFromA.second.top().component;

            // Assuming 'component' is a LuaRef or an object that can be accessed like a Lua table
            if (!FixtureA->IsSensor())
            {
                luabridge::LuaRef onCollisionEnter = (component)["OnCollisionEnter"];

                if (onCollisionEnter.isFunction())
                {
                    std::shared_ptr<luabridge::LuaRef> componentPointer = currentComponentFromA.second.top().component;
                    std::shared_ptr<luabridge::LuaRef> functionPointer = std::make_shared<luabridge::LuaRef>(onCollisionEnter);
                    collisionData data = { collisionA, componentPointer, functionPointer };
                    collisionB.other->OnCollisionEventQueue.push_back(data);
                }
            }
            else
            {
                luabridge::LuaRef onCollisionEnter = (component)["OnTriggerEnter"];

                if (onCollisionEnter.isFunction())
                {
                    collisionA.point = b2Vec2(-999.0f, -999.0f);
                    collisionA.normal = b2Vec2(-999.0f, -999.0f);
                    std::shared_ptr<luabridge::LuaRef> componentPointer = currentComponentFromA.second.top().component;
                    std::shared_ptr<luabridge::LuaRef> functionPointer = std::make_shared<luabridge::LuaRef>(onCollisionEnter);
                    collisionData data = { collisionA, componentPointer, functionPointer };
                    collisionB.other->OnCollisionEventQueue.push_back(data);
                }
            }


        }
        // It is now time to check if there are components that have on Collision enter
        for (auto& currentComponentFromB : collisionA.other->ComponentsByKey)
        {
            // Get the top component from the priority queue
            luabridge::LuaRef component = *currentComponentFromB.second.top().component;

            // Assuming 'component' is a LuaRef or an object that can be accessed like a Lua table
            if (!FixtureA->IsSensor())
            {
                luabridge::LuaRef onCollisionEnter = (component)["OnCollisionEnter"];

                if (onCollisionEnter.isFunction())
                {
                    std::shared_ptr<luabridge::LuaRef> componentPointer = currentComponentFromB.second.top().component;
                    std::shared_ptr<luabridge::LuaRef> functionPointer = std::make_shared<luabridge::LuaRef>(onCollisionEnter);
                    collisionData data = { collisionB, componentPointer, functionPointer };
                    collisionA.other->OnCollisionEventQueue.push_back(data);
                }
            }
            else
            {
                luabridge::LuaRef onCollisionEnter = (component)["OnTriggerEnter"];

                if (onCollisionEnter.isFunction())
                {
                    collisionB.point = b2Vec2(-999.0f, -999.0f);
                    collisionB.normal = b2Vec2(-999.0f, -999.0f);
                    std::shared_ptr<luabridge::LuaRef> componentPointer = currentComponentFromB.second.top().component;
                    std::shared_ptr<luabridge::LuaRef> functionPointer = std::make_shared<luabridge::LuaRef>(onCollisionEnter);
                    collisionData data = { collisionB, componentPointer, functionPointer };
                    collisionA.other->OnCollisionEventQueue.push_back(data);
                }
            }

        }

        while (!collisionB.other->OnCollisionEventQueue.empty())
        {
            luabridge::LuaRef component = *collisionB.other->OnCollisionEventQueue.front().component;
            luabridge::LuaRef luaFunction = *collisionB.other->OnCollisionEventQueue.front().function;
            collisionComponent Data = collisionB.other->OnCollisionEventQueue.front().componentData;
            luaFunction(component, Data);
            collisionB.other->OnCollisionEventQueue.pop_front();
        }
        while (!collisionA.other->OnCollisionEventQueue.empty())
        {
            luabridge::LuaRef component = *collisionA.other->OnCollisionEventQueue.front().component;
            luabridge::LuaRef luaFunction = *collisionA.other->OnCollisionEventQueue.front().function;
            collisionComponent Data = collisionA.other->OnCollisionEventQueue.front().componentData;
            luaFunction(component, Data);
            collisionA.other->OnCollisionEventQueue.pop_front();
        }

    }
    void EndContact(b2Contact* contact) override
    {
        b2Fixture* FixtureA = contact->GetFixtureA();
        b2Fixture* FixtureB = contact->GetFixtureB();
        collisionComponent collisionA;
        collisionComponent collisionB;
        // Get the other Actor
        collisionA.other = reinterpret_cast<Actor*>(FixtureB->GetUserData().pointer);
        collisionB.other = reinterpret_cast<Actor*>(FixtureA->GetUserData().pointer);
        // Set the other properties
        collisionA.point = b2Vec2(-999.0f, -999.0f);
        collisionB.point = b2Vec2(-999.0f, -999.0f);
        // Get the relative Velocity
        b2Vec2 relative_velocity = FixtureA->GetBody()->GetLinearVelocity() - FixtureB->GetBody()->GetLinearVelocity();
        collisionA.relativeVelocity = relative_velocity;
        collisionB.relativeVelocity = relative_velocity;
        // Get the normals
        collisionA.normal = b2Vec2(-999.0f, -999.0f);
        collisionB.normal = b2Vec2(-999.0f, -999.0f);

        
        // It is now time to check if there are components that have on Collision enter
        for (auto& currentComponentFromA : collisionB.other->ComponentsByKey)
        {
            // Get the top component from the priority queue
            luabridge::LuaRef component = *currentComponentFromA.second.top().component;

            // Assuming 'component' is a LuaRef or an object that can be accessed like a Lua table
            if (!FixtureA->IsSensor())
            {
                luabridge::LuaRef onCollisionEnter = (component)["OnCollisionExit"];

                if (onCollisionEnter.isFunction())
                {
                    std::shared_ptr<luabridge::LuaRef> componentPointer = currentComponentFromA.second.top().component;
                    std::shared_ptr<luabridge::LuaRef> functionPointer = std::make_shared<luabridge::LuaRef>(onCollisionEnter);
                    collisionData data = { collisionA, componentPointer, functionPointer };
                    collisionB.other->OnCollisionExitEventQueue.push_back(data);
                }
            }
            else
            {
                luabridge::LuaRef onCollisionEnter = (component)["OnTriggerExit"];

                if (onCollisionEnter.isFunction())
                {
                    std::shared_ptr<luabridge::LuaRef> componentPointer = currentComponentFromA.second.top().component;
                    std::shared_ptr<luabridge::LuaRef> functionPointer = std::make_shared<luabridge::LuaRef>(onCollisionEnter);
                    collisionData data = { collisionA, componentPointer, functionPointer };
                    collisionB.other->OnCollisionExitEventQueue.push_back(data);
                }
            }

        }
        // It is now time to check if there are components that have on Collision enter
        for (auto& currentComponentFromB : collisionA.other->ComponentsByKey)
        {
            // Get the top component from the priority queue
            luabridge::LuaRef component = *currentComponentFromB.second.top().component;

            // Assuming 'component' is a LuaRef or an object that can be accessed like a Lua table
            if (!FixtureA->IsSensor())
            {
                luabridge::LuaRef onCollisionEnter = (component)["OnCollisionExit"];

                if (onCollisionEnter.isFunction())
                {
                    std::shared_ptr<luabridge::LuaRef> componentPointer = currentComponentFromB.second.top().component;
                    std::shared_ptr<luabridge::LuaRef> functionPointer = std::make_shared<luabridge::LuaRef>(onCollisionEnter);
                    collisionData data = { collisionB, componentPointer, functionPointer };
                    collisionA.other->OnCollisionExitEventQueue.push_back(data);
                }
            }
            else
            {
                luabridge::LuaRef onCollisionEnter = (component)["OnTriggerExit"];

                if (onCollisionEnter.isFunction())
                {
                    std::shared_ptr<luabridge::LuaRef> componentPointer = currentComponentFromB.second.top().component;
                    std::shared_ptr<luabridge::LuaRef> functionPointer = std::make_shared<luabridge::LuaRef>(onCollisionEnter);
                    collisionData data = { collisionB, componentPointer, functionPointer };
                    collisionA.other->OnCollisionExitEventQueue.push_back(data);
                }
            }

        }

        while (!collisionB.other->OnCollisionExitEventQueue.empty())
        {
            luabridge::LuaRef component = *collisionB.other->OnCollisionExitEventQueue.front().component;
            luabridge::LuaRef luaFunction = *collisionB.other->OnCollisionExitEventQueue.front().function;
            collisionComponent Data = collisionB.other->OnCollisionExitEventQueue.front().componentData;
            luaFunction(component, Data);
            collisionB.other->OnCollisionExitEventQueue.pop_front();
        }
        while (!collisionA.other->OnCollisionExitEventQueue.empty())
        {
            luabridge::LuaRef component = *collisionA.other->OnCollisionExitEventQueue.front().component;
            luabridge::LuaRef luaFunction = *collisionA.other->OnCollisionExitEventQueue.front().function;
            collisionComponent Data = collisionA.other->OnCollisionExitEventQueue.front().componentData;
            luaFunction(component, Data);
            collisionA.other->OnCollisionExitEventQueue.pop_front();
        }
    }

    // call lifecycle function on A then B. In these functions, if one of them is triggered, we know that there are two objects. 
    // When this collision occurs, we need to go through the actors and find if either of them have any OnCollisionEnter or Exit commands
    // If they do, we add them to a deque to be run. I believe that (and dob't quote me) that the functions should be run right as it is occurs

};

class MyRayCastCallback : public b2RayCastCallback {
public:
    // Constructor to initialize the results container
    MyRayCastCallback() : closestHit(nullptr), allHits() {}

    // This function will be called for every fixture hit by the ray
    float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction) override {
        // Skip phantom (sensor) fixtures

        Actor* currentActor = reinterpret_cast<Actor*>(fixture->GetUserData().pointer);

        // Get the body (actor) associated with the fixture
        b2Body* actor = fixture->GetBody();

        if (currentActor->isPhantom) {
            return 1.0f;  // Continue with the next fixture
        }

        // Create a new HitResult
        HitResult* hit = new HitResult;
        hit->actor = currentActor;
        hit->point = point;
        hit->normal = normal;
        hit->isTrigger = fixture->IsSensor();

        // Store the closest hit (for Raycast)
        if (!closestHit || fraction < closestHitFraction) {
            closestHit = hit;
            closestHitFraction = fraction;
        }

        // Store all hits (for RaycastAll)
        allHits.push_back(hit);

        return 1.0f;  // Continue to the next fixture
    }

    // Getter for closest hit (Raycast)
    HitResult* GetClosestHit() {
        return closestHit;
    }

    // Getter for all hits (RaycastAll)
    const std::vector<HitResult*> GetAllHits() {
        return allHits;
    }

private:
    HitResult* closestHit;  // The closest hit result (Raycast)
    float closestHitFraction;  // The fraction of the closest hit along the ray
    std::vector<HitResult*> allHits;  // All hit results (RaycastAll)
};


#endif
