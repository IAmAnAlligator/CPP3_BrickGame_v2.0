#ifndef TETRISQT_H
#define TETRISQT_H

// Qt-заголовки для работы с графикой, событиями клавиш, таймерами и виджетами
#include <QDebug>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QPainter>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

// Подключение C-заголовка с логикой игры Tetris
#ifdef __cplusplus
extern "C" {
#endif
#include "../../../brick_game/tetris/tetris.h"
#ifdef __cplusplus
}
#endif

// Класс TetrisWidget реализует графический интерфейс для игры в Тетрис на Qt
class TetrisWidget : public QWidget {
  Q_OBJECT

  // Состояние игры, определённое в C-структуре
  TetrisState state;

  // Основной виджет с информацией
  QWidget *infoWidget;

  // Таймер, отвечающий за обновление игры
  QTimer *timer;

  // Метки для отображения информации
  QLabel *levelLabel;      // уровень
  QLabel *scoreLabel;      // счёт
  QLabel *speedLabel;      // скорость
  QLabel *highScoreLabel;  // рекорд
  QLabel *pause;           // метка "Пауза"
  QLabel *victory;         // метка "Победа"
  QLabel *next;            // следующая фигура
  QLabel *drop;       // подсказка управления — сброс
  QLabel *rotate;     // поворот фигуры
  QLabel *moveLeft;   // движение влево
  QLabel *moveRight;  // движение вправо
  QLabel *muteMusic;  // отключение музыки
  QLabel *pauseInfo;  // подсказка "Пауза"
  QLabel *quit;       // выход

  // Размер одной "клетки" на игровом поле
  const int DOT_SIZE = 30;

  // Счётчик тиков таймера
  int timerCount = 0;

  // Указатель на музыку (используется с SDL_mixer)
  Mix_Music *music = nullptr;

  // Вспомогательные методы для настройки интерфейса и отрисовки
  void setupLayout();  // Настройка компоновки виджетов
  void drawFigures();  // Отрисовка фигур на игровом поле
  void printInformation();  // Обновление отображаемой информации

  // Методы отрисовки различных экранов состояния игры
  void drawStartScreen();     // стартовый экран
  void drawPauseScreen();     // экран паузы
  void drawGameOverScreen();  // экран окончания игры

 protected:
  // Переопределение события отрисовки
  void paintEvent(QPaintEvent *e) override;

  // Переопределение события нажатия клавиш
  void keyPressEvent(QKeyEvent *event) override;

 public:
  // Конструктор и деструктор
  TetrisWidget(QWidget *parent = nullptr);
  ~TetrisWidget();

  // Метод обновления игрового состояния (по таймеру)
  void updateGame();

  // Метод перезапуска игры
  void restartGame();

  // Утилита для добавления метки с заданным шрифтом в компоновку
  void addLabel(QVBoxLayout *layout, QLabel *label, const QFont &font);
};

#endif  // TETRISQT_H
