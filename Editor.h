#pragma once


#ifndef EDITOR_H
#define EDITOR_H
#define GLM_ENABLE_EXPERIMENTAL

#include <iostream>
#include <filesystem>
#include <vector>
#include <string>
#include <fstream>
#include <array>
#include <unordered_map>
#include <map>
#include <memory>
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include <stdio.h>
#include "ThirdParty/SDL2/SDL.h"
#include "ThirdParty/SDL2_mixer/SDL_mixer.h"
#include "ThirdParty/SDL2_ttf/SDL_ttf.h"
#include "ThirdParty/SDL2_image/SDL_image.h"
#include "Helper.h"
#include <utility>
#include <algorithm>
#include "ThirdParty/rapidjson/include/rapidjson/document.h"
#include "ThirdParty/rapidjson/include/rapidjson/writer.h"
#include "ThirdParty/rapidjson/include/rapidjson/prettywriter.h"
#include "ThirdParty/rapidjson/include/rapidjson/stringbuffer.h"
#include <fstream>
#include <cstdlib>

struct editorRigidbody
{
	std::string body_type = "dynamic";
	bool precise = true;
	float gravity_scale = 1.0f;
	float density = 1.0f;
	float angularFriction = 0.3f;
	float rotation = 0.0f;          // This value will ALWAYS be in radians. If needed for lua, convert it to degrees rads * (180.0f / b2_pi)
	bool hasCollider = true;
	bool has_trigger = true;
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
};

struct editorActor {
	std::string name = "";
	ImVec2 position = ImVec2(0.0f, 0.0f);
	int UUID = -1;
	std::vector<std::string> custom_keywords;
	std::vector<std::string> custom_data;

	bool isRendering = false;
	std::string textureName = "";
	SDL_Texture* associatedTexture;
	float rotation = 0.0f;
	float scaleX = 1.0f;
	float scaleY = 1.0f;
	float pivotX = 0.0f;
	float pivotY = 0.0f;
	float r = 255.0f;
	float g = 255.0f;
	float b = 255.0f;
	float a = 255.0f;
	int sorting_order;
	editorRigidbody rigidbody;
	std::unordered_map<std::string, std::string> components;
	
};

class Editor 
{
public: 
	Editor() {};

	struct ImageDrawRequest
	{
		std::string imageName;
		float x;
		float y;
		int rotationDegrees = 0.0f;
		float scaleX = 1.0f;;
		float scaleY = 1.0f;
		float pivotX = 0.5f;
		float pivotY = 0.5f;
		int r;
		int g;
		int b;
		int a = 255;
		int sortingOrder;
		int sortingOrderPriority;
	};

	static bool ImageRenderComparator(ImageDrawRequest a, ImageDrawRequest b)
	{
		if (a.sortingOrder == b.sortingOrder)
		{
			return a.sortingOrderPriority < b.sortingOrderPriority;
		}
		return a.sortingOrder < b.sortingOrder;
	}

	// Global things to be used by everyone
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
	ImGuiIO* io = nullptr;
	ImVec4 clear_color;

	std::string currentSceneName;
	std::vector<std::string> Scenes;

	std::unordered_map<std::string, std::vector<std::shared_ptr<editorActor>>> currentSceneActorMap;

	// ImageDB stuff
	std::unordered_map<std::string, SDL_Texture*> loadedImages;
	std::deque<ImageDrawRequest> DrawRequests;
	int DrawPrioirtyNumber = 0;

	std::unordered_map<std::string, std::string> available_components;

	int keyCounter = 0;

	bool addCustom = false;
	bool keywordReady = false;
	bool DataReady = false;

	int EditorBegin();
	bool check_for_resourec_folder();
	bool check_for_config_file();
	bool check_for_rendering_config();
	void create_config_file();
	void create_render_config_file();
	void basic_print_JSON_to_file(std::string filename, std::vector<std::string> keywords, std::vector<std::array<char, 32>> data);
	void basic_print_JSON_to_file_numbers(std::string filename, std::vector<std::string> keywords, std::vector<std::array<char, 32>> data);
	void create_actor_list_in_scene();
	void Open_Properties_Menu(int selected_actor_index);
	void create_scene();
	void advanced_JSON_output(std::vector<std::shared_ptr<editorActor>> actors);
	void add_end_button(bool* done);
	void Render_Editor_Actors();
	void checkForImage(std::string imageName);
	void render_and_clear_image_queue();

	void editor();
};

#endif