#pragma once

#ifndef IMAGEDB_H
#define IMAGEDB_H

#include <string>
#include <vector>
#include "ThirdParty/SDL2_image/SDL_image.h"
#include "ThirdParty/SDL2/SDL.h"
#include "ThirdParty/SDL2_ttf/SDL_ttf.h"
#include "ThirdParty/glm/glm.hpp"
#include <queue>
#include "lua.hpp"
#include "ThirdParty/LuaBridge/LuaBridge.h"
#include <utility>
#include <algorithm>
#include <deque>

class gameEngine;

struct TextQueueData {
	std::string content;
	float x;
	float y;
	std::string fontName;
	int fontSize;
	int r;
	int g;
	int b;
	int a;
};

struct ImageDrawRequest
{
	std::string imageName;
	float x;
	float y;
	int rotationDegrees;
	float scaleX;
	float scaleY;
	float pivotX;
	float pivotY;
	int r;
	int g;
	int b;
	int a;
	int sortingOrder;
	int sortingOrderPriority;
};

struct PixelData
{
	float x;
	float y;
	int r;
	int g;
	int b;
	int a;
};


class ImageDB
{
public:
	static ImageDB* pointerToSelf;
	static gameEngine* pointerToEngine;

	std::vector<SDL_Texture*> LoadedTextures;

	std::unordered_map<std::string, std::unordered_map<int, TTF_Font*>> loadedTextFonts;

	SDL_Renderer* renderer = nullptr;

	std::queue<TextQueueData> TextToRenderQueue;

	ImageDB() {
		pointerToSelf = this;  // Initialize pointerToSelf in the constructor
	}

	static bool ImageRenderComparator(ImageDrawRequest a, ImageDrawRequest b)
	{
		if (a.sortingOrder == b.sortingOrder)
		{
			return a.sortingOrderPriority < b.sortingOrderPriority;
		}
		return a.sortingOrder < b.sortingOrder;
	}

	glm::vec2 CameraPosition = glm::vec2(0.0f, 0.0f);
	std::deque<ImageDrawRequest> ImagesToDraw;
	std::deque<ImageDrawRequest> DrawRequests;
	std::deque<PixelData> PixelRequests;

	std::unordered_map<std::string, SDL_Texture*> loadedImages;

	void LoadPNG(gameEngine* Game, std::vector<std::string> imageName, std::string path, std::vector<SDL_Texture*>* LoadedImageVector, SDL_Renderer* renderer);
	

	void RenderSingleImage(gameEngine* Game, SDL_Renderer* renderer, SDL_Texture* image);
	static inline int DrawingUIPrioirtyNumber = 0;
	static inline int DrawPrioirtyNumber = 0;

	// Rendering API Functions
	static void Draw(std::string content, float x, float y, std::string fontName, int fontSize, int r, int g, int b, int a);
	static void LoadFont(std::string fontName, int fontSize);
	static void ClearTextQueue();
	static void DrawUI(std::string imageName, float x, float y);
	static void CheckForImage(std::string imageName);
	static void RenderAndClearAllImages(std::deque<ImageDrawRequest>& currentDrawQueue);
	static void RenderAndClearAllUIImages(std::deque<ImageDrawRequest>& currentDrawQueue);
	static void DrawUIEx(std::string imageName, float x, float y, float r, float g, float b, float a, int sortingOrder);
	static void DrawImage(std::string imageName, float x, float y);
	static void DrawImageEx(std::string imageName, float x, float y, float rotation, float scaleX, float scaleY, float pivotX, float pivotY, float r, float g, float b, float a, int sortingOrder);
	static void DrawPixel(float x, float y, float r, float g, float b, float a);
	static void RenderAndClearPixels();

	// Camera API functions
	static void SetPosition(float x, float y);
	static float GetPositionX();
	static float GetPositionY();
	static void SetZoom(float newZoom);
	static float GetZoom();


	// Homework9 Stuff
	void CreateDefaultParticleTextureWithName(const std::string& name);
};

#endif
