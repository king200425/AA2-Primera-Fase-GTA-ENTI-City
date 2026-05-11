#include <iostream>
#include <cstdlib>
#include <ctime>
#include "Game.h"

using std::endl;
using std::cout;
using std::ifstream;


int main() {
    srand((unsigned int)time(NULL));

    Game myGame;

    if (!myGame.LoadConfigAndInit()) {
        std::cout << "[ERROR] Failed to load config.txt!" << std::endl;
        return -1;
    }

    myGame.Run();

    myGame.Cleanup();

}