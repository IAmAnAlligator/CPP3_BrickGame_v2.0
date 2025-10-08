#include "../../../brick_game/tetris/tetris.h"

// Главная функция программы
int main(void) {
  TetrisState state;  // Структура состояния игры

  // Инициализация ncurses
  initscr();
  noecho();  // Отключение отображения вводимых символов
  cbreak();  // Включение немедленного ввода
  keypad(stdscr, TRUE);  // Включение обработки функциональных клавиш
  timeout(80);  // Установка тайм-аута для функции getch()
  mousemask(0, NULL);  // Отключение обработки событий мыши
  curs_set(0);    // Скрывает курсор
  start_color();  // Инициализация цветового режима

  // Инициализация цветовых пар
  init_pair(1, COLOR_BLACK, COLOR_BLACK);
  init_pair(2, COLOR_GREEN, COLOR_BLACK);
  init_pair(3, COLOR_RED, COLOR_BLACK);
  init_pair(4, COLOR_BLUE, COLOR_BLACK);
  init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(6, COLOR_YELLOW, COLOR_BLACK);

  state.state = START;  // Установка начального состояния игры

  Mix_Music *music = NULL;  // Указатель на музыку

  play_music(&music, AUDIO_FILE);  // Воспроизведение музыки

  int timer = 0;  // Таймер для управления движением фигуры
  int ch;  // Переменная для хранения ввода пользователя
  while (state.state != EXIT) {  // Цикл игры
    ch = getch();  // Получение ввода от пользователя
    userInput(&state, ch);  // Обработка пользовательского ввода
    updateCurrentState(&state);  // Обновление текущего состояния игры
    move_and_level_check(&state, &timer);  // Проверка движения и уровня
  }
  endwin();  // Завершение работы ncurses

  stop_music();          // Остановка музыки
  Mix_FreeMusic(music);  // Освобождение ресурсов музыки
  Mix_CloseAudio();      // Закрытие аудио системы
  SDL_Quit();            // Завершение работы SDL

  return 0;  // Успешное завершение программы
}

// Функция для отрисовки игрового поля
void draw_game(TetrisState *state) {
  clear();  // Очистка экрана

  // Отрисовка игрового поля
  for (int i = 0; i < FIELD_HEIGHT; i++) {
    for (int j = 0; j < FIELD_WIDTH; j++) {
      if (state->field[i][j] == 1) {  // Если ячейка заполнена
        attron(COLOR_PAIR(1));  // Включение цветовой пары
        if (i == 0 || i == FIELD_HEIGHT - 1 || j == 0 || j == FIELD_WIDTH - 1) {
          attron(COLOR_PAIR(2));  // Установка цвета для границ
          mvprintw(i, j * 2, "[]");  // Отрисовка границы
          attroff(COLOR_PAIR(2));  // Отключение цветовой пары
        } else {
          mvprintw(i, j * 2, "X");  // Отрисовка заполненной ячейки
        }
        attroff(COLOR_PAIR(1));  // Отключение цветовой пары
      } else {
        mvprintw(i, j * 2, "  ");  // Отрисовка пустой ячейки
      }
    }
  }

  // Отрисовка текущей фигуры
  for (int i = 0; i < FIGURE_SIZE; i++) {
    for (int j = 0; j < FIGURE_SIZE; j++) {
      if (state->current_figure[i][j] == 1) {
        mvprintw(state->figure_y + i, (state->figure_x + j) * 2,
                 "[]");  // Отрисовка части фигуры
      }
    }
  }

  // Отрисовка счета и уровня
  attron(COLOR_PAIR(5));
  mvprintw(0, FIELD_OFFSET_X, "SCORE %d",
           state->score);  // Отображение текущего счета
  mvprintw(
      1, FIELD_OFFSET_X, "MAX %s",
      state->high_score > 0 ? "" : "-");  // Отображение максимального счета
  if (state->high_score > 0) {
    mvprintw(1, FIELD_OFFSET_X + 7, "%d",
             state->high_score);  // Отображение максимального счета
  }
  mvprintw(2, FIELD_OFFSET_X, "LEVEL %d", state->level);  // Отображение уровня

  draw_next_figure(state);  // Отрисовка следующей фигуры
  attroff(COLOR_PAIR(5));   // Отключение цветовой пары

  // Отрисовка подсказок управления
  attron(COLOR_PAIR(4));
  mvprintw(10, FIELD_OFFSET_X, "|v| DROP");  // Подсказка для падения
  mvprintw(11, FIELD_OFFSET_X, "|^| ROTATE");  // Подсказка для поворота
  mvprintw(12, FIELD_OFFSET_X,
           "|<| MOVE LEFT");  // Подсказка для движения влево
  mvprintw(13, FIELD_OFFSET_X,
           "|>| MOVE RIGHT");  // Подсказка для движения вправо
  mvprintw(14, FIELD_OFFSET_X,
           "|s| MUTE MUSIC");  // Подсказка для отключения музыки
  mvprintw(15, FIELD_OFFSET_X, "|p| PAUSE");  // Подсказка для паузы
  mvprintw(16, FIELD_OFFSET_X, "|q| QUIT");  // Подсказка для выхода
  attroff(COLOR_PAIR(4));  // Отключение цветовой пары

  refresh();  // Обновление экрана
}

// Функция для отрисовки следующей фигуры
void draw_next_figure(TetrisState *state) {
  mvprintw(3, FIELD_OFFSET_X, "NEXT");  // Заголовок для следующей фигуры

  // Отрисовка следующей фигуры
  for (int i = 0; i < FIGURE_SIZE; i++) {
    for (int j = 0; j < FIGURE_SIZE; j++) {
      if (state->next_figure[i][j] == 1) {
        mvprintw(4 + i, FIELD_OFFSET_X + j * 2,
                 "[]");  // Отрисовка части следующей фигуры
      } else {
        mvprintw(4 + i, FIELD_OFFSET_X + j * 2, "  ");  // Пустая ячейка
      }
    }
  }
}

// Функция для отрисовки стартового экрана
void draw_start_screen() {
  clear();                // Очистка экрана
  attron(COLOR_PAIR(2));  // Установка цвета
  draw_frame(8, 4, 7, 25);  // Отрисовка рамки для стартового экрана
  mvprintw(FIELD_HEIGHT / 2, FIELD_WIDTH - 6,
           "PRESS SPACE TO START");  // Подсказка для начала игры
  attroff(COLOR_PAIR(2));            // Отключение цвета
  refresh();                         // Обновление экрана
}

// Функция для отрисовки экрана окончания игры
void draw_game_over_screen(TetrisState *state) {
  clear();                // Очистка экрана
  attron(COLOR_PAIR(3));  // Установка цвета для окончания игры
  draw_line(9, 3, 19);                        // Отрисовка линии
  mvprintw(8, FIELD_WIDTH - 4, "GAME OVER");  // Сообщение об окончании игры
  mvprintw(10, FIELD_WIDTH - 4, "SCORE %d", state->score);  // Отображение счета

  // Подсказки для выхода или перезапуска игры
  mvprintw(12, 4, "PRESS Q TO QUIT OR");
  mvprintw(13, 4, "SPACE TO PLAY AGAIN");

  attroff(COLOR_PAIR(3));  // Отключение цвета
  refresh();               // Обновление экрана
}

// Функция для отрисовки экрана паузы
void draw_pause_screen() {
  attron(COLOR_PAIR(6));  // Установка цвета для паузы
  mvprintw(FIELD_HEIGHT / 2 - 2, FIELD_WIDTH / 2 + 3,
           "PAUSE");       // Сообщение о паузе
  attroff(COLOR_PAIR(6));  // Отключение цвета
  refresh();               // Обновление экрана
}

// Функция для обновления текущего состояния игры
void updateCurrentState(TetrisState *state) {
  switch (state->state) {
    case START:
      draw_start_screen();  // Отрисовка стартового экрана
      break;
    case PLAY:
      draw_game(state);  // Отрисовка игрового поля
      break;
    case PAUSE:
      draw_pause_screen();  // Отрисовка экрана паузы
      break;
    case GAME_OVER:
      draw_game_over_screen(state);  // Отрисовка экрана окончания игры
      break;
    case EXIT:
      endwin();  // Завершение работы ncurses
      break;
    default:
      perror("Error\n");  // Сообщение об ошибке
      break;
  }
}

// Функция для отрисовки рамки
void draw_frame(int start_y, int start_x, int height, int width) {
  mvhline(start_y, start_x, '-', width);  // Верхняя граница
  mvhline(start_y + height - 1, start_x, '-', width);  // Нижняя граница
  mvvline(start_y, start_x, '|', height);  // Левая граница
  mvvline(start_y, start_x + width - 1, '|', height);  // Правая граница

  mvaddch(start_y, start_x, '<');  // Левый верхний угол
  mvaddch(start_y, start_x + width - 1, '>');  // Правый верхний угол
  mvaddch(start_y + height - 1, start_x, '<');  // Левый нижний угол
  mvaddch(start_y + height - 1, start_x + width - 1,
          '>');  // Правый нижний угол
}

// Функция для отрисовки линии
void draw_line(int start_y, int start_x, int width) {
  mvhline(start_y, start_x, '-', width);  // Отрисовка линии
  mvaddch(start_y, start_x, '<');         // Левый конец линии
  mvaddch(start_y, start_x + width - 1, '>');  // Правый конец линии
}
