#include <iostream>
#include <new>
#include <unistd.h>
#include <curses.h>
#include <stdlib.h>
#include <stdio.h>
#include <utility>
#include <vector>
#include <random>
#include <algorithm>

#include "src/Snake.cpp"

//#define KOLUMNY 30
//#define WIERSZE 30

char znak = 219;

int kbhit(void);

class SessionSettings {
    public:
        short COLUMNS = 20;
        short ROWS = 20;

        unsigned short SPEED = 6;

        char SnakeCharacter = 219;
        char AppleCharacter = 219;

    private:
        SessionSettings() {}
        SessionSettings(SessionSettings&);

    public:
        static SessionSettings & GetSessionSettings() {
            static SessionSettings settings;
            return settings;
        }
};

class RandomGenerator {
    private:
        static std::random_device device;
        RandomGenerator() {}
    
    public:
        static short generateNumber(short Min, short Max) {
            if(Min > Max)
                std::swap(Min, Max);

            std:std::uniform_int_distribution<short> dist(Min, Max);
            return dist(RandomGenerator::device);
        }

        
};


class Apple {
    public:
        short AppleX;
        short AppleY;

        Apple(char **logicalField) {
            DrawDirection(logicalField);
        }

        void DrawDirection(char **logicalField) {
            do {
                AppleX=rand()%SessionSettings::GetSessionSettings().COLUMNS;
                AppleY=rand()%SessionSettings::GetSessionSettings().ROWS; //rand()%SessionSettings::GetSessionSettings().ROWS;
            }while(logicalField[AppleX][AppleY] != 'p');
        }
};

class Session {
    private:
        short COLUMNS;
        short ROWS;
        int SPEED;
        char SnakeCharacter;
        char AppleCharacter;

        WINDOW*  window;

        Snake *snake;
        Apple *apple;

        char **logicalField;

        int HowMany;

    public:
        Session() {
            inicialization();
            OnRefresh();
        }

        ~Session() {
            delwin(window);
            delete snake;
            delete apple;
            delete [] logicalField;
        }

        void inicialization() {

            COLUMNS = SessionSettings::GetSessionSettings().COLUMNS;
            ROWS = SessionSettings::GetSessionSettings().ROWS;

            SPEED = SessionSettings::GetSessionSettings().SPEED*10000;

            SnakeCharacter = SessionSettings::GetSessionSettings().SnakeCharacter;
            AppleCharacter = SessionSettings::GetSessionSettings().AppleCharacter;

            HowMany = 0;

            logicalField = new char*[COLUMNS];
             for(int i=0; i<COLUMNS; i++){
                logicalField[i] = new char[ROWS];
                for(int j=0; j<ROWS; j++){
                     logicalField[i][j] = 'p';
                }
             }
             snake = new Snake();
             logicalField[snake->SnakeX][snake->SnakeY] = 'w';
             apple = new Apple(getLogicalField());
             logicalField[apple->AppleX][apple->AppleY] = 'j';
            
             window = newwin(ROWS + 2, COLUMNS*2 + 2, LINES/2 - ROWS/2, COLS/2 - COLUMNS);
             box(window, '|', '-');
             touchwin(window);
        }

        void OnRefresh() {
            while(true) {
                
                HowMany++;

                snake->HistoryOfX[HowMany]=snake->SnakeX;
                snake->HistoryOfY[HowMany]=snake->SnakeY;

                logicalField[snake->HistoryOfX[HowMany-snake->Length]][snake->HistoryOfY[HowMany-snake->Length]]='p';

                if(kbhit()){
                    int zn = getch();
                    switch(zn) {
                        case KEY_UP:
                            if(snake->Direction != 'd')
                                snake->Direction='g';
                        break;
                        case KEY_DOWN:
                            if(snake->Direction != 'g')
                                 snake->Direction='d';
                        break;
                        case KEY_RIGHT:
                            if(snake->Direction != 'l')
                                 snake->Direction='p';
                        break;
                        case KEY_LEFT:
                            if(snake->Direction != 'p')
                                snake->Direction='l';
                            break;
                        case 66:
                            break;
                    }
                }

                switch(snake->Direction) {
                    case 'g':
                        snake->SnakeY--;
                        if(snake->SnakeY == -1)
                            snake->SnakeY=ROWS-1;
                    break;
                    case 'd':
                        snake->SnakeY++;
                        if(snake->SnakeY == ROWS)
                            snake->SnakeY=0;
                    break;
                    case 'p':
                        snake->SnakeX++;
                        if(snake->SnakeX == COLUMNS)
                            snake->SnakeX=0;
                        break;
                    case 'l':
                        snake->SnakeX--;
                        if(snake->SnakeX == -1)
                            snake->SnakeX=COLUMNS-1;
                        break;
                }

                if(logicalField[snake->SnakeX][snake->SnakeY] == 'w'){
                    LoseEvent();
                    break;
                }

                logicalField[snake->SnakeX][snake->SnakeY] = 'w';
                
                DrawMap();

                AppleGrabEvent();

                logicalField[snake->SnakeX][snake->SnakeY]='w'; 
                

                wrefresh(window);
                usleep(SPEED);
            }

        }

        void AppleGrabEvent() {
            if(snake->CheckAppleColision(apple->AppleX, apple->AppleY)) {
                    snake->Length++;
                    apple->DrawDirection(logicalField);
                    logicalField[apple->AppleX][apple->AppleY]='j';
                }else {
                    //pole_logiczne[hx[ile-dlugosc]][hy[ile-dlugosc]]='p';
                }
        }

        void LoseEvent() {
            WINDOW* loseWindow = newwin(10, 30, LINES/2 - 5, COLS/2 - 15);
            box(loseWindow, '|', '-');
            touchwin(loseWindow);

            mvwprintw(loseWindow, 1, 1, "%s %d", "Przegrana! Dlugosc = ", snake->Length);
            wrefresh(loseWindow);
            sleep(3);
        }

        void DrawMap() {
            wattrset(window, COLOR_PAIR(4));
            mvwprintw(window, 0, COLUMNS/2+12, "%s %d", "current lenght:", COLS);
            wattrset(window, COLOR_PAIR(1));

            for(int i=0; i<ROWS; i++){
                for(int j=0; j<COLUMNS; j++){
                    if(logicalField[j][i]=='p') {
                        mvwprintw(window, i+1, j*2+1, "%s", " .");
                    }
                    if(logicalField[j][i]=='w') {                    
                        wattrset(window, COLOR_PAIR(2));
                        mvwprintw(window, i+1, j*2+1, "%c%c", SnakeCharacter, SnakeCharacter);
                        wattrset(window, COLOR_PAIR(1));
                    }
                    if(logicalField[j][i]=='j'){
                        wattrset(window, COLOR_PAIR(3));
                        mvwprintw(window, i+1, j*2+1, "%c%c", AppleCharacter, AppleCharacter);
                        wattrset(window, COLOR_PAIR(1));
                    }
                }
            }

        }

        char ** getLogicalField() {
            return logicalField;
        }
};

int main()
{
    srand(time(NULL));

    int terminal_cols, terminal_rows;
    getmaxyx(stdscr, terminal_rows, terminal_cols);

    SessionSettings::GetSessionSettings().COLUMNS = 25;
    SessionSettings::GetSessionSettings().ROWS = 25;


    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    scrollok(stdscr, TRUE);
    keypad(stdscr, TRUE);

    if(start_color() != OK){
        return 0;
    }

    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_GREEN);
    init_pair(3, COLOR_RED, COLOR_RED);
    init_pair(4, COLOR_WHITE, COLOR_BLACK);

    attrset(COLOR_PAIR(1));

    Session session;


    endwin();

    return 0;
}

int kbhit(void){
    int ch = getch();
    if (ch != ERR) {
        ungetch(ch);
        return 1;
    } else {
        return 0;
    }
}
