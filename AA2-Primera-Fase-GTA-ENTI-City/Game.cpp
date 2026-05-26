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

    int cjHealth = 0, cjAttack = 0;
	int lsPedestrians = 0, lsPedHealth = 0, lsPedAttack = 0;
	int sfPedestrians = 0, sfPedHealth = 0, sfPedAttack = 0;
	int lvPedestrians = 0, lvPedMaxMoney = 0, lvPedHealth = 0, lvPedAttack = 0;

    if (configFile.is_open()) {
        char delimiter;
		//Map dimensions
        configFile >> mapWidth >> delimiter >> mapHeight >> delimiter;
        //Health
        configFile >> cjHealth >> delimiter >> cjAttack >> delimiter;
		//Toll costs
		configFile >> lsToll >> delimiter >> sfToll >> delimiter;
        //Los Santos
        configFile >> lsPedestrians >> delimiter >> lsMaxMoney >> delimiter >> lsPedHealth >> delimiter >> lsPedAttack >> delimiter;
        //San Fierro
        configFile >> sfPedestrians >> delimiter >> sfMaxMoney >> delimiter >> sfPedHealth >> delimiter >> sfPedAttack >> delimiter;
		//Las Venturas
        configFile >> lvPedestrians >> delimiter >> lvPedMaxMoney >> delimiter >> lvPedHealth >> delimiter >> lvPedAttack >> delimiter;

        configFile.close();
    }
    else {
        return false;
    }
	// Initialize game map and player
    worldMap.Initialize(mapWidth, mapHeight);
    worldMap.grid[mapHeight / 2][mapWidth / 3] = 'T';
    worldMap.grid[mapHeight / 2][(mapWidth * 2) / 3] = 'T';

    //CJ
    cj.x = 5;
    cj.y = mapHeight / 2;
    cj.symbol = '>';
    cj.money = 0;
	cj.health = cjHealth;
	cj.attack = cjAttack;

	// Initialize pedestrians
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

		pedsArray[i].isAggressive = (rand() % 2 == 0);  // 50% chance to be aggressive
		pedsArray[i].isProvoked = false;  // No están provocados al inicio
		pedsArray[i].attackCooldown = 0;  // CD

        //Asignar diferentes atributos a los peatones según la isla
        if (i < lsPedestrians) {
            pedsArray[i].islandMinX = lsMinX;
            pedsArray[i].islandMaxX = lsMaxX;
            pedsArray[i].maxMoneyDrop = lsMaxMoney;
            pedsArray[i].health = lsPedHealth;
			pedsArray[i].attack = lsPedAttack;
        }
        else {
            pedsArray[i].islandMinX = sfMinX;
            pedsArray[i].islandMaxX = sfMaxX;
            pedsArray[i].maxMoneyDrop = sfMaxMoney;
			pedsArray[i].health = sfPedHealth;
			pedsArray[i].attack = sfPedAttack;
        }

		// Buscar una posición vacía y segura para generar el peatón
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

    //Cars
    totalCars = 3;
	carsArray = new Car[totalCars];

	int islandMins[3] = { 1, mapWidth / 3 + 1, (mapWidth * 2) / 3 + 1 };
	int islandMaxs[3] = { mapWidth / 3 - 1, (mapWidth * 2) / 3 - 1, mapWidth - 2 };

    for (int i = 0; i < totalCars; ++i) {
        carsArray[i].symbol = 'C';
        bool valid = false;
        while (!valid) {
            carsArray[i].x = islandMins[i] + rand() % (islandMaxs[i] - islandMins[i] + 1);
            carsArray[i].y = 1 + rand() % (mapHeight - 2);
            if (worldMap.grid[carsArray[i].y][carsArray[i].x] == ' ' && !(carsArray[i].x == cj.x && carsArray[i].y == cj.y)) {
                valid = true;
            }
        }
        worldMap.grid[carsArray[i].y][carsArray[i].x] = carsArray[i].symbol;
    }

    //No mober cars
	isDriving = false;
	currentCarIndex = -1;

	// Initialize other game variables
    viewWidth = 15;
    viewHeight = 15;
    currentState = GameState::INIT;
    timerCount = 0;
	menuSelection = 0;
    isGameRunning = true;

    return true;
}

// Procesar la lógica del menú principal
void Game::ProcessInit() {
	SetCursorPosition(0, 0);
    cout << "========================================" << endl;
    cout << "      GRAND THEFT AUTO: ENTI CITY      " << endl;
    cout << "========================================" << endl;
    cout << "\n\n      Loading game assets...          " << endl;
    cout << "      Please wait for a moment...     " << endl;
    cout << "\n========================================" << endl;

    timerCount++;
    if (timerCount >= 50){
        currentState = GameState::MENU;
		timerCount = 0;// Clear the screen before showing the menu
        system("cls");
    }
    Sleep(60);
}

// Process main menu input
void Game::ProcessMenu() {
    SetCursorPosition(0, 0);
    cout << "========================================" << endl;
    cout << "               MAIN MENU                " << endl;
    cout << "========================================" << endl;
    cout << "\n      Use UP/DOWN arrows to select:\n" << endl;

    if (menuSelection != 0 && menuSelection != 1) {
        menuSelection = 0;
    }

    if (menuSelection == 0) {
        cout << "   --> [ PLAY GAME ] <--" << endl;
        cout << "       [   EXIT    ]    " << endl;
    }
    else {
        cout << "       [ PLAY GAME ]    " << endl;
        cout << "   --> [   EXIT    ] <--" << endl;
    }
    cout << "\n========================================" << endl;

    if (GetAsyncKeyState(VK_UP) & 0x8000) {
        menuSelection = 0;
        Sleep(150);
    }
    else if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
        menuSelection = 1;
        Sleep(150);
    }

    if (GetAsyncKeyState(VK_RETURN) & 0x8000) {
        if (menuSelection == 0) {
            currentState = GameState::PLAYING;
        }
        else {
			isGameRunning = false;// Exit the game loop to quit
        }
        system("cls");
        Sleep(200);
    }
}


// Process player input and game logic while playing
void Game::ProcessPlaying() {
    if (GetAsyncKeyState('E') & 0x8000) {
        if (!isDriving) {
            for (int i = 0; i < totalCars; ++i) {
                if (abs(carsArray[i].x - cj.x) <= 1 && abs(carsArray[i].y - cj.y) <= 1) {
                    isDriving = true;
                    currentCarIndex = i;
                    cj.x = carsArray[i].x;
                    cj.y = carsArray[i].y;
                    cj.symbol = 'C';
                    break;
                }
            }
        }
        else {
            isDriving = false;
            cj.symbol = '>';

            if (worldMap.grid[cj.y][cj.x - 1] == ' ') { cj.x--; cj.symbol = '<'; }
            else if (worldMap.grid[cj.y][cj.x + 1] == ' ') { cj.x++; cj.symbol = '>'; }
            else if (worldMap.grid[cj.y - 1][cj.x] == ' ') { cj.y--; cj.symbol = '^'; }
            else if (worldMap.grid[cj.y + 1][cj.x] == ' ') { cj.y++; cj.symbol = 'v'; }

            currentCarIndex = -1;
        }
        Sleep(200);
        return;
    }

    int nextX = cj.x;
    int nextY = cj.y;

	// Handle movement input
    if (GetAsyncKeyState(VK_UP) & 0x8000) { nextY--; if (!isDriving) cj.symbol = '^'; }
    else if (GetAsyncKeyState(VK_DOWN) & 0x8000) { nextY++; if (!isDriving) cj.symbol = 'v'; }
    else if (GetAsyncKeyState(VK_LEFT) & 0x8000) { nextX--; if (!isDriving) cj.symbol = '<'; }
    else if (GetAsyncKeyState(VK_RIGHT) & 0x8000) { nextX++; if (!isDriving) cj.symbol = '>'; }

    // Handle attack input
    if (!isDriving && (GetAsyncKeyState(VK_SPACE) & 0x8000)) {
        for (int i = 0; i < totalPeds; ++i) {
            if (!pedsArray[i].isDead && abs(pedsArray[i].x - cj.x) <= 1 && abs(pedsArray[i].y - cj.y) <= 1) {
				
				//Reducir salud del peatón
                pedsArray[i].health -= cj.attack;
                if (pedsArray[i].health <= 0) {
					// Peatón muere, dinero
                    pedsArray[i].isDead = true;
                    pedsArray[i].symbol = '$';
                    worldMap.grid[pedsArray[i].y][pedsArray[i].x] = '$';
				}
                else if (pedsArray[i].isAggressive) {
					// Peatón agresivo es provocado
                    pedsArray[i].isProvoked = true;
                }
            }
        }
        Sleep(150);
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
        else {
            currentState = GameState::GAME_OVER;
            timerCount = 0;
            system("cls");
            return;
        }
    }

    if (isDriving) {
        if (nextTile == ' ') {
            worldMap.grid[cj.y][cj.x] = ' ';
            cj.x = nextX;
            cj.y = nextY;
            carsArray[currentCarIndex].x = nextX;
            carsArray[currentCarIndex].y = nextY;
            worldMap.grid[cj.y][cj.x] = 'C';
        }
    }
    else {
        if (nextTile == ' ' || nextTile == '$') {
            cj.x = nextX;
            cj.y = nextY;

            if (nextTile == '$') {
                for (int i = 0; i < totalPeds; ++i) {
                    if (pedsArray[i].isDead && pedsArray[i].x == cj.x && pedsArray[i].y == cj.y) {
                        cj.money += 1 + rand() % pedsArray[i].maxMoneyDrop;
                        worldMap.grid[cj.y][cj.x] = ' ';

						// Respawn the pedestrian in a new valid location
                        pedsArray[i].isDead = false;
                        pedsArray[i].symbol = 'P';
                        pedsArray[i].isAggressive = (rand() % 2 == 0);
						pedsArray[i].isProvoked = false;
						pedsArray[i].attackCooldown = 0;

						pedsArray[i].health = (pedsArray[i].islandMinX == 1) ? 100 : 150;

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

    SetCursorPosition(0, 0);
    cout << "--- GTA: ENTI City Playing ---" << endl;
    if (isDriving) {
        cout << "[ DRIVING CAR ] Use 'E' key to exit vehicle!    " << endl;
    }
    else {
        cout << "HP: " << cj.health << " | Money: $" << cj.money << " | Toll 1: $" << lsToll << "    " << endl;
    }
    cout << "-------------------------------" << endl;
    worldMap.Render(cj, viewWidth, viewHeight);
    cout << "-------------------------------" << endl;
    Sleep(60);
}

// Update AI pedestrians' movement
void Game::UpdateAI() {
    for (int i = 0; i < totalPeds; ++i) {
        if (pedsArray[i].isDead) continue;

		bool isNearCJ = (abs(pedsArray[i].x - cj.x) <= 1 && abs(pedsArray[i].y - cj.y) <= 1);

        //Mecánica de contraataque
        if (pedsArray[i].isProvoked && isNearCJ && !isDriving) {
            if (pedsArray[i].attackCooldown <= 0) {
                cj.health -= pedsArray[i].attack;
                pedsArray[i].attackCooldown = 16;

                // Game Over
                if (cj.health <= 0) {
                    currentState = GameState::GAME_OVER;
                    timerCount = 0;
                    system("cls");
                    return;
                }
            }else {
                pedsArray[i].attackCooldown--;
            }
            continue;
        }

        if (isNearCJ) continue;
        
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

// Process game over screen and auto-exit after a delay
void Game::ProcessGameOver() {
    SetCursorPosition(0, 0);
    cout << "========================================" << endl;
    cout << "               GAME OVER                " << endl;
    cout << "========================================" << endl;
    cout << "\n   [POLICE] You were arrested!        " << endl;
    cout << "   Reason: Trying to cross toll without money!" << endl;
    cout << "\n   The game will close automatically... " << endl;
    cout << "========================================" << endl;

    timerCount++;
    if (timerCount >= 80) {
        isGameRunning = false;
    }
    Sleep(60);
}

void Game::Run() {
    system("cls");
    while (isGameRunning) {
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) isGameRunning = false;

        if (currentState == GameState::INIT) {
            ProcessInit();
        }
        else if (currentState == GameState::MENU) {
            ProcessMenu();
        }
        else if (currentState == GameState::PLAYING) {
            ProcessPlaying();
            UpdateAI();
        }
        else if (currentState == GameState::GAME_OVER) {
            ProcessGameOver();
        }
    }
}

void Game::Cleanup() {
    system("cls");
    delete[] pedsArray;
	delete[] carsArray;
    worldMap.Destroy();
    cout << "\n[System] Exiting game..." << endl;
    cout << "[System] Memory cleaned successfully! No leaks!" << endl;
}