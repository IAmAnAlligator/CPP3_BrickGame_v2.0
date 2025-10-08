#ifndef VIEW_H_
#define VIEW_H_
#define USE_NCURSES

#include "../../../brick_game/snake/controller/controller.h"  // Подключение заголовка контроллера

namespace s21 {

// Класс представления змейки
class SnakeView {
  SnakeController *controller;  // Указатель на контроллер змейки

  // Метод для обработки победы
  void victory();

  // Метод для отрисовки игры
  void draw();

  // Метод для отображения информации (например, счета)
  void information();

  // Метод для отображения скорости
  void print_speed(int offset_x);

  // Метод для перезапуска игры
  void restart();

  // Метод для начала игры
  void start();

  // Метод для паузы игры
  void pause();

  // Метод для отрисовки головы змейки
  void draw_head(int y, int x, bool &drawn);

  // Метод для отрисовки хвоста змейки
  void draw_tail(int y, int x, bool &drawn);

  // Метод для отрисовки ячейки поля
  void draw_cell(int y, int x, bool drawn);

 public:
  // Конструктор класса, принимающий указатель на контроллер
  SnakeView(SnakeController *controller);

  // Деструктор класса
  ~SnakeView();

  // Метод для запуска игры
  void run_snake();
};

}  // namespace s21

#endif  // VIEW_H_
