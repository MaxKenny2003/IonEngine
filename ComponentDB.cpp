#include "ComponentDB.h"
#include "SceneDB.h"
#include "Helper.h"
#include "Actor.h"
#include "Input.h"
#include "gameEngine.h"
#include "ImageDB.h"
#include <string>
#include "include/box2d/box2d.h"
#include "Rigidbody.h"
#include "ParticleDB.h"


void ComponentDB::Initialize()
{
    InitializeStates();
    InitializeFunctions();
    //InitializeComponents();

}

void ComponentDB::InitializeStates()
{
    currentLuaState = luaL_newstate();
    luaL_openlibs(currentLuaState);
}

void ComponentDB::InitializeFunctions()
{
    luabridge::getGlobalNamespace(currentLuaState)
        .beginNamespace("Debug")
        .addFunction("Log", ComponentDB::CppLog)
        .endNamespace();
     
    luabridge::getGlobalNamespace(currentLuaState)
        .beginClass<Actor>("Actor")
        .addFunction("GetName", &Actor::GetName)
        .addFunction("GetID", &Actor::GetID)
        .addFunction("GetComponentByKey", &Actor::GetComponentByKey)
        .addFunction("GetComponent", &Actor::GetComponent)
        .addFunction("GetComponents", &Actor::GetComponents)
        .addFunction("AddComponent", &Actor::AddComponent)
        .addFunction("RemoveComponent", &Actor::RemoveComponent)
        .endClass();

    luabridge::getGlobalNamespace(currentLuaState)
        .beginNamespace("Actor")
        .addFunction("Find", &gameEngine::FindActor)
        .addFunction("FindAll", &gameEngine::FindAllActor)
        .addFunction("Instantiate", &gameEngine::Instantiate)
        .addFunction("Destroy", &gameEngine::Destroy)
        .endNamespace();

    luabridge::getGlobalNamespace(currentLuaState)
        .beginNamespace("Application")
        .addFunction("Quit", ComponentDB::CppQuit)
        .addFunction("Sleep", ComponentDB::CppSleep)
        .addFunction("OpenURL", ComponentDB::CppOpenURL)
        .addFunction("GetFrame", ComponentDB::GetFrame)
        .endNamespace();

    luabridge::getGlobalNamespace(currentLuaState)
        .beginNamespace("Input")
        .addFunction("GetKey", &Input::LuaGetKey)
        .addFunction("GetKeyDown", &Input::LuaGetKeyDown)
        .addFunction("GetKeyUp", &Input::LuaGetKeyUp)
        .addFunction("GetMousePosition", &Input::GetMousePosition)
        .addFunction("GetMouseButton", &Input::GetMouseButton)
        .addFunction("GetMouseButtonDown", &Input::GetMouseButtonDown)
        .addFunction("GetMouseButtonUp", &Input::GetMouseButtonUp)
        .addFunction("GetMouseScrollDelta", &Input::GetMouseScrollDelta)
        .addFunction("HideCursor", &Input::HideCursor)
        .addFunction("ShowCursor", &Input::ShowCursor)
        .endNamespace();

    luabridge::getGlobalNamespace(currentLuaState)
        .beginClass<glm::vec2>("vec2")
        .addProperty("x", &glm::vec2::x)
        .addProperty("y", &glm::vec2::y)
        .endClass();

    luabridge::getGlobalNamespace(currentLuaState)
        .beginNamespace("Text")
        .addFunction("Draw", &ImageDB::Draw)
        .endNamespace();

    luabridge::getGlobalNamespace(currentLuaState)
        .beginNamespace("Audio")
        .addFunction("Play", &AudioDB::Play)
        .addFunction("Halt", &AudioDB::Halt)
        .addFunction("SetVolume", &AudioDB::SetVolume)
        .endNamespace();

    luabridge::getGlobalNamespace(currentLuaState)
        .beginNamespace("Image")
        .addFunction("DrawUI", &ImageDB::DrawUI)
        .addFunction("DrawUIEx", &ImageDB::DrawUIEx)
        .addFunction("Draw", &ImageDB::DrawImage)
        .addFunction("DrawEx", &ImageDB::DrawImageEx)
        .addFunction("DrawPixel", &ImageDB::DrawPixel)
        .endNamespace();

    luabridge::getGlobalNamespace(currentLuaState)
        .beginNamespace("Camera")
        .addFunction("SetPosition", &ImageDB::SetPosition)
        .addFunction("GetPositionX", &ImageDB::GetPositionX)
        .addFunction("GetPositionY", &ImageDB::GetPositionY)
        .addFunction("SetZoom", &ImageDB::SetZoom)
        .addFunction("GetZoom", &ImageDB::GetZoom)
        .endNamespace();

    luabridge::getGlobalNamespace(currentLuaState)
        .beginNamespace("Scene")
        .addFunction("Load", &gameEngine::LoadScene)
        .addFunction("GetCurrent", &gameEngine::GetCurrent)
        .addFunction("DontDestroy", &gameEngine::DontDestroy)
        .endNamespace();

    luabridge::getGlobalNamespace(currentLuaState)
        .beginClass<Rigidbody>("Rigidbody")
        .addConstructor<void (*)()>() 
        .addProperty("x", &Rigidbody::x)
        .addProperty("y", &Rigidbody::y)
        .addProperty("body_type", &Rigidbody::body_type)
        .addProperty("precise", &Rigidbody::precise)
        .addProperty("gravity_scale", &Rigidbody::gravity_scale)
        .addProperty("density", &Rigidbody::density)
        .addProperty("angularFriction", &Rigidbody::angularFriction)
        .addProperty("rotation", &Rigidbody::rotation)
        .addProperty("hasCollider", &Rigidbody::hasCollider)
        .addProperty("has_trigger", &Rigidbody::has_trigger)
        .addProperty("started", &Rigidbody::started)
        .addFunction("GetPosition", &Rigidbody::GetPosition)
        .addFunction("GetRotation", &Rigidbody::GetRotation)
        .addFunction("AddForce", &Rigidbody::addForce)
        .addFunction("SetVelocity", &Rigidbody::setVelocity)
        .addFunction("SetPosition", &Rigidbody::setPosition)
        .addFunction("SetRotation", &Rigidbody::setRotation)
        .addFunction("SetAngularVelocity", &Rigidbody::setAngularVelocity)
        .addFunction("SetGravityScale", &Rigidbody::setGravityScale)
        .addFunction("SetUpDirection", &Rigidbody::setUpDirection)
        .addFunction("SetRightDirection", &Rigidbody::setRightDirection)
        .addFunction("GetVelocity", &Rigidbody::getVelocity)
        .addFunction("GetAngularVelocity", &Rigidbody::getAngularVelocity)
        .addFunction("GetGravityScale", &Rigidbody::getGravityScale)
        .addFunction("GetUpDirection", &Rigidbody::getUpDirection)
        .addFunction("GetRightDirection", &Rigidbody::getRightDirection)
        .addFunction("OnDestroy", &Rigidbody::OnDestroy)
        .addFunction("OnStart", &Rigidbody::OnStart)
        .endClass();

    luabridge::getGlobalNamespace(currentLuaState)
        .beginClass<b2Vec2>("Vector2")
        .addConstructor<void(*) (float, float)>()
        .addData("x", &b2Vec2::x)
        .addData("y", &b2Vec2::y)
        .addFunction("Normalize", &b2Vec2::Normalize)
        .addFunction("Length", &b2Vec2::Length)
        .addFunction("__add", &b2Vec2::operator_add)
        .addFunction("__sub", &b2Vec2::operator_sub)
        .addFunction("__mul", &b2Vec2::operator_mul)
        .addStaticFunction("Dot", static_cast<float (*)(const b2Vec2&, const b2Vec2&)>(&b2Dot))
        .addStaticFunction("Distance", static_cast<float (*)(const b2Vec2&, const b2Vec2&)>(&b2Distance))
        .endClass();

    luabridge::getGlobalNamespace(currentLuaState)
        .beginClass<collisionComponent>("collision")
        .addData("other", &collisionComponent::other)                     // Make sure Actor* is also registered
        .addData("point", &collisionComponent::point)
        .addData("relative_velocity", &collisionComponent::relativeVelocity)
        .addData("normal", &collisionComponent::normal)
        .endClass();

    luabridge::getGlobalNamespace(currentLuaState)
        .beginClass<HitResult>("HitResult")
        .addData("actor", &HitResult::actor)
        .addData("point", &HitResult::point)
        .addData("normal", &HitResult::normal)
        .addData("is_trigger", &HitResult::isTrigger)
        .endClass();

    luabridge::getGlobalNamespace(currentLuaState)
        .beginNamespace("Physics")
        .addFunction("Raycast", &gameEngine::raycast)
        .addFunction("RaycastAll", &gameEngine::RaycastAll)
        .endNamespace();

    luabridge::getGlobalNamespace(currentLuaState)
        .beginClass<ParticleDB>("ParticleSystem")
        .addConstructor<void (*)()>()
        .addProperty("enabled", &ParticleDB::enabled)
        .addProperty("started", &ParticleDB::started)
        .addProperty("x", &ParticleDB::startingX)
        .addProperty("y", &ParticleDB::startingY)
        .addProperty("emit_angle_min", &ParticleDB::emit_angle_min)
        .addProperty("emit_angle_max", &ParticleDB::emit_angle_max)
        .addProperty("emit_radius_min", &ParticleDB::emit_radius_min)
        .addProperty("emit_radius_max", &ParticleDB::emit_radius_max)
        .addProperty("sorting_order", &ParticleDB::sortingOrder)
        .addProperty("r", &ParticleDB::r)
        .addProperty("g", &ParticleDB::g)
        .addProperty("b", &ParticleDB::b)
        .addProperty("a", &ParticleDB::a)
        .addProperty("frames_between_bursts", &ParticleDB::frames_between_bursts)
        .addProperty("burst_quantity", &ParticleDB::burst_quantity)
        .addProperty("start_scale_min", &ParticleDB::start_scale_min)
        .addProperty("start_scale_max", &ParticleDB::start_scale_max)
        .addProperty("rotation_min", &ParticleDB::rotation_min)
        .addProperty("rotation_max", &ParticleDB::rotation_max)
        .addProperty("duration_frames", &ParticleDB::duration_frames)
        .addProperty("start_speed_min", &ParticleDB::start_speed_min)
        .addProperty("start_speed_max", &ParticleDB::start_speed_max)
        .addProperty("rotation_speed_min", &ParticleDB::rotation_speed_min)
        .addProperty("rotation_speed_max", &ParticleDB::rotation_speed_max)
        .addProperty("gravity_scale_x", &ParticleDB::gravity_scale_x)
        .addProperty("gravity_scale_y", &ParticleDB::gravity_scale_y)
        .addProperty("drag_factor", &ParticleDB::drag_factor)
        .addProperty("angular_drag_factor", &ParticleDB::angular_drag_factor)
        .addProperty("end_scale", &ParticleDB::end_scale)
        .addProperty("end_color_r", &ParticleDB::end_color_r)
        .addProperty("end_color_g", &ParticleDB::end_color_g)
        .addProperty("end_color_b", &ParticleDB::end_color_b)
        .addProperty("end_color_a", &ParticleDB::end_color_a)
        .addFunction("OnStart", &ParticleDB::OnStart)
        .addFunction("OnUpdate", &ParticleDB::OnUpdate)
        .addFunction("Stop", &ParticleDB::Stop)
        .addFunction("Play", &ParticleDB::Play)
        .addFunction("Burst", &ParticleDB::Burst)
        .endClass();
}

void ComponentDB::InitializeComponents(std::string componentName)
{
    if (components.find(componentName) == components.end())
    {
        if (componentName == "Rigidbody")
        {
            Rigidbody* temp = new Rigidbody();
            luabridge::LuaRef ref(currentLuaState, temp);
            std::shared_ptr<luabridge::LuaRef> RigidBodyPointer = std::make_shared<luabridge::LuaRef>(ref);
            components.insert({ componentName, RigidBodyPointer });
        }
        else if (componentName == "ParticleSystem")
        {
            ParticleDB* temp = new ParticleDB();
            luabridge::LuaRef ref(currentLuaState, temp);
            std::shared_ptr<luabridge::LuaRef> ParticlePointer = std::make_shared<luabridge::LuaRef>(ref);
            components.insert({ componentName, ParticlePointer });
        }
        else
        {
            std::string path = "resources/component_types/";
            std::string filetype = ".lua";
            if (luaL_dofile(currentLuaState, (path + componentName + filetype).c_str()) != LUA_OK)
            {
                std::cout << "problem with lua file " << componentName;
                exit(0);
            }
            luabridge::LuaRef component = luabridge::getGlobal(currentLuaState, componentName.c_str());
            component["enabled"] = true;
            component["started"] = false;
            component["removed"] = false;
            components.insert({ componentName, std::make_shared<luabridge::LuaRef>(component) });
        }
    }
}

void ComponentDB::EstablishInheritance(luabridge::LuaRef& instanceTable, luabridge::LuaRef& parentTable)
{
    luabridge::LuaRef new_metatable = luabridge::newTable(currentLuaState);
    new_metatable["__index"] = parentTable;

    instanceTable.push(currentLuaState);
    new_metatable.push(currentLuaState);
    lua_setmetatable(currentLuaState, -2);
    lua_pop(currentLuaState, 1);
}

void ComponentDB::CppLog(const std::string& message)
{
    std::cout << message << std::endl;
}

void ComponentDB::CppQuit()
{
    exit(0);
}

void ComponentDB::CppSleep(const int& milliseconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

void ComponentDB::CppOpenURL(const std::string& URL)
{
    std::string command;
    #if defined(_WIN32) 
        command = "start " + URL;
    #elif defined(__APPLE__)
        command = "open " + URL;
    #elif defined(__linux__)
        command = "xdg-open " + URL;
    #endif

        system(command.c_str());
}

int ComponentDB::GetFrame()
{
    return Helper::GetFrameNumber();

}

