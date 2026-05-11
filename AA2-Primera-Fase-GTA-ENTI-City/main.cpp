#include <iostream>
#include <fstream>
#include <Windows.h>
#include<cstdlib>
#include<ctime>
#include<cmath>
#include "Player.h"
#include "ConsoleUtils.h"
#include "GameMap.h"
#include"Pedestrian.h"
#include"GameState.h"

using std::endl;
using std::cout;
using std::ifstream;


int main() {
    srand((unsigned int)time(NULL));

    ifstream configFile("config.txt");

    int mapWidth = 0, mapHeight = 0;
    int lsPedestrians, lsToll, lsMaxMoney;
    int sfPedestrians, sfToll, sfMaxMoney;

    if (configFile.is_open()) {
        char delimiter;
        configFile >> mapWidth >> delimiter >> mapHeight >> delimiter;
        configFile >> lsPedestrians >> delimiter >> lsToll >> delimiter >> lsMaxMoney >> delimiter;
        configFile >> sfPedestrians >> delimiter >> sfToll >> delimiter >> sfMaxMoney >> delimiter;
        configFile.close();
    }
    else {
        cout << "[ERROR] Failed to load config.txt!" << endl;
        return -1;
    }

    // Initialize Map
    GameMap worldMap;
    worldMap.Initialize(mapWidth, mapHeight);

    // Initialize Player
    Player cj;
    cj.x = 5;
    cj.y = mapHeight / 2;
    cj.symbol = '>';
    cj.money = 0;

	int totalPeds = lsPedestrians + sfPedestrians;
	Pedestrian* PedsArray = new Pedestrian[totalPeds];

	int lsMinX = 1;
	int lsMaxX = mapWidth / 3 - 1;
	int sfMinX = mapWidth / 3 + 1;
	int sfMaxX = (mapWidth * 2) / 3 - 1;

    for (int i = 0; i < totalPeds; ++i) {
        PedsArray[i].symbol = 'P';
        PedsArray[i].isDead = false;

        if (i < lsPedestrians) {
            PedsArray[i].islandMinX = lsMinX;
            PedsArray[i].islandMaxX = lsMaxX;
            PedsArray[i].maxMoneyDrop = lsMaxMoney;
        }
        else {
            PedsArray[i].islandMinX = sfMinX;
            PedsArray[i].islandMaxX = sfMaxX;
            PedsArray[i].maxMoneyDrop = sfMaxMoney;
        }

        bool validPosition = false;
        while (!validPosition) {
            PedsArray[i].x = (i < lsPedestrians) ? (lsMinX + rand() % (lsMaxX - lsMinX + 1)) : (sfMinX + rand() % (sfMaxX - sfMinX + 1));
            PedsArray[i].y = 1 + rand() % (mapHeight - 2);

            if (worldMap.grid[PedsArray[i].y][PedsArray[i].x] == ' ' && !(PedsArray[i].x == cj.x && PedsArray[i].y == cj.y)) {
                validPosition = true;
            }
        }
		worldMap.grid[PedsArray[i].y][PedsArray[i].x] = PedsArray[i].symbol;
	}


    int viewWidth = 15;
    int viewHeight = 15;

	GameState currentState = GameState::MENU;
    bool isGameRunning = true;

    system("cls"); // Clear screen once before loop

    while (isGameRunning) {

        // 1. INPUT
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) isGameRunning = false;

        if (currentState == GameState::MENU) {
            DrawMainMenu();

            if (GetAsyncKeyState(VK_RETURN) & 0x8000) {
                currentState = GameState::PLAYING;
                system("cls"); // Debounce Enter key
            }
            Sleep(100); // Skip rest of loop until game starts
        }
        else if (currentState == GameState::PLAYING) {

            int nextX = cj.x;
            int nextY = cj.y;

            if (GetAsyncKeyState(VK_UP) & 0x8000) { nextY--; cj.symbol = '^'; }
            else if (GetAsyncKeyState(VK_DOWN) & 0x8000) { nextY++; cj.symbol = 'v'; }
            else if (GetAsyncKeyState(VK_LEFT) & 0x8000) { nextX--; cj.symbol = '<'; }
            else if (GetAsyncKeyState(VK_RIGHT) & 0x8000) { nextX++; cj.symbol = '>'; }

            if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
                for (int i = 0; i < totalPeds; ++i) {
                    if (!PedsArray[i].isDead && abs(PedsArray[i].x - cj.x) <= 1 && abs(PedsArray[i].y - cj.y) <= 1) {
                        PedsArray[i].isDead = true;
                        PedsArray[i].symbol = '$';
                        worldMap.grid[PedsArray[i].y][PedsArray[i].x] = '$';
                    }
                }
            }

            char nextTile = worldMap.grid[nextY][nextX];

            if (nextTile != 'X' && nextTile != 'P') {
                cj.x = nextX;
                cj.y = nextY;

                if (nextTile == '$') {
                    for (int i = 0; i < totalPeds; ++i) {
                        if (PedsArray[i].isDead && PedsArray[i].x == cj.x && PedsArray[i].y == cj.y) {
                            int earned = 1 + rand() % PedsArray[i].maxMoneyDrop;
                            cj.money += earned;

                            worldMap.grid[cj.y][cj.x] = ' ';

                            PedsArray[i].isDead = false;
                            PedsArray[i].symbol = 'P';
                            bool validPosition = false;
                            while (!validPosition) {
                                PedsArray[i].x = PedsArray[i].islandMinX + rand() % (PedsArray[i].islandMaxX - PedsArray[i].islandMinX + 1);
                                PedsArray[i].y = 1 + rand() % (mapHeight - 2);
                                if (worldMap.grid[PedsArray[i].y][PedsArray[i].x] == ' ' && !(PedsArray[i].x == cj.x && PedsArray[i].y == cj.y)) {
                                    validPosition = true;
                                }
                            }

                            worldMap.grid[PedsArray[i].y][PedsArray[i].x] = PedsArray[i].symbol;
                        }
                    }
                }
            }

            for (int i = 0; i < totalPeds; ++i) {
                if (PedsArray[i].isDead) continue;

                if (abs(PedsArray[i].x - cj.x) <= 1 && abs(PedsArray[i].y - cj.y) <= 1) {
                    continue;
                }

                if (rand() % 100 < 10) {
                    int dir = rand() % 4;
                    int nextPx = PedsArray[i].x;
                    int nextPy = PedsArray[i].y;

                    if (dir == 0) nextPy--;
                    else if (dir == 1) nextPy++;
                    else if (dir == 2) nextPx--;
                    else if (dir == 3) nextPx++;

                    if (nextPx >= PedsArray[i].islandMinX && nextPx <= PedsArray[i].islandMaxX) {
                        if (worldMap.grid[nextPy][nextPx] == ' ') {
                            worldMap.grid[PedsArray[i].y][PedsArray[i].x] = ' ';
                            PedsArray[i].x = nextPx;
                            PedsArray[i].y = nextPy;
                            worldMap.grid[PedsArray[i].y][PedsArray[i].x] = 'P';
                        }
                    }
                }
            }


            // 3. RENDER 
            SetCursorPosition(0, 0);
            cout << "--- GTA: ENTI City Starting ---" << endl;
            cout << "Money / Dinero: $" << cj.money << "       " << endl;
            cout << "-------------------------------" << endl;

            worldMap.Render(cj, viewWidth, viewHeight);

            cout << "-------------------------------" << endl;

            // 4. FRAME RATE 
            Sleep(60);
        }

    }
    // Cleanup
    system("cls");

    delete[] PedsArray;

    worldMap.Destroy();
    cout << "\n[System] Exiting game... / Saliendo del juego..." << endl;
    cout << "[System] Memory cleaned successfully! No leaks!" << endl;

}