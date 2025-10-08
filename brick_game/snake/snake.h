#ifndef SNAKE_H
#define SNAKE_H

#ifdef USE_NCURSES
#include <ncurses.h>  // Подключается только при необходимости
#endif

#include <stdio.h>  // Стандартная библиотека ввода-вывода
#include <unistd.h>  // POSIX API

#include <chrono>    // Работа со временем
#include <cstdlib>   // Стандартные функции C
#include <cstring>   // Работа со строками
#include <ctime>     // Работа с датой и временем
#include <fstream>   // Работа с файлами
#include <iostream>  // Стандартный ввод-вывод C++
#include <thread>    // Потоки

namespace s21 {

// Константы для игры
#define HEIGHT 20
#define WIDTH 10
#define ESCAPE 27
#define ENTER_KEY 10
#define KEY_SPACE ' '
#define FINAL_LEVEL 10
#define FINAL_LENGTH 200
#define MAX_DELAY 800000
#define MIN_DELAY 150000
#define PACE_DELAY 50000
#define FILE_NAME "snake_score.txt"

// Действия пользователя
typedef enum {
  Start,
  Pause,
  Terminate,
  Left,
  Right,
  Up,
  Down,
  Action,
  None
} UserAction_t;

// Состояния игры
typedef enum { PAUSE, PLAY, START, EXIT, GAME_OVER, VICTORY } GameState;

// Информация об игре
typedef struct {
  int field[HEIGHT + 2][WIDTH + 2];
  int **next;
  int score;
  int high_score;
  int level;
  int speed;
  int pause;
} GameInfo_t;

// Координаты
typedef struct {
  int x;
  int y;
} Position;

}  // namespace s21

#endif  // SNAKE_H
