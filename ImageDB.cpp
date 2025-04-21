#include "ImageDB.h"
#include "Helper.h"
#include <filesystem>
#include "gameEngine.h"
#include "Helper.h"
#include "ThirdParty/SDL2_ttf/SDL_ttf.h"



void ImageDB::LoadPNG(gameEngine* Game, std::vector<std::string> imageName, std::string path, std::vector<SDL_Texture*>* LoadedImageVector, SDL_Renderer* renderer)
{
	for (int i = 0; i < imageName.size(); i++)
	{
		std::string fileName = path + imageName[i] + ".png";
		if (std::filesystem::exists(fileName))
		{
			SDL_Texture* loadedImage = IMG_LoadTexture(renderer, fileName.c_str());
			LoadedImageVector->push_back(loadedImage);
		}
		else
		{
			cout << "error: missing image " << imageName[i];
			exit(0);
		}
	}
}
void ImageDB::RenderSingleImage(gameEngine* Game, SDL_Renderer* renderer, SDL_Texture* image)
{
	Helper::SDL_RenderCopy(renderer, image, nullptr, nullptr);
}

void ImageDB::Draw(std::string content, float x, float y, std::string fontName, int fontSize, int r, int g, int b, int a)
{
	if (pointerToSelf->loadedTextFonts.find(fontName) == pointerToSelf->loadedTextFonts.end())
	{
		// The font does not exist
		LoadFont(fontName, fontSize);
	}
	if (pointerToSelf->loadedTextFonts.at(fontName).find(fontSize) == pointerToSelf->loadedTextFonts.at(fontName).end())
	{
		// The font size for this font does not exist
		LoadFont(fontName, fontSize);
	}

	// We are garunteed to have the font and the fontsize at this point

	TextQueueData temp = { content, x, y, fontName, fontSize, r, g, b, a };
	pointerToSelf->TextToRenderQueue.push(temp);
}

void ImageDB::LoadFont(std::string fontName, int fontSize)
{
	if (std::filesystem::exists("resources/fonts/" + fontName + ".ttf"))
	{
		TTF_Font* currentFont = TTF_OpenFont(("resources/fonts/" + fontName + ".ttf").c_str(), fontSize);
		std::unordered_map<int, TTF_Font*> tempMap;
		tempMap.insert({ fontSize, currentFont });
		if (pointerToSelf->loadedTextFonts.find(fontName) == pointerToSelf->loadedTextFonts.end())
		{
			// The font does not exist
			pointerToSelf->loadedTextFonts.insert({ fontName, tempMap });
		}
		else
		{
			// The font does exist
			if (pointerToSelf->loadedTextFonts.at(fontName).find(fontSize) == pointerToSelf->loadedTextFonts.at(fontName).end())
			{
				// Font size does not exist
				pointerToSelf->loadedTextFonts.at(fontName) = tempMap;
			}
		}
	}
	else
	{
		cout << "error: font " << fontName << " missing";
		exit(0);
	}
}

void ImageDB::ClearTextQueue()
{
	while (!pointerToSelf->TextToRenderQueue.empty())
	{
		TextQueueData temp;
		temp = pointerToSelf->TextToRenderQueue.front();

		SDL_Color textColor = { static_cast<Uint8>(temp.r), static_cast<Uint8>(temp.g), static_cast<Uint8>(temp.b), static_cast<Uint8>(temp.a) };
		SDL_Surface* currentRender = TTF_RenderText_Solid(pointerToSelf->loadedTextFonts.at(temp.fontName).at(temp.fontSize), temp.content.c_str(), textColor);
		float textWidth = currentRender->w;
		float textHeight = currentRender->h;
		SDL_FRect textRect = { temp.x, temp.y, textWidth, textHeight };
		SDL_Texture* textTexture = SDL_CreateTextureFromSurface(pointerToSelf->renderer, currentRender);
		Helper::SDL_RenderCopy(pointerToSelf->renderer, textTexture, nullptr, &textRect);
		pointerToSelf->TextToRenderQueue.pop();
	}
}

void ImageDB::DrawUI(std::string imageName, float x, float y)
{
	pointerToSelf->CheckForImage(imageName);
	// We are garunteed to have the image at this point

	SDL_Texture* tex = pointerToSelf->loadedImages.at(imageName);
	SDL_FRect texRect;
	Helper::SDL_QueryTexture(tex, &texRect.w, &texRect.h);
	ImageDrawRequest currentRequest = { imageName, x, y, 0, 1.0f, 1.0f, 0.5f, 0.5f, 255, 255, 255, 255, 0 , pointerToSelf->DrawingUIPrioirtyNumber};
	pointerToSelf->DrawingUIPrioirtyNumber++;

	pointerToSelf->ImagesToDraw.push_back(currentRequest);
}

void ImageDB::CheckForImage(std::string imageName)
{
	if (pointerToSelf->loadedImages.find(imageName) == pointerToSelf->loadedImages.end())
	{
		// image is not currently loaded in
		std::string fileName = "resources/images/" + imageName + ".png";
		if (std::filesystem::exists(fileName))
		{
			SDL_Texture* loadedImage = IMG_LoadTexture(pointerToSelf->renderer, fileName.c_str());
			pointerToSelf->loadedImages.insert({ imageName, loadedImage });
		}
		else
		{
			cout << "error: missing image " << imageName;
			exit(0);
		}
	}
}

void ImageDB::DrawUIEx(std::string imageName, float x, float y, float r, float g, float b, float a, int sortingOrder)
{
	pointerToSelf->CheckForImage(imageName);
	// We are garunteed to have the image at this point

	SDL_Texture* tex = pointerToSelf->loadedImages.at(imageName);
	SDL_FRect texRect;
	Helper::SDL_QueryTexture(tex, &texRect.w, &texRect.h);
	ImageDrawRequest currentRequest = { imageName, x, y, 0, 1.0f, 1.0f, 0.5f, 0.5f, static_cast<int>(r), static_cast<int>(g), static_cast<int>(b), static_cast<int>(a), sortingOrder , pointerToSelf->DrawingUIPrioirtyNumber };
	pointerToSelf->DrawingUIPrioirtyNumber++;

	pointerToSelf->ImagesToDraw.push_back(currentRequest);
}

void ImageDB::DrawImage(std::string imageName, float x, float y)
{
	pointerToSelf->CheckForImage(imageName);
	// We are garunteed to have the image at this point

	SDL_Texture* tex = pointerToSelf->loadedImages.at(imageName);
	SDL_FRect texRect;
	Helper::SDL_QueryTexture(tex, &texRect.w, &texRect.h);
	ImageDrawRequest currentRequest = { imageName, x, y, 0, 1.0f, 1.0f, 0.5f, 0.5f, 255, 255, 255, 255, 0 , pointerToSelf->DrawPrioirtyNumber };
	pointerToSelf->DrawPrioirtyNumber++;

	pointerToSelf->DrawRequests.push_back(currentRequest);
}

void ImageDB::DrawImageEx(std::string imageName, float x, float y, float rotation, float scaleX, float scaleY, float pivotX, float pivotY, float r, float g, float b, float a, int sortingOrder)
{
	pointerToSelf->CheckForImage(imageName);
	// We are garunteed to have the image at this point

	SDL_Texture* tex = pointerToSelf->loadedImages.at(imageName);
	SDL_FRect texRect;
	Helper::SDL_QueryTexture(tex, &texRect.w, &texRect.h);
	float halfWinX = pointerToEngine->WindowXResolution * 0.5f;
	float halfWinY = pointerToEngine->WindowYResolution * 0.5f;

	float left = x;
	float right = x + texRect.w;
	float top = y;
	float bottom = y + texRect.h;

	float camLeft = pointerToEngine->CameraPosition.x - halfWinX;
	float camRight = pointerToEngine->CameraPosition.x + halfWinX;
	float camTop = pointerToEngine->CameraPosition.y - halfWinY;
	float camBottom = pointerToEngine->CameraPosition.y + halfWinY;

	// Check if sprite is completely outside camera bounds
	if (right < camLeft || left > camRight ||
		bottom < camTop || top > camBottom)
	{
		return; // Don't draw, it's off screen
	}
	int newRotation = static_cast<int>(rotation);
	ImageDrawRequest currentRequest = { imageName, x, y, newRotation, scaleX, scaleY, pivotX, pivotY, static_cast<int>(r), static_cast<int>(g), static_cast<int>(b), static_cast<int>(a), sortingOrder , pointerToSelf->DrawPrioirtyNumber };
	pointerToSelf->DrawPrioirtyNumber++;

	pointerToSelf->DrawRequests.push_back(currentRequest);

}

void ImageDB::DrawPixel(float x, float y, float r, float g, float b, float a)
{
	PixelData currentRequest = { x, y, static_cast<int>(r), static_cast<int>(g), static_cast<int>(b), static_cast<int>(a) };
	pointerToSelf->PixelRequests.push_back(currentRequest);
}



void ImageDB::RenderAndClearAllImages(std::deque<ImageDrawRequest>& currentDrawQueue)
{
	std::stable_sort(currentDrawQueue.begin(), currentDrawQueue.end(), ImageDB::ImageRenderComparator);
	float zoomFactor = pointerToEngine->cameraZoom;
	SDL_RenderSetScale(pointerToSelf->renderer, zoomFactor, zoomFactor);

	for (auto& request : currentDrawQueue)
	{
		const int pixelsPerMeter = 100;
		glm::vec2 finalRenderingPosition = glm::vec2(request.x, request.y) - pointerToSelf->CameraPosition;
		//cout << finalRenderingPosition.x << endl;;
		SDL_Texture* tex = pointerToSelf->loadedImages.at(request.imageName);
		SDL_FRect texRect;
		Helper::SDL_QueryTexture(tex, &texRect.w, &texRect.h);

		int flipMode = SDL_FLIP_NONE;
		if (request.scaleX < 0)
			flipMode |= SDL_FLIP_HORIZONTAL;
		if (request.scaleY < 0)
			flipMode |= SDL_FLIP_VERTICAL;

		float xScale = glm::abs(request.scaleX);
		float yScale = glm::abs(request.scaleY);

		texRect.w *= xScale;
		texRect.h *= yScale;

		SDL_FPoint pivotPoint = { (request.pivotX * texRect.w), (request.pivotY * texRect.h) };

		glm::ivec2 camDimensions;
		camDimensions.x = pointerToEngine->WindowXResolution;
		camDimensions.y = pointerToEngine->WindowYResolution;

		texRect.x =(finalRenderingPosition.x * 100.0f + camDimensions.x * 0.5f * (1.0f / zoomFactor) - pivotPoint.x);
		texRect.y =(finalRenderingPosition.y * 100.0f + camDimensions.y * 0.5f * (1.0f / zoomFactor) - pivotPoint.y);

		SDL_SetTextureColorMod(tex, request.r, request.g, request.b);
		SDL_SetTextureAlphaMod(tex, request.a);

		Helper::SDL_RenderCopyEx(1, request.imageName, pointerToSelf->renderer, tex, NULL, &texRect, request.rotationDegrees, &pivotPoint, static_cast<SDL_RendererFlip>(flipMode));

		SDL_RenderSetScale(pointerToSelf->renderer, zoomFactor, zoomFactor);

		SDL_SetTextureColorMod(tex, 255, 255, 255);
		SDL_SetTextureAlphaMod(tex, 255);
	}

	SDL_RenderSetScale(pointerToSelf->renderer, 1, 1);

	currentDrawQueue.clear();
	pointerToSelf->DrawPrioirtyNumber = 0;
}

void ImageDB::RenderAndClearAllUIImages(std::deque<ImageDrawRequest>& currentDrawQueue)
{
	std::stable_sort(currentDrawQueue.begin(), currentDrawQueue.end(), ImageDB::ImageRenderComparator);
	float zoomFactor = pointerToEngine->cameraZoom;
	//SDL_RenderSetScale(pointerToSelf->renderer, zoomFactor, zoomFactor);

	for (auto& request : currentDrawQueue)
	{
		const float pixelsPerMeter = 100.0f;
		glm::vec2 finalRenderingPosition = glm::vec2(request.x, request.y) - pointerToSelf->CameraPosition;
		//cout << finalRenderingPosition.x << endl;;
		SDL_Texture* tex = pointerToSelf->loadedImages.at(request.imageName);
		SDL_FRect texRect;
		Helper::SDL_QueryTexture(tex, &texRect.w, &texRect.h);

		int flipMode = SDL_FLIP_NONE;
		if (request.scaleX < 0)
			flipMode |= SDL_FLIP_HORIZONTAL;
		if (request.scaleY < 0)
			flipMode |= SDL_FLIP_VERTICAL;

		float xScale = glm::abs(request.scaleX);
		float yScale = glm::abs(request.scaleY);

		texRect.w *= xScale;
		texRect.h *= yScale;

		SDL_FPoint pivotPoint = { (request.pivotX * texRect.w), (request.pivotY * texRect.h) };

		glm::ivec2 camDimensions;
		camDimensions.x = pointerToEngine->WindowXResolution;
		camDimensions.y = pointerToEngine->WindowYResolution;
		texRect.x = (finalRenderingPosition.x);
		texRect.y = (finalRenderingPosition.y);

		SDL_SetTextureColorMod(tex, request.r, request.g, request.b);
		SDL_SetTextureAlphaMod(tex, request.a);

		Helper::SDL_RenderCopyEx(1, request.imageName, pointerToSelf->renderer, tex, NULL, &texRect, request.rotationDegrees, &pivotPoint, static_cast<SDL_RendererFlip>(flipMode));

		//SDL_RenderSetScale(pointerToSelf->renderer, zoomFactor, zoomFactor
		SDL_SetTextureColorMod(tex, 255, 255, 255);
		SDL_SetTextureAlphaMod(tex, 255);
	}

	//SDL_RenderSetScale(pointerToSelf->renderer, 1, 1);

	currentDrawQueue.clear();
	pointerToSelf->DrawingUIPrioirtyNumber = 0;
}

void ImageDB::RenderAndClearPixels()
{
	for (auto& request : pointerToSelf->PixelRequests)
	{
		SDL_SetRenderDrawBlendMode(pointerToSelf->renderer, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(pointerToSelf->renderer, request.r, request.g, request.b, request.a);
		SDL_RenderDrawPoint(pointerToSelf->renderer, request.x, request.y);
		SDL_SetRenderDrawBlendMode(pointerToSelf->renderer, SDL_BLENDMODE_NONE);
	}
	pointerToSelf->PixelRequests.clear();
}

void ImageDB::SetPosition(float x, float y)
{
	pointerToSelf->CameraPosition.x = x;
	pointerToSelf->CameraPosition.y = y;
}

float ImageDB::GetPositionX()
{
	return pointerToSelf->CameraPosition.x;
}

float ImageDB::GetPositionY()
{
	return pointerToSelf->CameraPosition.y;
}

void ImageDB::SetZoom(float newZoom)
{
	pointerToEngine->cameraZoom = newZoom;
}

float ImageDB::GetZoom()
{
	return pointerToEngine->cameraZoom;
}

void ImageDB::CreateDefaultParticleTextureWithName(const std::string& name)
{
	if (loadedImages.find(name) != loadedImages.end())
	{
		return;
	}

	SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, 8, 8, 32, SDL_PIXELFORMAT_RGBA8888);

	Uint32 white_color = SDL_MapRGBA(surface->format, 255, 255, 255, 255);
	SDL_FillRect(surface, NULL, white_color);

	SDL_Texture* texture = SDL_CreateTextureFromSurface(pointerToSelf->renderer, surface);

	loadedImages[name] = texture;
	SDL_FreeSurface(surface);
	
}


