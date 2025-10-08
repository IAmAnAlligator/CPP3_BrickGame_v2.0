#ifndef SNAKEWIDGET_H
#define SNAKEWIDGET_H

#include <QApplication>
#include <QDebug>
#include <QMainWindow>
#include <QTimer>
#include <QWidget>

#include "../../../brick_game/snake/controller/controller.h"

namespace s21 {

// Класс SnakeWidget реализует графический интерфейс игры Snake с использованием
// Qt
class SnakeWidget : public QWidget {
  Q_OBJECT

  // Контроллер, управляющий логикой игры
  SnakeController* controller_;

  // Таймер игрового цикла (движение змейки и обновление состояния)
  QTimer* timer_;

  // Изображения объектов на игровом поле
  QPixmap headPixmap_;
  QPixmap cakePixmap_;
  QPixmap diamondPixmap_;
  QPixmap grapePixmap_;
  QPixmap rubyPixmap_;
  QPixmap strawberryPixmap_;

  // Размер одной клетки в пикселях
  const int DOT_SIZE = 30;

  // Флаги состояния игры
  bool gameStarted_;  // Игра началась
  bool pause_;        // Игра на паузе
  bool gameOver_;     // Игра окончена
  bool victory_;      // Победа

  // Основная отрисовка игрового поля
  void drawGame(QPainter& painter);

  // Отрисовка содержимого ячейки (внутри drawGame)
  void drawCellContents(QPainter& painter, int x, int y, const QRect& cellRect);

  // Получение цвета для конкретной ячейки
  QColor getCellColor(int x, int y);

  // Проверка: находится ли голова змейки в ячейке (x, y)
  bool isSnakeHead(int x, int y);

  // Проверка: находится ли сегмент хвоста в ячейке (x, y)
  bool isTailSegment(int x, int y);

  // Отрисовка изображения объекта (еда, алмазы и т.п.)
  void drawItemPixmap(QPainter& painter, int x, int y, const QRect& cellRect);

  // Обводка головы или хвоста змейки
  void drawCellOutline(QPainter& painter, int x, int y, const QColor& color,
                       const QRect& cellRect);

  // Внутренняя обводка границ поля
  void drawFieldInnerBorder(QPainter& painter, int x, int y,
                            const QRect& cellRect);

  // Жирная внешняя обводка поля
  void drawFieldBorder(QPainter& painter, int fieldWidth, int fieldHeight);

  // Отрисовка одной клетки заданного цвета
  void drawCell(QPainter& painter, int x, int y, QColor color);

  // Отображение игровой информации (счёт и т.д.)
  void drawInfo(QPainter& painter);

  // Обработка изменения состояния (например, проверка на победу)
  void handleState();

  // Отображение сообщения о победе
  void drawVictoryMessage(QPainter& painter);

  // Стартовый экран с инструкциями
  void drawStartScreen(QPainter& painter);

  // Надпись "PAUSE" при остановке игры
  void drawPauseLabel(QPainter& painter);

  // Надпись "GAME OVER" и инструкции
  void drawGameOverLabel(QPainter& painter);

  // Запуск новой игры
  void start();

  // Перезапуск игры
  void restart();

  // Получение текущей скорости игры (зависит от уровня)
  int getHRSpeed();

 public:
  // Конструктор с передачей контроллера
  explicit SnakeWidget(SnakeController* controller, QWidget* parent = nullptr);

  // Деструктор
  ~SnakeWidget();

 protected:
  // Событие перерисовки окна
  void paintEvent(QPaintEvent* event) override;

  // Обработка нажатия клавиш
  void keyPressEvent(QKeyEvent* event) override;

 private slots:
  // Игровой цикл, вызываемый таймером
  void gameLoop();
};

}  // namespace s21

#endif  // SNAKEWIDGET_H
