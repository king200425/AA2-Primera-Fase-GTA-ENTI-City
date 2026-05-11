#pragma once

#include"Player.h"
#include<iostream>

struct GameMap {
	char** grid;
	int width;
	int height;

	// Constructor to initialize the map dimensions and allocate memory
	void Initialize(int w, int h);

	// Destructor to free allocated memory
	void Destroy();

	// Function to render the map centered around the player's position
	void Render(const Player& player, int viewWidth, int viewHeight);
};