#include "view.h"

namespace s21 {

// Конструктор класса SnakeView, инициализирующий указатель на контроллер
SnakeView::SnakeView(SnakeController *c) : controller{c} {};

// Деструктор класса SnakeView
SnakeView::~SnakeView() = default;

// Метод для запуска игры
void SnakeView::run_snake() {
  controller->game_start();  // Запускает игру
  start();  // Вызывает метод для начала игры

  // Основной игровой цикл
  while (controller->get_state() != EXIT) {
    int key = getch();  // Получает нажатую клавишу

    if (key != ERR) {
      UserAction_t action =
          controller->get_signal(key);  // Получает действие пользователя
      controller->userInput(action, true);  // Обрабатывает ввод пользователя
    }

    controller->game_update_current_state();  // Обновляет состояние игры
    draw();  // Отрисовывает текущее состояние игры

    // Проверяет состояние игры и выполняет соответствующие действия
    if (controller->get_state() == VICTORY) {
      victory();  // Обрабатывает победу
    }

    if (controller->get_state() == PAUSE) {
      pause();  // Обрабатывает паузу
    }

    if (controller->get_state() == GAME_OVER) {
      restart();  // Обрабатывает конец игры
    }
  }
}

// Метод для обработки паузы в игре
void SnakeView::pause() {
  attron(COLOR_PAIR(6));  // Включает цветовую пару для текста
  mvprintw(10, 10, "PAUSE");  // Отображает текст "PAUSE"
  attroff(COLOR_PAIR(6));     // Отключает цветовую пару
  refresh();                  // Обновляет экран

  nodelay(stdscr, FALSE);  // Отключает немедленный ввод
  // Цикл ожидания, пока игра на паузе
  while (controller->get_state() == PAUSE) {
    int key = getch();  // Получает нажатую клавишу
    if (key != ERR) {
      UserAction_t action =
          controller->get_signal(key);  // Получает действие пользователя
      controller->userInput(action, true);  // Обрабатывает ввод
    }
  }

  nodelay(stdscr, TRUE);  // Включает немедленный ввод
}

// Метод для начала игры
void SnakeView::start() {
  clear();                // Очищает экран
  attron(COLOR_PAIR(2));  // Включает цветовую пару для текста
  mvprintw(13, 10,
           "Press SPACE to start...");  // Отображает сообщение о начале игры
  attroff(COLOR_PAIR(2));  // Отключает цветовую пару
  refresh();               // Обновляет экран

  nodelay(stdscr, FALSE);  // Отключает немедленный ввод
  // Цикл ожидания, пока игра не начнется
  while (controller->get_state() == START) {
    int key = getch();  // Получает нажатую клавишу
    if (key != ERR) {
      UserAction_t action =
          controller->get_signal(key);  // Получает действие пользователя
      controller->userInput(action, true);  // Обрабатывает ввод
    }
  }
  nodelay(stdscr, TRUE);  // Включает немедленный ввод
}

// Метод для перезапуска игры
void SnakeView::restart() {
  clear();  // Очищает экран
  int offset_x = 10;  // Смещение по оси X для отображения текста
  attron(COLOR_PAIR(3));  // Включает цветовую пару для текста
  mvprintw(8, offset_x, "GAME OVER");  // Отображает сообщение о конце игры
  attroff(COLOR_PAIR(3));  // Отключает цветовую пару

  // Сообщение о перезапуске или выходе
  mvprintw(10, offset_x, "Press SPACE to restart");
  mvprintw(11, offset_x, "Press ESC to quit");

  nodelay(stdscr, FALSE);  // Отключает немедленный ввод
  // Цикл ожидания, пока игра не перезапустится
  while (controller->get_state() == GAME_OVER) {
    int key = getch();  // Получает нажатую клавишу
    if (key != ERR) {
      UserAction_t action =
          controller->get_signal(key);  // Получает действие пользователя
      controller->userInput(action, true);  // Обрабатывает ввод
    }
  }

  nodelay(stdscr, TRUE);  // Включает немедленный ввод

  controller->game_write_to_file();  // Записывает результаты игры в файл

  // Если состояние не EXIT, перезапускает игру
  if (controller->get_state() != EXIT) {
    controller->game_restart();
  }
}

// Метод для обработки победы
void SnakeView::victory() {
  clear();                // Очищает экран
  attron(COLOR_PAIR(6));  // Включает цветовую пару для текста
  mvprintw(7, 4,
           "Victory! You have completed the game!");  // Отображает сообщение о
                                                      // победе
  attroff(COLOR_PAIR(6));  // Отключает цветовую пару
  mvprintw(9, 4, "Score: %d", controller->get_score());  // Отображает счет
  mvprintw(10, 4, "Press ESC to quit");  // Сообщение о выходе
  refresh();                             // Обновляет экран
  controller->game_write_to_file();  // Записывает результаты игры в файл

  // Цикл ожидания, пока игра в состоянии победы
  while (controller->get_state() == VICTORY) {
    int key = getch();  // Получает нажатую клавишу
    if (key != ERR && key != KEY_SPACE) {
      UserAction_t action =
          controller->get_signal(key);  // Получает действие пользователя
      controller->userInput(action, true);  // Обрабатывает ввод
    }
  }
}

// Метод для отрисовки игрового поля
void SnakeView::draw() {
  clear();  // Очищает экран перед отрисовкой

  // Цикл по всем строкам игрового поля
  for (int i = 0; i < HEIGHT + 2; i++) {
    // Цикл по всем столбцам игрового поля
    for (int j = 0; j < WIDTH + 2; j++) {
      int x = j;           // Координата по оси X
      int y = i;           // Координата по оси Y
      bool drawn = false;  // Флаг, указывающий, была ли ячейка уже нарисована

      // Вызов методов для отрисовки головы, хвоста и ячейки
      draw_head(y, x, drawn);
      draw_tail(y, x, drawn);
      draw_cell(y, x, drawn);
    }
    printw("\n");  // Переход на новую строку в конце каждой строки поля
  }

  // Проверяет, обновился ли уровень, и обрабатывает победу
  if (controller->game_update_level() == 1) {
    victory();
  }

  information();  // Вызывает метод для отображения информации о состоянии игры
}

// Метод для отрисовки головы змейки
void SnakeView::draw_head(int y, int x, bool &drawn) {
  // Проверяет, является ли текущая позиция головой змейки
  bool isHead = (y == controller->get_snake_head_y() &&
                 x == controller->get_snake_head_x());
  if (!drawn && isHead) {  // Если ячейка еще не нарисована и это голова
    attron(COLOR_PAIR(2));  // Включает цветовую пару для головы
    mvprintw(y, x * 2, "O");  // Отображает голову змейки
    attroff(COLOR_PAIR(2));   // Отключает цветовую пару
    drawn = true;  // Устанавливает флаг, что ячейка была нарисована
  }
}

// Метод для отрисовки хвоста змейки
void SnakeView::draw_tail(int y, int x, bool &drawn) {
  bool isTail = false;  // Флаг для проверки, является ли ячейка частью хвоста

  if (!drawn) {  // Если ячейка еще не нарисована
    int tailSize = controller->get_tail_size();  // Получает размер хвоста
    // Цикл по всем частям хвоста
    for (int k = 0; k < tailSize; k++) {
      // Проверяет, совпадает ли текущая ячейка с частью хвоста
      bool match =
          (controller->get_tail_x(k) == x && controller->get_tail_y(k) == y);
      isTail = isTail ||
               match;  // Накопление факта, если ячейка является частью хвоста
    }

    // Если ячейка является частью хвоста
    if (isTail) {
      attron(COLOR_PAIR(7));  // Включает цветовую пару для хвоста
      mvprintw(y, x * 2, "o");  // Отображает часть хвоста
      attroff(COLOR_PAIR(7));   // Отключает цветовую пару
      drawn = true;  // Устанавливает флаг, что ячейка была нарисована
    }
  }
}

// Метод для отрисовки ячейки поля
void SnakeView::draw_cell(int y, int x, bool drawn) {
  if (!drawn) {  // Если ячейка еще не нарисована
    int cell = controller->get_field(y, x);  // Получает значение ячейки из поля

    // Проверяет тип ячейки
    bool isFood = (cell <= -1);  // Является ли ячейка препятствием
    bool isObstacle = (cell == 1);  // Является ли ячейка едой
    bool isEmpty = (cell == 0);  // Является ли ячейка пустой

    // Если ячейка является едой
    if (isFood) {
      attron(COLOR_PAIR(3));  // Включает цветовую пару для еды
      mvprintw(y, x * 2, "@");  // Отображает еду
      attroff(COLOR_PAIR(3));   // Отключает цветовую пару
    }

    // Если ячейка является препятствием
    if (isObstacle) {
      mvprintw(y, x * 2, "[]");  // Отображает препятствие
    }

    // Если ячейка пустая
    if (isEmpty) {
      attron(COLOR_PAIR(1));  // Включает цветовую пару для пустого пространства
      mvprintw(y, x * 2, " ");  // Отображает пустое пространство
      attroff(COLOR_PAIR(1));  // Отключает цветовую пару
    }
  }
}

// Метод для отображения информации о игре
void SnakeView::information() {
  int offset_x = 20;  // Смещение по оси X для отображения информации

  attron(COLOR_PAIR(5));  // Включает цветовую пару для информации
  mvprintw(1, WIDTH + offset_x, "High score: %d",
           controller->get_high_score());  // Отображает лучший счет
  mvprintw(2, WIDTH + offset_x, "Score: %d",
           controller->get_score());  // Отображает текущий счет
  // Отображает уровень
  if (controller->get_level() <= FINAL_LEVEL) {
    mvprintw(3, WIDTH + offset_x, "Level: %d", controller->get_level());
  } else if (controller->get_level() == FINAL_LEVEL + 1) {
    mvprintw(3, WIDTH + offset_x, "Level: %d", FINAL_LEVEL);
  }
  print_speed(offset_x);   // Отображает скорость
  attroff(COLOR_PAIR(5));  // Отключает цветовую пару

  attron(COLOR_PAIR(4));  // Включает цветовую пару для управления
  // Отображает управление
  mvprintw(HEIGHT - 7, WIDTH + offset_x, "|^| MOVE UP");
  mvprintw(HEIGHT - 6, WIDTH + offset_x, "|v| MOVE DOWN");
  mvprintw(HEIGHT - 5, WIDTH + offset_x, "|<| MOVE LEFT");
  mvprintw(HEIGHT - 4, WIDTH + offset_x, "|>| MOVE RIGHT");
  mvprintw(HEIGHT - 3, WIDTH + offset_x, "|P| PAUSE");
  mvprintw(HEIGHT - 2, WIDTH + offset_x, "|ENTER| SPEED UP");
  mvprintw(HEIGHT, WIDTH + offset_x, "|ESC| QUIT");
  attroff(COLOR_PAIR(4));  // Отключает цветовую пару
  refresh();               // Обновляет экран
}

// Метод для отображения скорости игры
void SnakeView::print_speed(int offset_x) {
  int hr_speed = 0;  // Переменная для хранения отображаемой скорости

  // Цикл для определения отображаемой скорости
  for (int i = MAX_DELAY, j = 10; i >= MIN_DELAY; i -= PACE_DELAY, j += 10) {
    if (i == controller->get_speed()) {
      hr_speed = j;  // Устанавливает отображаемую скорость
    }
  }

  mvprintw(4, WIDTH + offset_x, "Speed: %d", hr_speed);  // Отображает скорость
}

}  // namespace s21
