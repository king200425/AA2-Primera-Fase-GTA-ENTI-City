#pragma once
#include"Player.h"
#include"GameMap.h"
#include"Pedestrian.h"
#include"GameState.h"
#include"ConsoleUtils.h"

struct Game {
	Player cj;
	GameMap worldMap;
	Pedestrian* pedsArray;
	int totalPeds;

	int lsToll, sfToll, lsMaxMoney, sfMaxMoney;
	int mapWidth, mapHeight;
	int menuSelection;
	int timerCount;

	GameState currentState;
	bool isGameRunning;
	int viewWidth, viewHeight;

	bool LoadConfigAndInit();
	void Run();
	void Cleanup();

private:
	void ProcessMenu();
	void ProcessPlaying();
	void UpdateAI();
	void ProcessInit();
	void ProcessGameOver();
};