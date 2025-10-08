#include "tetris.h"

// Инициализация состояния игры
void init_game(TetrisState *state) {
  // Заполнение игрового поля границами
  for (int i = 0; i < FIELD_HEIGHT; i++) {
    for (int j = 0; j < FIELD_WIDTH; j++) {
      // Установка границ поля
      if (i == 0 || j == 0 || j == FIELD_WIDTH - 1 || i == FIELD_HEIGHT - 1) {
        state->field[i][j] = 1;  // Граница
      } else {
        state->field[i][j] = 0;  // Пустая ячейка
      }
    }
  }

  // Установка начальных параметров игры
  state->figure_x = FIGURE_SIZE;  // Начальная позиция фигуры по x
  state->figure_y = 1;  // Начальная позиция фигуры по y
  state->score = 0;     // Начальный счет
  state->level = 1;     // Начальный уровень

  srand(time(NULL));  // Инициализация генератора случайных чисел

  generate_next_figure(state);  // Генерация следующей фигуры
  spawn_current_figure(state);  // Спавн текущей фигуры

  load_high_score(state);  // Загрузка рекорда
}

// Загрузка рекорда из файла
void load_high_score(TetrisState *state) {
  FILE *file = fopen(SCORE_FILE, "r");  // Открытие файла для чтения

  if (file) {
    // Чтение рекорда из файла
    if (fscanf(file, "%d", &state->high_score) != 1) {
      state->high_score = 0;  // Если чтение не удалось, установить в 0
    }
    fclose(file);  // Закрытие файла
  } else {
    state->high_score = 0;  // Если файл не найден, установить рекорд в 0
    save_high_score(state);  // Сохранить рекорд в файл
  }
}

// Сохранение рекорда в файл
void save_high_score(TetrisState *state) {
  FILE *file = fopen(SCORE_FILE, "w");  // Открытие файла для записи

  if (file) {
    fprintf(file, "%d", state->high_score);  // Запись рекорда в файл
    fclose(file);                            // Закрытие файла
  } else {
    perror("Error, cannot open file for writing");  // Ошибка открытия файла
  }
}

// Генерация следующей фигуры
void generate_next_figure(TetrisState *state) {
  // Определение фигур
  int figures[7][4][4] = {
      {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},  // O
      {{0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}},  // I
      {{1, 0, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},  // J
      {{0, 0, 1, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},  // L
      {{1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},  // S
      {{0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},  // Z
      {{0, 1, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}   // T
  };

  int index = rand() % 7;  // Случайный выбор фигуры
  for (int i = 0; i < FIGURE_SIZE; i++) {
    for (int j = 0; j < FIGURE_SIZE; j++) {
      state->next_figure[i][j] =
          figures[index][i][j];  // Установка следующей фигуры
    }
  }
}

// Спавн текущей фигуры
bool spawn_current_figure(TetrisState *state) {
  bool result = true;  // Флаг успешного спавна

  // Копирование следующей фигуры в текущую
  for (int i = 0; i < FIGURE_SIZE; i++) {
    for (int j = 0; j < FIGURE_SIZE; j++) {
      state->current_figure[i][j] = state->next_figure[i][j];
    }
  }

  state->is_figure_fixed = false;  // Фигура еще не зафиксирована

  int figure_width = get_figure_width(state->current_figure);  // Ширина фигуры

  state->figure_x =
      get_random_start_position(figure_width);  // Случайная позиция по x
  state->figure_y = 1;  // Начальная позиция по y

  // Проверка на столкновение при спавне
  for (int y = 0; y < FIGURE_SIZE && result; y++) {
    for (int x = 0; x < FIGURE_SIZE; x++) {
      if (state->current_figure[y][x] == 1 &&
          state->field[state->figure_y + y][state->figure_x + x] == 1) {
        result = false;  // Есть столкновение
      }
    }
  }

  generate_next_figure(state);  // Генерация следующей фигуры
  return result;  // Возвращаем результат спавна
}

// Получение ширины фигуры
int get_figure_width(int figure[FIGURE_SIZE][FIGURE_SIZE]) {
  int width = 0;  // Изначально ширина равна 0

  // Проход по всем колонкам фигуры
  for (int j = 0; j < FIGURE_SIZE; j++) {
    for (int i = 0; i < FIGURE_SIZE; i++) {
      if (figure[i][j] == 1) {
        width = j + 1;  // Обновление ширины
      }
    }
  }
  return width;  // Возвращаем ширину
}

// Получение случайной стартовой позиции по x
int get_random_start_position(int figure_width) {
  int return_val;
  int min_x = 1;  // Минимальная позиция
  int max_x = FIELD_WIDTH - figure_width - 1;  // Максимальная позиция

  // Если максимальная позиция меньше минимальной, устанавливаем минимальную
  if (max_x < min_x) {
    return_val = min_x;
  }

  return_val = min_x + rand() % (max_x - min_x + 1);  // Случайная позиция
  return return_val;  // Возвращаем позицию
}

// Фиксация фигуры на поле
void fix_figure(TetrisState *state) {
  // Проход по всем ячейкам текущей фигуры
  for (int y = 0; y < FIGURE_SIZE; y++) {
    for (int x = 0; x < FIGURE_SIZE; x++) {
      if (state->current_figure[y][x] == 1) {
        state->field[state->figure_y + y][state->figure_x + x] =
            1;  // Установка фигуры на поле
      }
    }
  }
}

// Проверка и очистка заполненных линий
void check_lines(TetrisState *state) {
  int clear_lines = 0;  // Количество очищенных линий

  // Проход по всем линиям
  for (int i = 1; i < FIELD_HEIGHT - 1; i++) {
    bool line_full = true;  // Предполагаем, что линия полная
    for (int j = 1; j < FIELD_WIDTH - 1 && line_full; j++) {
      if (state->field[i][j] == 0) {
        line_full = false;  // Если есть пустая ячейка, линия не полная
      }
    }
    if (line_full) {
      clear_lines++;  // Увеличиваем счетчик очищенных линий
      // Сдвиг линий вниз
      for (int k = i; k > 1; k--) {
        for (int j = 1; j < FIELD_WIDTH - 1; j++) {
          state->field[k][j] =
              state->field[k - 1][j];  // Перемещение линии вниз
        }
      }
    }
  }

  // Обновление счета в зависимости от количества очищенных линий
  switch (clear_lines) {
    case 1:
      state->score += 100;  // 100 очков за 1 линию
      break;
    case 2:
      state->score += 300;  // 300 очков за 2 линии
      break;
    case 3:
      state->score += 700;  // 700 очков за 3 линии
      break;
    case 4:
      state->score += 1500;  // 1500 очков за 4 линии
      break;
    default:
      break;
  }

  // Проверка повышения уровня
  if (state->score / 600 > state->level - 1 && state->level < 10) {
    state->level++;  // Увеличиваем уровень
  }

  // Проверка рекорда
  if (state->score > state->high_score) {
    state->high_score = state->score;  // Обновляем рекорд
    save_high_score(state);            // Сохранение рекорда
  }
}

// Жесткое падение фигуры
void hard_drop(TetrisState *state) {
  while (move_figure_down(state)) {
    // Двигаем фигуру вниз, пока это возможно
  }
}

// Движение фигуры вниз
bool move_figure_down(TetrisState *state) {
  bool collision = false;  // Флаг столкновения

  // Проверка на столкновение при движении вниз
  for (int i = 0; i < FIGURE_SIZE && !collision; i++) {
    for (int j = 0; j < FIGURE_SIZE; j++) {
      if (state->current_figure[i][j] == 1 &&
          state->field[state->figure_y + i + 1][state->figure_x + j] == 1) {
        collision = true;  // Есть столкновение
      }
    }
  }

  if (!collision) {
    state->figure_y++;  // Двигаем фигуру вниз
  }

  return !collision;  // Возвращаем результат движения
}

// Функция для движения фигуры по горизонтали
void move_figure_horizontal(TetrisState *state, int dx) {
  bool collision = false;  // Флаг столкновения

  // Проверка на столкновение при движении
  for (int i = 0; i < FIGURE_SIZE && !collision; i++) {
    for (int j = 0; j < FIGURE_SIZE; j++) {
      if (state->current_figure[i][j] == 1 &&
          state->field[state->figure_y + i][state->figure_x + j + dx] == 1) {
        collision = true;  // Обнаружено столкновение
      }
    }
  }

  // Если столкновения нет, перемещаем фигуру
  if (!collision) {
    state->figure_x += dx;  // Двигаем фигуру по оси x
  }
}

// Проверка на столкновение фигуры с полем
bool check_collision(const TetrisState *state) {
  // Проход по всем ячейкам фигуры
  for (int i = 0; i < FIGURE_SIZE; ++i) {
    for (int j = 0; j < FIGURE_SIZE; ++j) {
      // Если есть пересечение с заполненной ячейкой поля
      if (state->current_figure[i][j] &&
          state->field[state->figure_y + i][state->figure_x + j]) {
        return true;  // Возвращаем true при столкновении
      }
    }
  }
  return false;  // Возвращаем false, если столкновений нет
}

// Копирование фигуры из источника в назначение
void copy_figure(int dest[FIGURE_SIZE][FIGURE_SIZE],
                 int src[FIGURE_SIZE][FIGURE_SIZE]) {
  for (int i = 0; i < FIGURE_SIZE; ++i) {
    for (int j = 0; j < FIGURE_SIZE; ++j) {
      dest[i][j] = src[i][j];  // Копируем значение
    }
  }
}

// Поворот фигуры на 90 градусов по часовой стрелке
void rotate_figure(TetrisState *state) {
  int temp_figure[FIGURE_SIZE]
                 [FIGURE_SIZE];  // Временное хранилище для текущей фигуры
  copy_figure(temp_figure, state->current_figure);  // Копируем текущую фигуру

  // Поворот на 90 градусов по часовой стрелке
  for (int i = 0; i < FIGURE_SIZE; ++i) {
    for (int j = 0; j < FIGURE_SIZE; ++j) {
      state->current_figure[j][FIGURE_SIZE - 1 - i] = temp_figure[i][j];
    }
  }

  // Проверка на столкновение после поворота
  if (check_collision(state)) {
    // Попытка сдвинуть фигуру влево или вправо
    int shifts[] = {-2, 2};  // Возможные сдвиги
    int resolved = 0;        // Флаг разрешения

    // Проверка сдвигов
    for (int k = 0; k < 2; ++k) {
      int shift = shifts[k];
      state->figure_x += shift;  // Применяем сдвиг

      if (!check_collision(state)) {
        resolved = 1;  // Сдвиг разрешен
        break;
      }

      state->figure_x -= shift;  // Откат сдвига
    }

    // Если сдвинуть не удалось — откат поворота
    if (!resolved) {
      copy_figure(state->current_figure,
                  temp_figure);  // Восстанавливаем прежнее состояние фигуры
    }
  }
}

// Обработка пользовательского ввода
void userInput(TetrisState *state, int ch) {
  // Проверка состояния игры
  if (state->state == START) {
    if (ch == START_BUTTON) {
      init_game(state);     // Инициализация игры
      state->state = PLAY;  // Переход в состояние игры
    } else if (ch == TERMINATE_BUTTON) {
      state->state = EXIT;  // Выход из игры
    }
  } else if (state->state == GAME_OVER) {
    if (ch == START_BUTTON) {
      init_game(state);     // Инициализация игры
      state->state = PLAY;  // Переход в состояние игры
    } else if (ch == TERMINATE_BUTTON) {
      state->state = EXIT;  // Выход из игры
    }
  } else if (state->state == PAUSE) {
    if (ch == PAUSE_BUTTON) {
      state->state = PLAY;  // Возвращение в игру
    } else if (ch == TERMINATE_BUTTON) {
      state->state = EXIT;  // Выход из игры
    }
  } else if (state->state == PLAY) {
    // Обработка действий игрока в состоянии игры
    switch (ch) {
      case TERMINATE_BUTTON:
        state->state = EXIT;  // Выход из игры
        break;
      case PAUSE_BUTTON:
        state->state = PAUSE;  // Пауза
        break;
      case LEFT_BUTTON:
        move_figure_horizontal(state, -1);  // Движение влево
        break;
      case RIGHT_BUTTON:
        move_figure_horizontal(state, 1);  // Движение вправо
        break;
      case UP_BUTTON:
        rotate_figure(state);  // Поворот фигуры
        break;
      case DOWN_BUTTON:
        if (!state->is_figure_fixed) {
          hard_drop(state);  // Жесткое падение фигуры
        }
        break;
      case ACTION_BUTTON:
        // Переключение музыки
        if (state->mute == false) {
          pause_music();       // Пауза музыки
          state->mute = true;  // Установка в режим без звука
        } else {
          resume_music();       // Продолжение музыки
          state->mute = false;  // Установка в режим со звуком
        }
        break;
      default:
        perror("Error\n");  // Ошибка ввода
        break;
    }
  }
}

// Движение фигуры и проверка уровня
void move_and_level_check(TetrisState *state, int *timer) {
  int delay = 12 - (state->level - 1);  // Задержка в зависимости от уровня

  // Минимальная задержка
  if (delay <= 2) {
    delay = 2;
  }

  // Если игра в состоянии PLAY и прошла задержка
  if (state->state == PLAY && (*timer)++ >= delay) {
    if (!move_figure_down(state)) {  // Двигаем фигуру вниз
      if (!state->is_figure_fixed) {
        fix_figure(state);  // Фиксируем фигуру
        state->is_figure_fixed = true;  // Устанавливаем флаг фиксации
      }

      check_lines(state);  // Проверка линий на очистку

      if (state->is_figure_fixed) {
        if (!spawn_current_figure(state)) {
          state->state =
              GAME_OVER;  // Если не удалось спавнить фигуру, игра окончена
        } else {
          state->is_figure_fixed = false;  // Сбрасываем флаг фиксации
        }
      }
    }

    (*timer) = 0;  // Сброс таймера
  }
}

// Воспроизведение музыки
int play_music(Mix_Music **music, const char *music_path) {
  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
    perror("SDL_mixer could not initialize!\n");  // Ошибка инициализации
    return 1;
  }

  *music = Mix_LoadMUS(music_path);  // Загрузка музыки

  if (*music == NULL) {
    perror(
        "Failed to load music! SDL_mixer error.\n");  // Ошибка загрузки музыки
    return 1;
  }

  Mix_PlayMusic(*music, -1);  // Воспроизведение музыки

  return 0;  // Успешное завершение
}

// Остановка музыки
int stop_music() {
  Mix_HaltMusic();  // Остановка воспроизведения музыки
  return 0;         // Успешное завершение
}

// Пауза музыки
int pause_music() {
  if (Mix_PlayingMusic()) {
    Mix_PauseMusic();  // Пауза музыки
  }
  return 0;  // Успешное завершение
}

// Возобновление музыки
int resume_music() {
  if (Mix_PausedMusic()) {
    Mix_ResumeMusic();  // Продолжение музыки
  }
  return 0;  // Успешное завершение
}
