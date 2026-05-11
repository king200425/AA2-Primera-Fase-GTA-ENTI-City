#include"GameMap.h"

using std::cout;
using std::endl;

void GameMap::Initialize(int w, int h) {
    width = w;
    height = h;
    grid = new char* [height];

    for (int i = 0; i < height; ++i) {
        grid[i] = new char[width];
        for (int j = 0; j < width; ++j) {
            if (i == 0 || i == height - 1 || j == 0 || j == width - 1) {
                grid[i][j] = 'X';
            }
            else if (j == width / 3 || j == (width * 2) / 3) {
                if (i != height / 2) {
                    grid[i][j] = 'X';
                }
                else {
                    grid[i][j] = ' ';
                }
            }
            else {
                grid[i][j] = ' ';
            }
        }
    }
}

void GameMap::Destroy() {
    for (int i = 0; i < height; ++i) {
        delete[] grid[i];
    }
    delete[] grid;
}

void GameMap::Render(const Player& player, int viewWidth, int viewHeight) {
    int startY = player.y - (viewHeight / 2);
    int endY = player.y + (viewHeight / 2);
    int startX = player.x - (viewWidth / 2);
    int endX = player.x + (viewWidth / 2);

    for (int i = startY; i <= endY; ++i) {
        for (int j = startX; j <= endX; ++j) {
            if (i < 0 || i >= height || j < 0 || j >= width) {
                cout << ' ';
            }
            else if (i == player.y && j == player.x) {
                cout << player.symbol;
            }
            else {
                cout << grid[i][j];
            }
        }
        cout << endl;
    }
}