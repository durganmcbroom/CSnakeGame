#include <stdio.h>
#include "stdbool.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ncurses.h>
#include <stdlib.h>

struct Position {
    int x;
    int y;
    bool render;
} Position;

struct KeyMapping {
    char key;
    int output;
} KeyMapping;

const int GAME_WIDTH = 10;

const int GAME_HEIGHT = 10;

const struct KeyMapping VALID_KEYS[] = {{'w', 1},
                                        {'s', -1},
                                        {'a', -3},
                                        {'d', 3}};
const int VALID_KEY_NUM = 4;

void renderGame(struct Position positions[], int size);

void incrementPositions(struct Position *positions, int size, int rotation);

bool isValidKey(char key);

int retrieveKey(char key);

bool handleApples(struct Position apples[], int appleSize, struct Position start);

bool isValidMove(struct Position positions[], int size);

void preRender(const struct Position *snakePosition, const struct Position *applePositions, int snakeSize, int apples);

int main() {
    struct Position snakePosition[GAME_WIDTH * GAME_HEIGHT] = {{1, 1, true},
                                                               {1, 2, true}};
    struct Position applePositions[GAME_WIDTH * GAME_HEIGHT] = {{GAME_WIDTH - 1, GAME_HEIGHT - 1, true}};

    int snakeSize = 2;
    int apples = 1;

    time_t startTime = time(NULL);

    bool isGameRunning = true;

    initscr();

    while (isGameRunning) {
        char key;
        scanf("%c", &key);

        if (isValidKey(key)) {
            incrementPositions(snakePosition, snakeSize, retrieveKey(key));


            if (handleApples(applePositions, apples, *snakePosition)) {
                snakeSize++;
                srand(time(NULL));

                bool placedApple = false;

                while (!placedApple) {
                    struct Position apple = (struct Position) {rand() % GAME_WIDTH, rand() % GAME_HEIGHT, true};

                    bool correctIteration = true;

                    for (int i = 0; i < snakeSize; ++i) {
                        struct Position segment = *(snakePosition + i); //Just because i wanted to

                        if (apple.x == segment.x && apple.y == segment.y) {
                            correctIteration = false;
                        }
                    }

                    if (correctIteration) {
                        applePositions[apples] = apple;
                        placedApple = true;
                    }
                }

                preRender(snakePosition, applePositions, snakeSize, apples);
                apples++;
            } else if (!isValidMove(snakePosition, snakeSize)) {
                isGameRunning = false;

                printw("\n Game finished! | Time: %lo seconds | Snake Size: %i | Apples Eaten: %i ",
                       time(NULL) - startTime, snakeSize, apples - 1);
            } else {
                preRender(snakePosition, applePositions, snakeSize, apples);

            }
            refresh();
        }
    }

    endwin();

    printf("Game finished! | Time: %lo seconds | Snake Size: %i | Apples Eaten: %i \n",
           time(NULL) - startTime, snakeSize, apples - 1);

    return 0;
}

void preRender(const struct Position *snakePosition, const struct Position *applePositions, int snakeSize, int apples) {
    struct Position outCome[snakeSize + apples];

    for (int i = 0; i < snakeSize + apples; i++) {
        if (i < snakeSize) outCome[i] = snakePosition[i];
        else outCome[i] = applePositions[i % snakeSize];
    }
    move(0, 0);
    renderGame(outCome, snakeSize + apples);
}

bool isValidMove(struct Position positions[], int size) {
    struct Position start = positions[0];

    if ((start.x < 0 || start.x >= GAME_WIDTH) || (start.y < 0 || start.y >= GAME_HEIGHT)) return false;
    for (int i = 1; i < size; i++) {
        struct Position current = positions[i];

        if (current.x == start.x && current.y == start.y) return false;
    }

    return true;
}

bool handleApples(struct Position apples[], int appleSize, struct Position start) {
    for (int i = 0; i < appleSize; i++) {
        struct Position apple = apples[i];

        if (apple.x == start.x && apple.y == start.y && apple.render) {
            apples[i] = (struct Position) {apple.x, apple.y, false};
            return true;
        }
    }
    return false;
}

bool isValidKey(char key) {
    for (int i = 0; i < VALID_KEY_NUM; i++) {
        struct KeyMapping mapping = VALID_KEYS[i];
        if (mapping.key == key) return true;
    }
    return false;
}

int retrieveKey(char key) {
    for (int i = 0; i < VALID_KEY_NUM; i++) {
        struct KeyMapping mapping = VALID_KEYS[i];
        if (mapping.key == key) return mapping.output;
    }
    return 0;
}

/*
 *  1 = up
 * -1 = down
 * ---- Greater than 1 Odd ----
 *  3 = right
 * -3 = left
 * 
 *
 */
void incrementPositions(struct Position *position, int size, int rot) {

    int initialX = (position)[0].x;
    int initialY = (position)[0].y;
    position[0] = (int) abs(rot) > 1 ?
                  (struct Position) {initialX + (rot % 2), initialY, true} :
                  (struct Position) {initialX, initialY - rot, true};


    for (int i = size - 1; i > 0; i--) {
        if (i > 1) position[i] = position[i - 1];
        else position[i] = (struct Position) {initialX, initialY, true};
    }
}

void renderGame(struct Position positions[], int size) {
    bool gameBoard[GAME_WIDTH][GAME_HEIGHT] = {{false}};

    for (int i = 0; i < size; i++) {
        struct Position position = positions[i];

        if ((position.x < GAME_WIDTH && position.y < GAME_HEIGHT) &&
            (position.x > -1 && position.y > -1) && position.render)
            gameBoard[position.x][position.y] = true;
    }

    for (int i = 0; i < GAME_WIDTH * GAME_HEIGHT; i++) {
        int height = i / GAME_WIDTH;
        int width = i - (height * GAME_WIDTH);

        bool active = gameBoard[width][height];

        printw(active ? "[0]" : "---");
        if (width == GAME_WIDTH - 1) printw("\n");
    }
}
