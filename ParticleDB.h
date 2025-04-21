#pragma once

#ifndef PARTICLEDB_H
#define PARTICLEDB_H
#define GLM_ENABLE_EXPERIMENTAL

#include "include/box2d/box2d.h"
#include "ThirdParty/glm/glm.hpp"
#include "ThirdParty/glm/gtx/hash.hpp"
#include <string>
#include <vector>
#include "Actor.h"
#include "gameEngine.h"
#include "lua.hpp"
#include "ThirdParty/LuaBridge/LuaBridge.h"
#include <utility>
#include "Helper.h"
#include <deque>

class ParticleDB
{

public:
	gameEngine* pointerToEngine = nullptr;

	bool started = false;

	std::string textureName = "default";
	glm::vec2 pivotPoint = glm::vec2(0.5f, 0.5f);
	float startingX = 0.0f;
	float startingY = 0.0f;

	float emit_angle_min = 0.0f;
	float emit_angle_max = 360.0f;
	float emit_radius_min = 0.0f;
	float emit_radius_max = 0.5f;
	 
	RandomEngine emit_angle_distribution; 
	RandomEngine emit_radius_distribution;
	RandomEngine scale_distribution;
	RandomEngine rotation_distribution;
	RandomEngine speed_distribution;
	RandomEngine rotation_speed_distribution;

	int sortingOrder = 9999; 
	int r = 255;
	int g = 255;
	int b = 255; 
	int a = 255;

	int localFrameNumber = 0;
	int frames_between_bursts = 1;
	int burst_quantity = 1;

	float start_scale_min = 1.0f;
	float start_scale_max = 1.0f;
	float rotation_min = 0.0f;
	float rotation_max = 0.0f;

	std::vector<float> x;
	std::vector<float> y;
	std::vector<float> scale;
	std::vector<float> rotation;
	std::vector<int> localParticleFrame;
	std::vector<float> velocityX;
	std::vector<float> velocityY; 
	std::vector<float> rotationSpeed;

	std::vector<float> initialScale;
	std::vector<float> current_r;
	std::vector<float> current_g;
	std::vector<float> current_b;
	std::vector<float> current_a;

	std::vector<float> initial_r;
	std::vector<float> initial_g;
	std::vector<float> initial_b;
	std::vector<float> initial_a;

	std::vector<bool> doRender;



	int duration_frames = 300;

	std::deque<int> freeSpace;

	float start_speed_min = 0.0f;
	float start_speed_max = 0.0f;
	float rotation_speed_min = 0.0f;
	float rotation_speed_max = 0.0f;
	float gravity_scale_x = 0.0f;
	float gravity_scale_y = 0.0f;
	float drag_factor = 1.0f;
	float angular_drag_factor = 1.0f;
	float end_scale = -1.0f;
	float end_color_r = -1;
	float end_color_g = -1;
	float end_color_b = -1;
	float end_color_a = -1;

	bool enabled = true;
	bool removed = false;

	bool emmissionAllowed = true;



	void OnStart();
	void OnUpdate();

	void GenerateNewParticles();

	void Stop();
	void Play();
	void Burst();

};

#endif