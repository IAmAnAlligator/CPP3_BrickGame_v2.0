#ifndef CONTROLLER_H_
#define CONTROLLER_H_
#define USE_NCURSES

#include "../model/model.h"

namespace s21 {

// Класс SnakeController отвечает за управление логикой игры и взаимодействие с
// моделью
class SnakeController {
  SnakeModel *model;  // Указатель на модель змейки

 public:
  // Конструктор класса SnakeController, принимает указатель на модель
  SnakeController(SnakeModel *model);

  // Деструктор класса SnakeController
  ~SnakeController();

  // Метод обработки пользовательского ввода
  void userInput(UserAction_t action, bool hold);

  // Получение сигнала действия пользователя на основе ввода
  UserAction_t get_signal(int user_input);

  // Обновление текущего состояния игры (делаем один шаг)
  void game_update_current_state();

  // Начало игры
  void game_start();

  // Перезапуск игры
  void game_restart();

  // Получение размера хвоста змейки
  int get_tail_size();

  // Обновление уровня игры
  bool game_update_level();

  // Получение состояния игры (пауза или активная игра)
  int get_state();

  // Установить состояние игры
  void set_state(int state);

  // Запись состояния игры в файл
  void game_write_to_file();

  // Получение текущего счета
  int get_score();

  // Получение координаты Y головы змейки
  int get_snake_head_y();

  // Получение координаты X головы змейки
  int get_snake_head_x();

  // Получение координаты X хвоста змейки по индексу
  int get_tail_x(int a);

  // Получение координаты Y хвоста змейки по индексу
  int get_tail_y(int a);

  // Получение значения поля игры по координатам
  int get_field(int a, int b);

  // Получение рекорда
  int get_high_score();

  // Получение текущего уровня
  int get_level();

  // Получение текущей скорости игры
  int get_speed();
};

}  // namespace s21

#endif  // CONTROLLER_H_