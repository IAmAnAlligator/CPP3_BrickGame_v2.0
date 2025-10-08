#ifndef TETRIS_H
#define TETRIS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Размеры игрового поля
#define FIELD_WIDTH 12
#define FIELD_HEIGHT 22

// Размер фигуры (всегда 4x4)
#define FIGURE_SIZE 4

// Путь к файлу для сохранения счета
#define SCORE_FILE "score.txt"

// Путь к музыкальному файлу
#define AUDIO_FILE "../audio/demo.mp3"

// Отступы для отрисовки в интерфейсе
#define FIELD_OFFSET_X (FIELD_WIDTH * 2 + 10)
#define FIGURE_OFFSET_X (FIELD_OFFSET_X + 15)

// Состояние игры
typedef enum {
  START,      // Начальный экран
  PLAY,       // Игра идет
  PAUSE,      // Игра на паузе
  GAME_OVER,  // Игра окончена
  EXIT        // Выход из игры
} GameState;

// Действия пользователя
typedef enum {
  START_BUTTON = ' ',        // Старт игры
  PAUSE_BUTTON = 'p',        // Пауза
  TERMINATE_BUTTON = 'q',    // Завершение игры
  LEFT_BUTTON = KEY_LEFT,    // Влево
  RIGHT_BUTTON = KEY_RIGHT,  // Вправо
  UP_BUTTON = KEY_UP,        // Поворот фигуры
  DOWN_BUTTON = KEY_DOWN,    // Ускорение падения
  ACTION_BUTTON = 's'        // Хард-дроп
} UserAction_t;

// Основная структура, содержащая все данные состояния игры
typedef struct {
  int field[FIELD_HEIGHT][FIELD_WIDTH];          // Игровое поле
  int current_figure[FIGURE_SIZE][FIGURE_SIZE];  // Текущая фигура
  int next_figure[FIGURE_SIZE][FIGURE_SIZE];  // Следующая фигура
  int figure_x;          // Позиция фигуры по x
  int figure_y;          // Позиция фигуры по y
  int score;             // Текущий счет
  bool is_figure_fixed;  // Фигура зафиксирована
  int high_score;        // Лучший результат
  int level;             // Текущий уровень
  int speed;             // Скорость падения
  bool pause;            // Флаг паузы
  bool mute;             // Флаг отключения звука
  GameState state;       // Текущее состояние игры
} TetrisState;

// Основной игровой цикл — обновление состояния
void updateCurrentState(TetrisState *state);

// Инициализация новой игры
void init_game(TetrisState *state);

// Отрисовка игрового поля
void draw_game(TetrisState *state);

// Отрисовка следующей фигуры
void draw_next_figure(TetrisState *state);

// Генерация новой фигуры
void generate_next_figure(TetrisState *state);

// Спавн фигуры на поле
bool spawn_current_figure(TetrisState *state);

// Движение фигуры влево или вправо
void move_figure_horizontal(TetrisState *state, int dx);

// Поворот текущей фигуры
void rotate_figure(TetrisState *state);

// Проверка заполненных линий
void check_lines(TetrisState *state);

// Сдвиг фигуры вниз на 1
bool move_figure_down(TetrisState *state);

// Резкое падение фигуры вниз
void hard_drop(TetrisState *state);

// Фиксация фигуры на поле
void fix_figure(TetrisState *state);

// Сохранение рекорда
void save_high_score(TetrisState *state);

// Загрузка рекорда
void load_high_score(TetrisState *state);

// Получение случайной позиции по X для появления фигуры
int get_random_start_position(int figure_width);

// Получение ширины фигуры
int get_figure_width(int figure[FIGURE_SIZE][FIGURE_SIZE]);

// Обработка пользовательского ввода
void userInput(TetrisState *state, int ch);

// Обработка падения фигуры и повышение уровня
void move_and_level_check(TetrisState *state, int *timer);

// Проверка столкновений
bool check_collision(const TetrisState *state);

// Копирование фигуры
void copy_figure(int dest[FIGURE_SIZE][FIGURE_SIZE],
                 int src[FIGURE_SIZE][FIGURE_SIZE]);

// Воспроизведение музыки
int play_music(Mix_Music **music, const char *music_path);

// Остановка музыки
int stop_music();

// Пауза музыки
int pause_music();

// Продолжение воспроизведения музыки
int resume_music();

// Рисование рамки
void draw_frame(int start_y, int start_x, int height, int width);

// Рисование горизонтальной линии
void draw_line(int start_y, int start_x, int width);

#endif  // TETRIS_H
