#pragma once

struct Pedestrian {
	int x;
	int y;
	char symbol;
	bool isDead;

	int maxMoneyDrop;
	int islandMinX;
	int islandMaxX;
	int health;
	int attack;
};