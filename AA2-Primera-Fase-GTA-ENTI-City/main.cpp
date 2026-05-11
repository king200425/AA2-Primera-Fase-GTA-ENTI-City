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

    Pedestrian* lsPedsArray = new Pedestrian[lsPedestrians];

    int lsMaxX = mapWidth / 3 - 1;

    for (int i = 0; i < lsPedestrians; ++i) {
        lsPedsArray[i].symbol = 'P';
        lsPedsArray[i].isDead = false;

        bool validPosition = false;
        while (!validPosition) {
            lsPedsArray[i].x = 1 + rand() % lsMaxX;
            lsPedsArray[i].y = 1 + rand() % (mapHeight - 2);

            if (worldMap.grid[lsPedsArray[i].y][lsPedsArray[i].x] == ' ' && !(lsPedsArray[i].x == cj.x && lsPedsArray[i].y == cj.y)) {
                validPosition = true;
            }
        }
        worldMap.grid[lsPedsArray[i].y][lsPedsArray[i].x] = lsPedsArray[i].symbol;
    }

    int viewWidth = 15;
    int viewHeight = 15;
    bool isGameRunning = true;

    system("cls"); // Clear screen once before loop

    while (isGameRunning) {

        // 1. INPUT
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) isGameRunning = false;

        int nextX = cj.x;
        int nextY = cj.y;

        if (GetAsyncKeyState(VK_UP) & 0x8000) { nextY--; cj.symbol = '^'; }
        else if (GetAsyncKeyState(VK_DOWN) & 0x8000) { nextY++; cj.symbol = 'v'; }
        else if (GetAsyncKeyState(VK_LEFT) & 0x8000) { nextX--; cj.symbol = '<'; }
        else if (GetAsyncKeyState(VK_RIGHT) & 0x8000) { nextX++; cj.symbol = '>'; }

        if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
            for (int i = 0; i < lsPedestrians; ++i) {
                // Check if pedestrian is alive and within 1 unit of the player
                if (!lsPedsArray[i].isDead && abs(lsPedsArray[i].x - cj.x) <= 1 && abs(lsPedsArray[i].y - cj.y) <= 1) {
                    lsPedsArray[i].isDead = true;  // Mark pedestrian as dead
                    lsPedsArray[i].symbol = '$';  // Remove pedestrian symbol from map
                    worldMap.grid[lsPedsArray[i].y][lsPedsArray[i].x] = '$';
                }
            }
        }
        // Check next tile for debugging
        char nextTile = worldMap.grid[nextY][nextX];

        if (nextTile != 'X' && nextTile != 'P') {
            cj.x = nextX;
            cj.y = nextY;

            if (nextTile == '$') {
                for (int i = 0; i < lsPedestrians; ++i) {
                    // Check if this pedestrian is the one we just killed
                    if (lsPedsArray[i].isDead && lsPedsArray[i].x == cj.x && lsPedsArray[i].y == cj.y) {
                        int earned = 1 + rand() % lsMaxMoney; // Earn between 1 and lsMaxMoney
                        cj.money += earned;

                        // Remove the corpse from the map
                        worldMap.grid[cj.y][cj.x] = ' ';

                        lsPedsArray[i].isDead = false;
                        lsPedsArray[i].symbol = 'P';
                        bool validPosition = false;
                        while (!validPosition) {
                            lsPedsArray[i].x = 1 + rand() % lsMaxX;
                            lsPedsArray[i].y = 1 + rand() % (mapHeight - 2);
                            if (worldMap.grid[lsPedsArray[i].y][lsPedsArray[i].x] == ' ' && !(lsPedsArray[i].x == cj.x && lsPedsArray[i].y == cj.y)) {
                                validPosition = true;
                            }
                        }

                        worldMap.grid[lsPedsArray[i].y][lsPedsArray[i].x] = lsPedsArray[i].symbol; // Place new pedestrian on map
                    }
                }
            }
        }

        for (int i = 0; i < lsPedestrians; ++i) {
            if (!lsPedsArray[i].isDead) continue;

            if (abs(lsPedsArray[i].x - cj.x) <= 1 && abs(lsPedsArray[i].y - cj.y) <= 1) {
                continue;
            }

            if (rand() % 100 < 10) {
                int dir = rand() % 4;
                int nextPx = lsPedsArray[i].x;
                int nextPy = lsPedsArray[i].y;

                if (dir == 0) nextPy--;
                else if (dir == 1) nextPy++;
                else if (dir == 2) nextPx--;
                else if (dir == 3) nextPx++;

                if (worldMap.grid[nextPy][nextPx] == ' ') {
                    worldMap.grid[lsPedsArray[i].y][lsPedsArray[i].x] = ' ';
                    lsPedsArray[i].x = nextPx;
                    lsPedsArray[i].y = nextPy;
                    worldMap.grid[lsPedsArray[i].y][lsPedsArray[i].x] = 'P';
                }
            }
        }


        // 3. RENDER 
        SetCursorPosition(0, 0);
        cout << "--- GTA: ENTI City Starting ---" << endl;
        cout << "Money / Dinero: $" << cj.money << "      " << endl;
        cout << "-------------------------------" << endl;

        worldMap.Render(cj, viewWidth, viewHeight);

        cout << "-------------------------------" << endl;

        // 4. FRAME RATE 
        Sleep(60);
    }

    // Cleanup
    system("cls");

    delete[] lsPedsArray;

    worldMap.Destroy();
    cout << "\n[System] Exiting game... / Saliendo del juego..." << endl;
    cout << "[System] Memory cleaned successfully! No leaks!" << endl;
}