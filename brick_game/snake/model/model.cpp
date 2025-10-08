#include "model.h"

namespace s21 {

// Конструктор класса SnakeModel
SnakeModel::SnakeModel() {
  for (int i = 0; i < HEIGHT + 2; ++i) {
    for (int j = 0; j < WIDTH + 2; ++j) {
      game.field[i][j] = 0;  // Инициализация поля нулями
    }
  }
}

// Деструктор класса SnakeModel
SnakeModel::~SnakeModel() = default;

GameInfo_t SnakeModel::updateCurrentState() {
  // Проверяем, находится ли игра в состоянии "игра идет"
  if (game.pause == PLAY) {
    // Вызываем метод для постоянного движения змейки
    constant_movement();

    // Проверяем столкновение с фруктом и обрабатываем его
    collision_to_fruit();

    // Перемещаем хвост змейки
    move_tail();

    // Приостанавливаем выполнение потока на время, равное скорости игры
    std::this_thread::sleep_for(std::chrono::microseconds(game.speed));

    // Загружаем наивысший рекорд (может быть, для отображения на экране)
    // load_high_score();

    // Проверяем столкновение с границами или хвостом
    if (check_collision()) {
      // Если произошло столкновение, меняем состояние игры на "игра окончена"
      game.pause = GAME_OVER;
    }

    // Проверяем, достиг ли игрок финального уровня или максимальной длины
    // хвоста
    if (game.level == FINAL_LEVEL + 1 || tail_size == FINAL_LENGTH) {
      // Если да, меняем состояние игры на "победа"
      game.pause = VICTORY;
    }
  }

  return game;
}

void SnakeModel::constant_movement() {
  switch (direction) {
    case 3:  // Влево
      snake_head.x--;
      break;
    case 4:  // Вправо
      snake_head.x++;
      break;
    case 5:  // Вверх
      snake_head.y--;
      break;
    case 6:  // Вниз
      snake_head.y++;
      break;
  }
}

void SnakeModel::initialize() {
  // Установка начальных значений для змейки и игры
  snake_head = {WIDTH / 2 + 1, HEIGHT / 2};  // Установка позиции головы змейки
  game.next = NULL;  // Инициализация следующего элемента
  game.high_score = load_high_score();  // Поиск рекорда
  game.score = 0;                       // Начальный счет
  game.level = 1;                       // Начальный уровень
  game.speed = MAX_DELAY;               // Начальная скорость
  tail_size = 4;  // Начальная длина хвоста
  direction = 6;  // Начальное направление (вниз)

  // Инициализация координат хвоста
  memset(tail_x, 0, sizeof(tail_x));
  memset(tail_y, 0, sizeof(tail_y));

  for (int i = 0; i < tail_size; i++) {
    tail_x[i] = snake_head.x;  // Установка координат хвоста
    tail_y[i] = snake_head.y - i;
  }

  // Инициализация игрового поля
  for (int i = 0; i < HEIGHT + 2; i++) {
    for (int j = 0; j < WIDTH + 2; j++) {
      // Установка границ игрового поля
      if (i == 0 || i == HEIGHT + 1 || j == 0 || j == WIDTH + 1) {
        game.field[i][j] = 1;  // Граница
      } else {
        game.field[i][j] = 0;  // Пустое пространство
      }
    }
  }

  place_fruit();  // Размещение первого фрукта
}

// Метод для инициализации игры
void SnakeModel::start() {
  initialize();
  game.pause = START;  // Начальное состояние паузы
}

void SnakeModel::restart() {
  initialize();
  game.pause = PLAY;  // Начальное состояние паузы
}

// Метод для записи рекорда в файл
void SnakeModel::write_to_file() {
  std::ofstream out(FILE_NAME, std::ios::out);  // Открытие файла для добавления

  if (!out) {  // Проверка на успешное открытие файла
    std::cerr << "Ошибка при открытии файла!" << std::endl;
    return;  // Выход из метода в случае ошибки
  }

  out << game.high_score << " ";  // Запись рекорда

  // Файл будет автоматически закрыт, когда объект out выйдет из области
  // видимости
}

// Метод для поиска рекорда
int SnakeModel::load_high_score() {
  int high_score = 0;  // Переменная для хранения рекорда
  int res = 0;
  std::ifstream f(FILE_NAME);  // Открытие файла с рекордами

  if (!f) {  // Проверка на успешное открытие файла
    return 0;  // Если файл не открыт, возвращаем 0
  }

  if (f >> high_score) {  // Чтение первого рекорда
    res = high_score;     // Возврат первого рекорда
  }

  return res;  // Если чтение не удалось, возвращаем 0
}

// Метод для перемещения хвоста змейки
void SnakeModel::move_tail() {
  if (tail_size > 0) {
    // Сдвигаем все сегменты хвоста на одну позицию назад
    for (int i = tail_size - 1; i > 0; --i) {
      tail_x[i] = tail_x[i - 1];
      tail_y[i] = tail_y[i - 1];
    }
    // Первый сегмент хвоста следует за текущей головой
    tail_x[0] = snake_head.x;
    tail_y[0] = snake_head.y;
  }
}

// Метод для размещения фрукта на игровом поле
void SnakeModel::place_fruit() {
  bool empty_place = false;

  while (!empty_place) {
    fruit.x = rand() % WIDTH + 1;
    fruit.y = rand() % HEIGHT + 1;

    bool valid_position = true;

    // Проверка: фрукт не должен быть на голове змейки
    if (fruit.x == snake_head.x && fruit.y == snake_head.y) {
      valid_position = false;
    }

    // Проверка: фрукт не должен быть на хвосте
    for (int i = 0; i < tail_size; ++i) {
      if (tail_x[i] == fruit.x && tail_y[i] == fruit.y) {
        valid_position = false;
      }
    }

    // Проверка: фрукт не должен быть в занятой клетке поля
    if (game.field[fruit.y][fruit.x] != 0) {
      valid_position = false;
    }

    // Если позиция прошла все проверки — размещаем фрукт
    if (valid_position) {
      game.field[fruit.y][fruit.x] = -(rand() % 5 + 1);
      empty_place = true;
    }
  }
}

// Метод для обработки столкновения с фруктом
void SnakeModel::collision_to_fruit() {
  if (game.field[snake_head.y][snake_head.x] <=
      -1) {        // Проверка, съедена ли еда
    game.score++;  // Увеличение счета
    tail_size++;   // Увеличение длины хвоста
    game.field[fruit.y][fruit.x] = 0;  // Удаление фрукта с поля
    if (!update_level()) {             // Проверка уровня
      place_fruit();  // Размещение нового фрукта
    }
  }
}

bool SnakeModel::check_collision() {
  bool collision = false;

  // Столкновение со стеной
  if (game.field[snake_head.y][snake_head.x] == 1) {
    collision = true;
  }

  // Столкновение с хвостом (начиная со 2-го сегмента)
  for (int i = 1; i < tail_size; i++) {
    if (snake_head.x == tail_x[i] && snake_head.y == tail_y[i]) {
      collision = true;
    }
  }

  return collision;
}

// Метод для обновления уровня игры
bool SnakeModel::update_level() {
  bool res = false;  // Результат обновления уровня
  if (game.level <= FINAL_LEVEL && tail_size != FINAL_LENGTH) {
    int level = game.score / 5 + 1;  // Определение уровня на основе счета

    // Обновление рекорда, если текущий счет больше
    if (game.high_score < game.score) {
      game.high_score = game.score;  // Обновление рекорда
    }

    // Проверка, изменился ли уровень
    if (level != game.level) {
      game.level = level;  // Обновление уровня

      // Уменьшение скорости при повышении уровня
      if (game.speed > MIN_DELAY) {
        game.speed -= PACE_DELAY;
      }
      // Обеспечение минимальной скорости
      if (game.speed < MIN_DELAY) {
        game.speed = MIN_DELAY;
      }
    }
  } else {
    res = true;  // Если достигнут финальный уровень
  }

  return res;  // Возврат результата
}

}  // namespace s21
