#pragma once
#ifndef RENDERER_H
#define RENDERER_H

#include <string>
#include "ThirdParty/glm/glm.hpp"
#include "gameEngine.h"

// Requires: 
// Modifies: 

// Requires: Pointer to both playerHealth and playerScore
// Modifies: 
// Effects: Renders the game
void Render(gameEngine* Game);

void gameStartMessage(gameEngine* Game);

// Requires: 
// Modifies: 
// Effects: Print the bad ending game over message
void badEnding(gameEngine* Game);

void goodEnding(gameEngine* Game);


#endif