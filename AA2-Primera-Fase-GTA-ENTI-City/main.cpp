#include<iostream>
#include<fstream>

using std::endl;
using std::cout;
using std::ifstream;

int main() {
	cout << "--- GTA: ENTI City Starting ---" << endl;

	ifstream configFile("config.txt");

	int mapWidth = 0, mapHeight = 0;
	int lsPeatones, lsPeaje, lsDineroMax;
	int sfPeatones, sfPeaje, sfDineroMax;

	if (configFile.is_open()) {
		char delimiter;
		// Read the configuration values from the file
		configFile >> mapWidth >> delimiter >> mapHeight >> delimiter;
		// Read the rest of the configuration values
		configFile >> lsPeatones >> delimiter >> lsPeaje >> delimiter >> lsDineroMax >> delimiter;
		// Read the rest of the configuration values
		configFile >> sfPeatones >> delimiter >> sfPeaje >> delimiter >> sfDineroMax >> delimiter;
		configFile.close();
	}
	else {
		cout << "[ERROR] Failed to load config.txt! " << endl;
		return -1;
	}

	// 1. Create a dynamic array of pointers to represent the rows of the map
	char** map = new char* [mapHeight];
	for (int i = 0; i < mapHeight; ++i) {
		map[i] = new char[mapWidth];
		for (int j = 0; j < mapWidth; ++j) {
			map[i][j] = ' ';
		}
	}

	// 2. Create a dynamic 2D array (matrix) to represent the map
	for (int i = 0; i < mapHeight; ++i) {
		for (int j = 0; j < mapWidth; ++j) {
			if (i == 0 || i == mapHeight - 1 || j == 0 || j == mapWidth - 1) {
				map[i][j] = 'X';
			}
			else if (j == mapWidth / 3 || j == (mapWidth * 2) / 3) {

				// 3. 在每道墙的正中间高度留一个“桥”(空位) / Dejar un hueco para el puente
				if (i != mapHeight / 2) {
					map[i][j] = 'X';
				}
			}
		}
	}
	// 4. Print the map to the console for verification
	cout << "\n--- MAP PREVIEW / VISTA PREVIA DEL MAPA ---" << endl;
	for (int i = 0; i < mapHeight; ++i) {
		for (int j = 0; j < mapWidth; ++j) {
			cout << map[i][j];
		}
		cout << endl;
	}
	cout << "-------------------------------------------" << endl;

	// Clean up the dynamically allocated memory for the map
	for (int i = 0; i < mapHeight; ++i) {
		delete[] map[i];
	}
	delete[] map;

	cout << "\n[System] Map memory cleaned successfully. No leaks!" << endl;
}