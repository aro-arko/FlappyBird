#include <graphics.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define BASE_HEIGHT 50
#define PIPE_WIDTH 80
#define PIPE_GAP 150
#define BIRD_SIZE 20
#define LEVEL_LENGTH 5000  // Distance to complete a level
#define SCORE_POSITION_X (SCREEN_WIDTH - 200)
#define SCORE_POSITION_Y 30
#define SCORE_FONT_SIZE 3 // Adjust the font size here

int birdY = SCREEN_HEIGHT / 2;
int birdSpeed = 0;
int gravity = 1;
int pipeX[2] = {SCREEN_WIDTH, SCREEN_WIDTH + (SCREEN_WIDTH / 2)};
int pipeHeight[2];
int score = 0;
int level = 1;
int pipeSpeed = 5;
int pipeGap = PIPE_GAP;
int distanceCovered = 0;
bool gameOver = false;

void drawBird() {
    setcolor(RED);
    setfillstyle(SOLID_FILL, RED);
    fillellipse(SCREEN_WIDTH / 4, birdY, BIRD_SIZE, BIRD_SIZE);
}

void drawPipe(int x, int height) {
    setcolor(GREEN);
    setfillstyle(SOLID_FILL, GREEN);
    bar(x, 0, x + PIPE_WIDTH, height);
    bar(x, height + pipeGap, x + PIPE_WIDTH, SCREEN_HEIGHT - BASE_HEIGHT);
}

void drawBase() {
    setcolor(BROWN);
    setfillstyle(SOLID_FILL, BROWN);
    bar(0, SCREEN_HEIGHT - BASE_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void updateGame() {
    birdY += birdSpeed;
    birdSpeed += gravity;
    distanceCovered += pipeSpeed;

    for (int i = 0; i < 2; i++) {
        pipeX[i] -= pipeSpeed;

        if (pipeX[i] + PIPE_WIDTH < 0) {
            pipeX[i] = SCREEN_WIDTH;
            pipeHeight[i] = rand() % (SCREEN_HEIGHT - pipeGap - BASE_HEIGHT - 50) + 50;
            score++; // Increment score for passing each pipe
        }
    }

    if (birdY >= SCREEN_HEIGHT - BASE_HEIGHT || birdY <= 0) {
        gameOver = true;
    }

    for (int i = 0; i < 2; i++) {
        if (SCREEN_WIDTH / 4 + BIRD_SIZE > pipeX[i] && SCREEN_WIDTH / 4 - BIRD_SIZE < pipeX[i] + PIPE_WIDTH) {
            if (birdY - BIRD_SIZE < pipeHeight[i] || birdY + BIRD_SIZE > pipeHeight[i] + pipeGap) {
                gameOver = true;
            }
        }
    }
}

void resetGame() {
    birdY = SCREEN_HEIGHT / 2;
    birdSpeed = 0;
    pipeX[0] = SCREEN_WIDTH;
    pipeX[1] = SCREEN_WIDTH + (SCREEN_WIDTH / 2);
    pipeHeight[0] = rand() % (SCREEN_HEIGHT - pipeGap - BASE_HEIGHT - 50) + 50;
    pipeHeight[1] = rand() % (SCREEN_HEIGHT - pipeGap - BASE_HEIGHT - 50) + 50;
    distanceCovered = 0;
    gameOver = false;
    score = 0; // Reset score
    cleardevice();
}

void showCompletionMessage() {
    char message[100];
    sprintf(message, "Congratulations! You've passed level %d.", level);
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
    outtextxy(SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2 - 50, message);
    outtextxy(SCREEN_WIDTH / 2 - 300, SCREEN_HEIGHT / 2, "Press R to restart from menu or Q to quit.");
}

void showMenu() {
    cleardevice();
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
    outtextxy(SCREEN_WIDTH / 2 - 100, 50, "Select Level");
    for (int i = 1; i <= 8; i++) {
        char levelText[10];
        sprintf(levelText, "Level %d", i);
        outtextxy(SCREEN_WIDTH / 2 - 50, 100 + i * 30, levelText);
    }
}

int selectLevel() {
    showMenu();
    int selectedLevel = 0;
    while (1) {
        if (kbhit()) {
            char ch = getch();
            if (ch >= '1' && ch <= '8') {
                selectedLevel = ch - '0';
                break;
            }
        }
    }
    return selectedLevel;
}

void adjustDifficulty(int level) {
    pipeSpeed = 5 + level;
    pipeGap = PIPE_GAP - level * 10;
}

void checkLevelCompletion() {
    if (distanceCovered >= LEVEL_LENGTH) {
        showCompletionMessage();
        while (1) {
            if (kbhit()) {
                char ch = getch();
                if (ch == 'r' || ch == 'R') {
                    level = selectLevel();
                    adjustDifficulty(level);
                    distanceCovered = 0;
                    resetGame(); // Reset game including score
                    return;
                } else if (ch == 'q' || ch == 'Q') {
                    closegraph();
                    exit(0);
                }
            }
            delay(100); // Delay to reduce CPU usage
        }
    }
}

int main() {
    int gd = DETECT, gm;
    initgraph(&gd, &gm, "");

    srand(time(0));
    pipeHeight[0] = rand() % (SCREEN_HEIGHT - pipeGap - BASE_HEIGHT - 50) + 50;
    pipeHeight[1] = rand() % (SCREEN_HEIGHT - pipeGap - BASE_HEIGHT - 50) + 50;

    while (1) {
        level = selectLevel();
        adjustDifficulty(level);

        while (!gameOver) {
            cleardevice();
            drawBird();
            drawPipe(pipeX[0], pipeHeight[0]);
            drawPipe(pipeX[1], pipeHeight[1]);
            drawBase();
            updateGame();
            checkLevelCompletion();

            // Display score
            settextstyle(DEFAULT_FONT, HORIZ_DIR, SCORE_FONT_SIZE);
            char scoreText[50];
            sprintf(scoreText, "Score: %d", score);
            outtextxy(SCORE_POSITION_X, SCORE_POSITION_Y, scoreText);

            if (kbhit()) {
                char ch = getch();
                if (ch == ' ') birdSpeed = -10;
            }

            delay(30);
        }

        // Game over loop
        while (gameOver) {
            settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
            char scoreText[50];
            sprintf(scoreText, "Score: %d", score);
            setcolor(WHITE);
            outtextxy(SCORE_POSITION_X, SCORE_POSITION_Y, scoreText);
            outtextxy(SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 50, "Game Over!");
            outtextxy(SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2, "Press R to restart or Q to quit.");
            if (kbhit()) {
                char ch = getch();
                if (ch == 'r' || ch == 'R') {
                    resetGame();
                    break;
                } else if (ch == 'q' || ch == 'Q') {
                    closegraph();
                    return 0;
                }
            }
            delay(100); // Delay to reduce CPU usage
        }
    }

    closegraph();
    return 0;
}
