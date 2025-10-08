#ifndef MODEL_H_
#define MODEL_H_

#include "../snake.h"

namespace s21 {

// Класс SnakeModel отвечает за логику игры "Змейка"
class SnakeModel {
 public:
  // Позиция фрукта на игровом поле
  Position fruit;

  // Метод для постоянного движения змейки
  void constant_movement();

  // Метод для проверки столкновения с границами игрового поля или хвостом
  // змейки
  bool check_collision();

  // Метод для размещения фрукта на игровом поле
  void place_fruit();

  // Метод для обработки столкновения с фруктом
  void collision_to_fruit();

  // Метод для перемещения хвоста змейки
  void move_tail();

  // Метод для поиска наивысшего рекорда в игре
  int load_high_score();

  // Метод для инициализации параметров игры
  void initialize();

  // Конструктор класса SnakeModel
  SnakeModel();

  // Деструктор класса SnakeModel
  ~SnakeModel();

  // Метод для запуска игры
  void start();

  // Метод для перезапуска игры
  void restart();

  // Метод для выполнения одного такта игрового цикла
  GameInfo_t updateCurrentState();

  // Метод для записи данных в файл (например, рекорды)
  void write_to_file();

  // Метод для обновления уровня игры (например, при достижении определенных
  // условий)
  bool update_level();

  // Переменная для хранения текущего направления движения змейки
  int direction;

  // Структура для хранения информации о состоянии игры
  GameInfo_t game;

  // Позиция головы змейки
  Position snake_head;

  // Массивы для хранения координат частей хвоста змейки
  int tail_x[201], tail_y[201];

  // Текущая длина хвоста змейки
  int tail_size;
};

}  // namespace s21

#endif  // MODEL_H_
