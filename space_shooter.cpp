#include <iostream>
#include <vector>
#include <ncurses.h>
#include <unistd.h>
#include <cstdlib>
#include <ctime>

using namespace std;

const int WIDTH = 40;
const int HEIGHT = 20;

struct Bullet {
    int x, y;
};

struct Enemy {
    int x, y;
};

class Game {
public:
    Game() : playerX(WIDTH / 2), playerY(HEIGHT - 1), score(0) {
        srand(time(0));
        spawnEnemy();
    }

    void run() {
        initscr(); // Start ncurses mode
        cbreak(); // Disable line buffering
        noecho(); // Don't echo input characters
        keypad(stdscr, TRUE); // Enable special keys
        timeout(100); // Set a timeout for getch()

        while (true) {
            clear();
            drawBorders();
            drawPlayer();
            drawBullets();
            drawEnemies();
            displayScore();

            handleInput();
            updateBullets();
            updateEnemies();

            if (checkCollision()) {
                break; // End the game if there is a collision
            }

            usleep(100000); // Sleep to control game speed
        }

        endwin(); // End ncurses mode
    }

private:
    int playerX, playerY;
    vector<Bullet> bullets;
    vector<Enemy> enemies;
    int score;

    void drawBorders() {
        for (int i = 0; i < WIDTH + 2; i++) {
            mvaddch(0, i, '#');
            mvaddch(HEIGHT + 1, i, '#');
        }
        for (int i = 1; i <= HEIGHT; i++) {
            mvaddch(i, 0, '#');
            mvaddch(i, WIDTH + 1, '#');
        }
    }

    void drawPlayer() {
        mvprintw(playerY, playerX, "^");
    }

    void drawBullets() {
        for (const auto& bullet : bullets) {
            mvaddch(bullet.y, bullet.x, '|');
        }
    }

    void drawEnemies() {
        for (const auto& enemy : enemies) {
            mvaddch(enemy.y, enemy.x, 'X');
        }
    }

    void displayScore() {
        mvprintw(0, WIDTH / 2 - 5, "Score: %d", score);
    }

    void handleInput() {
        int ch = getch();
        switch (ch) {
            case KEY_LEFT:
                if (playerX > 1) playerX--;
                break;
            case KEY_RIGHT:
                if (playerX < WIDTH - 2) playerX++;
                break;
            case ' ':
                shoot();
                break;
            case 'q':
                endwin();
                exit(0);
                break;
        }
    }

    void shoot() {
        bullets.push_back({playerX, playerY - 1});
    }

    void updateBullets() {
        for (auto it = bullets.begin(); it != bullets.end();) {
            it->y--;
            if (it->y < 1) {
                it = bullets.erase(it); // Remove bullet if it goes off-screen
            } else {
                ++it;
            }
        }
    }

    void spawnEnemy() {
        int enemyX = rand() % (WIDTH - 2) + 1; // Random x position
        enemies.push_back({enemyX, 1});
    }

    void updateEnemies() {
        for (auto it = enemies.begin(); it != enemies.end();) {
            it->y++;
            if (it->y > HEIGHT) {
                it = enemies.erase(it); // Remove enemy if it goes off-screen
                score++; // Increase score for each enemy that goes off-screen
                spawnEnemy(); // Spawn a new enemy
            } else {
                ++it;
            }
        }
    }

    bool checkCollision() { 
       for (auto enemyIt = enemies.begin(); enemyIt != enemies.end();) { 
           bool hit = false; 
           for (auto bulletIt = bullets.begin(); bulletIt != bullets.end();) { 
               if (bulletIt->x == enemyIt->x && bulletIt->y == enemyIt->y) { 
                   bulletIt = bullets.erase(bulletIt); 
                   hit = true; 
                   score += 10; 
                   break; 
               } else { 
                   ++bulletIt; 
               } 
           } 

           if (hit) { 
               enemyIt = enemies.erase(enemyIt); 
           } else { 
               ++enemyIt; 
           } 

           if (enemyIt != enemies.end() && enemyIt->y == playerY && enemyIt->x == playerX) { 
               return true; 
           } 
       } 

       return false; 
   }
};

int main() { 
   Game game;
   game.run();
   
   return 0;
}