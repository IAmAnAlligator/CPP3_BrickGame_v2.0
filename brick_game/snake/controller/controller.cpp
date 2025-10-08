#include "controller.h"

namespace s21 {

// Конструктор класса SnakeController, инициализирующий указатель на модель
// змейки
SnakeController::SnakeController(SnakeModel *model) : model{model} {}

// Деструктор класса SnakeController
SnakeController::~SnakeController() = default;

// Установить состояние игры
void SnakeController::set_state(int state) { model->game.pause = state; }

// Получение размера хвоста змейки
int SnakeController::get_tail_size() { return model->tail_size; }

// Начало игры
void SnakeController::game_start() { model->start(); }

// Перезапуск игры
void SnakeController::game_restart() { model->restart(); }

// Обновление текущего состояния игры (делаем один шаг)
void SnakeController::game_update_current_state() {
  model->updateCurrentState();
}

// Обновление уровня игры
bool SnakeController::game_update_level() { return model->update_level(); }

// Получение состояния игры (пауза или активная игра)
int SnakeController::get_state() { return model->game.pause; }

// Запись состояния игры в файл
void SnakeController::game_write_to_file() { model->write_to_file(); }

// Получение текущего счета
int SnakeController::get_score() { return model->game.score; }

// Получение координаты Y головы змейки
int SnakeController::get_snake_head_y() { return model->snake_head.y; }

// Получение координаты X головы змейки
int SnakeController::get_snake_head_x() { return model->snake_head.x; }

// Получение координаты X хвоста змейки по индексу
int SnakeController::get_tail_x(int a) { return model->tail_x[a]; }

// Получение координаты Y хвоста змейки по индексу
int SnakeController::get_tail_y(int a) { return model->tail_y[a]; }

// Получение значения поля игры по координатам
int SnakeController::get_field(int a, int b) { return model->game.field[a][b]; }

// Получение рекорда
int SnakeController::get_high_score() { return model->game.high_score; }

// Получение текущего уровня
int SnakeController::get_level() { return model->game.level; }

// Получение текущей скорости игры
int SnakeController::get_speed() { return model->game.speed; }

// Получение сигнала действия пользователя на основе ввода
UserAction_t SnakeController::get_signal(int user_input) {
  UserAction_t sign = None;  // Переменная для хранения действия
  // Определяем действие на основе пользовательского ввода
  if (user_input == KEY_UP)
    sign = Up;  // Движение вверх
  else if (user_input == KEY_DOWN)
    sign = Down;  // Движение вниз
  else if (user_input == KEY_LEFT)
    sign = Left;  // Движение влево
  else if (user_input == KEY_RIGHT)
    sign = Right;  // Движение вправо
  else if (user_input == ESCAPE)
    sign = Terminate;  // Завершение игры
  else if (user_input == ENTER_KEY)
    sign = Action;  // Действие
  else if (user_input == KEY_SPACE)
    sign = Start;  // Пауза
  else if (user_input == 'p' || user_input == 'P')
    sign = Pause;  // Пауза
  return sign;     // Возврат действия
}

// Метод обработки пользовательского ввода
void SnakeController::userInput(UserAction_t action, bool hold) {
  // Проверяем, удерживается ли кнопка
  if (hold) {
    switch (action) {
      case Left:  // Движение влево
        if (model->direction != 4) {  // Не разворачиваемся на 180 градусов
          model->direction = 3;
        }
        break;
      case Right:  // Движение вправо
        if (model->direction != 3) {  // Не разворачиваемся на 180 градусов
          model->direction = 4;
        }
        break;
      case Up:                        // Движение вверх
        if (model->direction != 6) {  // Не разворачиваемся на 180 градусов
          model->direction = 5;
        }
        break;
      case Down:  // Движение вниз
        if (model->direction != 5) {  // Не разворачиваемся на 180 градусов
          model->direction = 6;
        }
        break;
      case Terminate:            // Завершение игры
        model->write_to_file();  // Сохранение состояния игры
        model->game.pause = EXIT;  // Установка паузы
        break;
      case Action:  // Ускорение игры
        if (model->game.speed >
            150000) {  // Уменьшаем скорость, если она больше порога
          model->game.speed -= 50000;
        }
        break;
      case Pause:                                // Пауза игры
        model->game.pause = !model->game.pause;  // Переключаем состояние паузы
        break;
      case Start:  // Начало игры
        model->game.pause =
            PLAY;  // Устанавливаем паузу на 1 (игра продолжается)
        break;
      case None:
        break;
    }
  }
}

}  // namespace s21
