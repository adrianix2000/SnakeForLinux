#include "Snake.hpp"
#include <iostream>

Snake::Snake() {
    SnakeX=5;
    SnakeY=5;
    Length=5;
    Direction='p';
}

bool Snake::CheckAppleColision(short appleX, short appleY) {
    return SnakeX == appleX && SnakeY == appleY;
}