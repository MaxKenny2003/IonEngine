#pragma once

#ifndef RIGIDBODY_H
#define RIGIDBODY_H
#define GLM_ENABLE_EXPERIMENTAL

#include "include/box2d/box2d.h"
#include "ThirdParty/glm/glm.hpp"
#include "ThirdParty/glm/gtx/hash.hpp"
#include <string>
#include "Actor.h"
#include "gameEngine.h"
#include "lua.hpp"
#include "ThirdParty/LuaBridge/LuaBridge.h"
#include <utility>


class Rigidbody
{
public:

	Rigidbody() = default;

	// Stuff that I need for ease of coding
	gameEngine* Game = nullptr;
	int associatedActorUUID = -1;
	Actor* associatedActor = nullptr;

	float x = 0.0f;
	float y = 0.0f;
	std::string body_type = "dynamic";
	bool precise = true;
	float gravity_scale = 1.0f;
	float density = 1.0f;
	float angularFriction = 0.3f;
	float rotation = 0.0f;          // This value will ALWAYS be in radians. If needed for lua, convert it to degrees rads * (180.0f / b2_pi)
	bool hasCollider = true;
	bool has_trigger = true;
	b2Body* body = nullptr;

	std::string colliderType = "box";
	float width = 1.0f;
	float height = 1.0f;
	float radius = 0.5f;
	float friction = 0.3f;
	float bounciness = 0.3f;

	std::string triggerType = "box";
	float triggerWidth = 1.0f;
	float triggerHeight = 1.0f;
	float triggerRadius = 0.5f;

	const uint16 SensorCategory = 0b0001;
	const uint16 ColliderCategory = 0b0010;

	bool started = false;

	bool removed = false;


	b2Vec2 GetPosition();
	float GetRotation();
	void defineShapeAndFixture();

	void addForce(b2Vec2 force);
	void setVelocity(b2Vec2 velocity);
	void setPosition(b2Vec2 position);
	void setRotation(float newRotation);
	void setAngularVelocity(float degreesClockwise);
	void setGravityScale(float newGravityScale);
	void setUpDirection(b2Vec2 direction);
	void setRightDirection(b2Vec2 direction);
	b2Vec2 getVelocity();
	float getAngularVelocity();
	float getGravityScale();
	b2Vec2 getUpDirection();
	b2Vec2 getRightDirection();

	void OnDestroy();
	void OnStart();

};





#endif

