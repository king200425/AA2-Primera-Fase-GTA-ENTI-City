#include "Game.h"
#include <iostream>
#include <fstream>
#include <Windows.h>
#include <cstdlib>
#include <ctime>
#include <cmath>

using std::endl;
using std::cout;

bool Game::LoadConfigAndInit() {
    std::ifstream configFile("config.txt");
    int lsPedestrians, sfPedestrians;

    if (configFile.is_open()) {
        char delimiter;
        configFile >> mapWidth >> delimiter >> mapHeight >> delimiter;
        configFile >> lsPedestrians >> delimiter >> lsToll >> delimiter >> lsMaxMoney >> delimiter;
        configFile >> sfPedestrians >> delimiter >> sfToll >> delimiter >> sfMaxMoney >> delimiter;
        configFile.close();
    }
    else {
        return false;
    }

    worldMap.Initialize(mapWidth, mapHeight);
    worldMap.grid[mapHeight / 2][mapWidth / 3] = 'T';
    worldMap.grid[mapHeight / 2][(mapWidth * 2) / 3] = 'T';

    cj.x = 5;
    cj.y = mapHeight / 2;
    cj.symbol = '>';
    cj.money = 0;

    totalPeds = lsPedestrians + sfPedestrians;
    pedsArray = new Pedestrian[totalPeds];

    int lsMinX = 1;
    int lsMaxX = mapWidth / 3 - 1;
    int sfMinX = mapWidth / 3 + 1;
    int sfMaxX = (mapWidth * 2) / 3 - 1;

    // Initialize pedestrians with valid positions
    for (int i = 0; i < totalPeds; ++i) {
        pedsArray[i].symbol = 'P';
        pedsArray[i].isDead = false;

        if (i < lsPedestrians) {
            pedsArray[i].islandMinX = lsMinX;
            pedsArray[i].islandMaxX = lsMaxX;
            pedsArray[i].maxMoneyDrop = lsMaxMoney;
        }
        else {
            pedsArray[i].islandMinX = sfMinX;
            pedsArray[i].islandMaxX = sfMaxX;
            pedsArray[i].maxMoneyDrop = sfMaxMoney;
        }

        bool validPosition = false;
        while (!validPosition) {
            pedsArray[i].x = (i < lsPedestrians) ? (lsMinX + rand() % (lsMaxX - lsMinX + 1)) : (sfMinX + rand() % (sfMaxX - sfMinX + 1));
            pedsArray[i].y = 1 + rand() % (mapHeight - 2);
            if (worldMap.grid[pedsArray[i].y][pedsArray[i].x] == ' ' && !(pedsArray[i].x == cj.x && pedsArray[i].y == cj.y)) {
                validPosition = true;
            }
        }
        worldMap.grid[pedsArray[i].y][pedsArray[i].x] = pedsArray[i].symbol;
    }

    viewWidth = 15;
    viewHeight = 15;
    currentState = GameState::MENU;
    isGameRunning = true;

    return true;
}

// Process main menu input
void Game::ProcessMenu() {
    DrawMainMenu();
    if (GetAsyncKeyState(VK_RETURN) & 0x8000) {
        currentState = GameState::PLAYING;
        system("cls");
    }
    Sleep(100);
}

// Process player input and game logic while playing
void Game::ProcessPlaying() {
    int nextX = cj.x;
    int nextY = cj.y;

    // Handle movement input
    if (GetAsyncKeyState(VK_UP) & 0x8000) { nextY--; cj.symbol = '^'; }
    else if (GetAsyncKeyState(VK_DOWN) & 0x8000) { nextY++; cj.symbol = 'v'; }
    else if (GetAsyncKeyState(VK_LEFT) & 0x8000) { nextX--; cj.symbol = '<'; }
    else if (GetAsyncKeyState(VK_RIGHT) & 0x8000) { nextX++; cj.symbol = '>'; }

    // Handle attack input
    if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
        for (int i = 0; i < totalPeds; ++i) {
            if (!pedsArray[i].isDead && abs(pedsArray[i].x - cj.x) <= 1 && abs(pedsArray[i].y - cj.y) <= 1) {
                pedsArray[i].isDead = true;
                pedsArray[i].symbol = '$';
                worldMap.grid[pedsArray[i].y][pedsArray[i].x] = '$';
            }
        }
    }

    char nextTile = worldMap.grid[nextY][nextX];
	// Handle toll payment

    if (nextTile == 'T') {
        int tollToPay = (nextX == mapWidth / 3) ? lsToll : sfToll;
        if (cj.money >= tollToPay) {
            cj.money -= tollToPay;
            worldMap.grid[nextY][nextX] = ' ';
            nextTile = ' ';
        }
    }

    // Move player if next tile is empty or has money
    if (nextTile != 'X' && nextTile != 'P' && nextTile != 'T') {
        cj.x = nextX;
        cj.y = nextY;

        if (nextTile == '$') {
            for (int i = 0; i < totalPeds; ++i) {
                if (pedsArray[i].isDead && pedsArray[i].x == cj.x && pedsArray[i].y == cj.y) {
                    cj.money += 1 + rand() % pedsArray[i].maxMoneyDrop;
                    worldMap.grid[cj.y][cj.x] = ' ';

                    pedsArray[i].isDead = false;
                    pedsArray[i].symbol = 'P';
                    bool validPos = false;
                    while (!validPos) {
                        pedsArray[i].x = pedsArray[i].islandMinX + rand() % (pedsArray[i].islandMaxX - pedsArray[i].islandMinX + 1);
                        pedsArray[i].y = 1 + rand() % (mapHeight - 2);
                        if (worldMap.grid[pedsArray[i].y][pedsArray[i].x] == ' ' && !(pedsArray[i].x == cj.x && pedsArray[i].y == cj.y)) {
                            validPos = true;
                        }
                    }
                    worldMap.grid[pedsArray[i].y][pedsArray[i].x] = pedsArray[i].symbol;
                }
            }
        }
    }
}

// Update AI pedestrians' movement
void Game::UpdateAI() {
    for (int i = 0; i < totalPeds; ++i) {
        if (pedsArray[i].isDead) continue;
        if (abs(pedsArray[i].x - cj.x) <= 1 && abs(pedsArray[i].y - cj.y) <= 1) continue;

        if (rand() % 100 < 10) {
            int dir = rand() % 4;
            int nextPx = pedsArray[i].x;
            int nextPy = pedsArray[i].y;

            if (dir == 0) nextPy--;
            else if (dir == 1) nextPy++;
            else if (dir == 2) nextPx--;
            else if (dir == 3) nextPx++;

			// Ensure pedestrian stays within their island and doesn't move into walls or player
            if (nextPx >= pedsArray[i].islandMinX && nextPx <= pedsArray[i].islandMaxX) {
                if (worldMap.grid[nextPy][nextPx] == ' ') {
                    worldMap.grid[pedsArray[i].y][pedsArray[i].x] = ' ';
                    pedsArray[i].x = nextPx;
                    pedsArray[i].y = nextPy;
                    worldMap.grid[pedsArray[i].y][pedsArray[i].x] = 'P';
                }
            }
        }
    }
}

// Main game loop
void Game::Run() {
    system("cls");
    while (isGameRunning) {
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) isGameRunning = false;

        if (currentState == GameState::MENU) {
            ProcessMenu();
        }
        else if (currentState == GameState::PLAYING) {
            ProcessPlaying();
            UpdateAI();

            SetCursorPosition(0, 0);
            cout << "--- GTA: ENTI City Playing ---" << endl;
            cout << "Money: $" << cj.money << " | Toll 1: $" << lsToll << " | Toll 2: $" << sfToll << "    " << endl;
            cout << "-------------------------------" << endl;
            worldMap.Render(cj, viewWidth, viewHeight);
            cout << "-------------------------------" << endl;
            Sleep(60);
        }
    }
}

void Game::Cleanup() {
    system("cls");
    delete[] pedsArray;
    worldMap.Destroy();
    cout << "\n[System] Exiting game..." << endl;
    cout << "[System] Memory cleaned successfully! No leaks!" << endl;
}