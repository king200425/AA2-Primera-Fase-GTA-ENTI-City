#include"ConsoleUtils.h"

void SetCursorPosition(int x, int y) {
	static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	std::cout.flush();
	COORD coord = { (SHORT)x, (SHORT)y };
	SetConsoleCursorPosition(hOut, coord);
}

void DrawMainMenu() {
    SetCursorPosition(0, 0);
    std::cout << "========================================" << std::endl;
    std::cout << "       GRAND THEFT AUTO: ENTI CITY      " << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "\n\n";
    std::cout << "          Press [ENTER] to Start        " << std::endl;
    std::cout << "\n\n";
    std::cout << " Controls:" << std::endl;
    std::cout << " Arrows -> Move" << std::endl;
    std::cout << " Space  -> Attack" << std::endl;
    std::cout << " ESC    -> Exit" << std::endl;
    std::cout << "========================================" << std::endl;
}