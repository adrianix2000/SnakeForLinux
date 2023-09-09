#include <iostream>

class Snake {
    public:
        short SnakeX;
        short SnakeY;
        int Length;
        short HistoryOfX[10000];
        short HistoryOfY[10000];
        char Direction;

        Snake();
        bool CheckAppleColision(short appleX, short appleY);

};