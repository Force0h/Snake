#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "ncurses.h"

#define DELAY 100000
#define TIMEOUT 10
#define X_MAX 50
#define Y_MAX 20

typedef struct {
    int x;
    int y;
} COORDINATE;

typedef struct {
    int max;
    int size;
    int tail_last_x;
    int tail_last_y;
    COORDINATE *head;
    COORDINATE *tail;
    COORDINATE *body;
} SNAKE;

void draw_screen(int score) {
    for (int y = 0; y < Y_MAX; y++) {
        for (int x = 0; x < X_MAX; x++) {
            mvaddch(y, x, '.');
        }
    }
    for (int x = 0; x < X_MAX; x++) {
        mvaddch(Y_MAX, x, '-');
    }
    mvprintw(Y_MAX + 1, 0, "Score: %i", score);
    mvaddstr(Y_MAX + 2, 0, "Press 'x' to quit");
    mvaddstr(Y_MAX + 3, 0, "Press wasd to move");
}

void draw_snake(SNAKE *snake) {
    mvaddch(snake->head->y, snake->head->x, 'O');
    for (int i = 1; i < snake->size; i++) {
        mvaddch(snake->body[i].y, snake->body[i].x, 'o');
    }
}

COORDINATE create_coordinate(int x, int y) {
    COORDINATE coordinate;
    coordinate.x = x;
    coordinate.y = y;
    return coordinate;
}

int coordinate_in_snake(SNAKE *snake, int x, int y) {
    for (int i = 0; i < snake->size; i++) {
        return snake->body[i].x == x && snake->body[i].y == y;
    }
    return 0;
}

COORDINATE create_food(SNAKE *snake) {
    COORDINATE coordinate;

    do {
        coordinate.x = rand() % X_MAX;
        coordinate.y = rand() % Y_MAX;
    } while (coordinate_in_snake(snake, coordinate.x, coordinate.y));

    return coordinate;
}

void init_snake(SNAKE *snake) {
    snake->max = Y_MAX * X_MAX;
    snake->body = malloc(snake->max * sizeof(COORDINATE));
    snake->body[0] = create_coordinate(rand() % X_MAX, rand() % Y_MAX);
    snake->head = &snake->body[0];
    snake->tail = snake->head;
    snake->size = 1;
}

int move_snake(SNAKE *snake, int xDir, int yDir) {
    if (xDir == 0 && yDir == 0) {
        return 0;
    }

    snake->tail_last_x = snake->tail->x;
    snake->tail_last_y = snake->tail->y;

    int new_x = snake->head->x + xDir;
    int new_y = snake->head->y + yDir;

    if (new_x < 0 || new_x == X_MAX || new_y < 0 || new_y == Y_MAX) {
        mvaddstr(Y_MAX + 4, 0, "GAME OVER!");
        return 1;
    }

    int temp_x = snake->head->x;
    int temp_y = snake->head->y;

    snake->head->x = new_x;
    snake->head->y = new_y;

    for (int i = 1; i < snake->size; i++) {
        new_x = temp_x;
        new_y = temp_y;
        temp_x = snake->body[i].x;
        temp_y = snake->body[i].y;
        snake->body[i].x = new_x;
        snake->body[i].y = new_y;

    }

    for (int i = 0; i < snake->size; i++) {
        for (int j = i + 1; j < snake->size; j++) {
            if (snake->body[i].x == snake->body[j].x && snake->body[i].y == snake->body[j].y) {
                mvaddstr(Y_MAX + 4, 0, "GAME OVER!");
                return 1;
            }
        }
    }

    return 0;
}

void increase_snake(SNAKE *snake) {
    snake->size++;
    snake->body[snake->size - 1] = create_coordinate(snake->tail_last_x, snake->tail_last_y);
    snake->tail = &snake->body[snake->size - 1];
}

void handle_input(int input, int *xDir, int *yDir) {
	if ((input == 'w' || input == KEY_UP) && *yDir != 1) {
		*xDir = 0;
        *yDir = -1;
	} else if ((input == 'a' || input == KEY_LEFT) && *xDir != 1) {
		*xDir = -1;
        *yDir = 0;
	} else if ((input == 's' || input == KEY_DOWN) && *yDir != -1) {
		*xDir = 0;
        *yDir = 1;
	} else if ((input == 'd' || input == KEY_RIGHT) && *xDir != -1) {
		*xDir = 1;
        *yDir = 0;
	}
}

int main() {
    int input = 0;
    int score = 0;
    int xDir = 0;
    int yDir = 0;
    int gameover = 0;
    SNAKE *snake = malloc(sizeof(SNAKE));
    COORDINATE food;
    
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    timeout(TIMEOUT);
    
    init_snake(snake);
    food = create_food(snake);

    while (input != 'x' && gameover != 1)  {
        input = getch();
        clear();

        handle_input(input, &xDir, &yDir);

        gameover = move_snake(snake, xDir, yDir);

        if (coordinate_in_snake(snake, food.x, food.y)) {
            increase_snake(snake);
            food = create_food(snake);
            score++;
        }

        draw_screen(score);
        draw_snake(snake);
        mvaddch(food.y, food.x, '@');

        refresh();
        usleep(DELAY);
    }

    if (input != 'x') {
        sleep(3);
    }

    free(snake->body);
    free(snake);

    nocbreak();
    endwin();

    return 0;
}
