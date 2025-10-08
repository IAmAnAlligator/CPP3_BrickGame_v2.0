#include "snake_qt.h"

#include <QDebug>
#include <QKeyEvent>
#include <QPainter>

namespace s21 {

// Конструктор виджета SnakeWidget
SnakeWidget::SnakeWidget(SnakeController* controller, QWidget* parent)
    : QWidget(parent),
      controller_(controller),
      timer_(new QTimer(this)),
      gameStarted_(false),
      pause_(false),
      gameOver_(false),
      victory_(false) {
  setWindowTitle("Snake");
  setFixedSize(DOT_SIZE * (WIDTH + 2) * 2, DOT_SIZE * (HEIGHT + 2));

  // Подключаем таймер к игровому циклу
  connect(timer_, &QTimer::timeout, this, &SnakeWidget::gameLoop);

  // Устанавливаем фокус для обработки клавиш
  setFocusPolicy(Qt::StrongFocus);
  setFocus();

  // Загружаем изображения игровых объектов
  headPixmap_ = QPixmap(":/img/face.png")
                    .scaled(DOT_SIZE, DOT_SIZE, Qt::KeepAspectRatio,
                            Qt::SmoothTransformation);
  cakePixmap_ = QPixmap(":/img/cake.png")
                    .scaled(DOT_SIZE, DOT_SIZE, Qt::KeepAspectRatio,
                            Qt::SmoothTransformation);
  diamondPixmap_ = QPixmap(":/img/diamond.png")
                       .scaled(DOT_SIZE, DOT_SIZE, Qt::KeepAspectRatio,
                               Qt::SmoothTransformation);
  grapePixmap_ = QPixmap(":/img/grape.png")
                     .scaled(DOT_SIZE, DOT_SIZE, Qt::KeepAspectRatio,
                             Qt::SmoothTransformation);
  rubyPixmap_ = QPixmap(":/img/ruby.png")
                    .scaled(DOT_SIZE, DOT_SIZE, Qt::KeepAspectRatio,
                            Qt::SmoothTransformation);
  strawberryPixmap_ = QPixmap(":/img/strawberry.png")
                          .scaled(DOT_SIZE, DOT_SIZE, Qt::KeepAspectRatio,
                                  Qt::SmoothTransformation);
}

// Деструктор
SnakeWidget::~SnakeWidget() { delete timer_; }

// Основной метод отрисовки
void SnakeWidget::paintEvent(QPaintEvent*) {
  QPainter painter(this);

  if (!gameStarted_) {
    drawStartScreen(painter);
  } else if (!gameOver_ && gameStarted_) {
    drawGame(painter);
    drawInfo(painter);

    if (pause_) {
      drawPauseLabel(painter);
    }

    if (victory_) {
      painter.fillRect(rect(), Qt::white);
      drawVictoryMessage(painter);
    }

  } else if (gameOver_) {
    drawGameOverLabel(painter);
  }
}

// Запуск игры
void SnakeWidget::start() {
  timer_->start(80);
  controller_->game_start();
  gameStarted_ = true;
  gameOver_ = false;
  update();
}

// Обработка нажатий клавиш
void SnakeWidget::keyPressEvent(QKeyEvent* event) {
  int key = event->key();
  int state = controller_->get_state();

  switch (key) {
    case Qt::Key_Space:
      if (state != PLAY) {
        start();
        controller_->userInput(Start, true);
      } else if (state == GAME_OVER) {
        controller_->userInput(Start, true);
        restart();
      }
      break;
    case Qt::Key_Return:
      controller_->userInput(Action, true);
      break;
    case Qt::Key_Enter:
      controller_->userInput(Action, true);
      break;
    case Qt::Key_Left:
      if (state == PLAY) {
        controller_->userInput(Left, true);
      }
      break;
    case Qt::Key_Right:
      if (state == PLAY) {
        controller_->userInput(Right, true);
      }
      break;
    case Qt::Key_Up:
      if (state == PLAY) {
        controller_->userInput(Up, true);
      }
      break;
    case Qt::Key_Down:
      if (state == PLAY) {
        controller_->userInput(Down, true);
      }
      break;
    case Qt::Key_Escape:
      if (state != START && state != VICTORY) {
        controller_->userInput(Terminate, true);
      } else if (controller_->get_state() == VICTORY) {
        controller_->userInput(Terminate, true);
      }
      break;
    case Qt::Key_P:
      if (state == PLAY) {
        pause_ = true;
        controller_->userInput(Pause, true);
      } else if (state == PAUSE) {
        pause_ = false;
        controller_->userInput(Pause, true);
      }
      break;
    default:
      break;
  }
}

// Перезапуск игры
void SnakeWidget::restart() {
  gameOver_ = false;
  controller_->game_restart();
}

// Основной игровой цикл
void SnakeWidget::gameLoop() {
  int state = controller_->get_state();

  if (state == PLAY || state == PAUSE || state == START) {
    controller_->game_update_current_state();
  }

  handleState();

  update();
}

// Обработка изменений состояния игры
void SnakeWidget::handleState() {
  int state = controller_->get_state();
  if (state == GAME_OVER) {
    controller_->game_write_to_file();
    gameOver_ = true;
  }

  if (state == VICTORY) {
    controller_->game_write_to_file();
    victory_ = true;
  }

  if (state == EXIT) {
    controller_->game_write_to_file();
    close();
  }
}

// Отрисовка экрана победы
void SnakeWidget::drawVictoryMessage(QPainter& painter) {
  QRect fullRect = rect();
  QFont titleFont("Arial", 48, QFont::Bold);
  painter.setFont(titleFont);
  painter.setPen(QColor(0, 150, 0));
  QRect titleRect(0, fullRect.height() / 4, fullRect.width(), 60);
  painter.drawText(titleRect, Qt::AlignHCenter, "VICTORY!");

  QFont infoFont("Arial", 20);
  painter.setFont(infoFont);
  painter.setPen(Qt::black);

  painter.drawText(0, titleRect.bottom() + 40, fullRect.width(), 30,
                   Qt::AlignHCenter,
                   QString("Score: %1").arg(controller_->get_score()));
  painter.drawText(0, titleRect.bottom() + 80, fullRect.width(), 30,
                   Qt::AlignHCenter, "Press ESC to quit");
}

// Экран окончания игры
void SnakeWidget::drawGameOverLabel(QPainter& painter) {
  QRect fullRect = rect();
  QFont titleFont("Arial", 48, QFont::Bold);
  painter.setFont(titleFont);
  painter.setPen(Qt::red);
  QRect gameOverRect(0, fullRect.height() / 4, fullRect.width(), 60);
  painter.drawText(gameOverRect, Qt::AlignHCenter, "GAME OVER");

  QFont infoFont("Arial", 20);
  painter.setFont(infoFont);
  painter.setPen(Qt::black);

  painter.drawText(0, gameOverRect.bottom() + 40, fullRect.width(), 30,
                   Qt::AlignHCenter, "Press SPACE to restart");
  painter.drawText(0, gameOverRect.bottom() + 80, fullRect.width(), 30,
                   Qt::AlignHCenter, "Press ESC to quit");
}

// Основная отрисовка игрового поля
void SnakeWidget::drawGame(QPainter& painter) {
  const int fieldWidth = DOT_SIZE * (WIDTH + 2);
  const int fieldHeight = DOT_SIZE * (HEIGHT + 2);

  painter.setPen(Qt::NoPen);

  for (int y = 0; y < HEIGHT + 2; ++y) {
    for (int x = 0; x < WIDTH + 2; ++x) {
      QRect cellRect(x * DOT_SIZE, y * DOT_SIZE, DOT_SIZE, DOT_SIZE);
      drawCellContents(painter, x, y, cellRect);
    }
  }

  drawFieldBorder(painter, fieldWidth, fieldHeight);
}

// Отрисовка содержимого одной ячейки
void SnakeWidget::drawCellContents(QPainter& painter, int x, int y,
                                   const QRect& cellRect) {
  QColor color = getCellColor(x, y);
  drawCell(painter, x, y, color);

  if (isSnakeHead(x, y)) {
    painter.drawPixmap(cellRect, headPixmap_);
  }

  drawItemPixmap(painter, x, y, cellRect);
  drawCellOutline(painter, x, y, color, cellRect);
  drawFieldInnerBorder(painter, x, y, cellRect);
}

// Получение цвета ячейки
QColor SnakeWidget::getCellColor(int x, int y) {
  if (isSnakeHead(x, y)) return Qt::green;
  if (isTailSegment(x, y)) return Qt::darkGreen;
  if (controller_->get_field(y, x) == 1) return Qt::gray;
  return QColor(245, 222, 179);
}

// Проверка, является ли ячейка головой змейки
bool SnakeWidget::isSnakeHead(int x, int y) {
  return (y == controller_->get_snake_head_y() &&
          x == controller_->get_snake_head_x());
}

// Проверка, является ли ячейка частью хвоста
bool SnakeWidget::isTailSegment(int x, int y) {
  int tailSize = controller_->get_tail_size();
  for (int i = 0; i < tailSize; ++i) {
    if (controller_->get_tail_x(i) == x && controller_->get_tail_y(i) == y)
      return true;
  }
  return false;
}

// Отрисовка изображения предмета в ячейке
void SnakeWidget::drawItemPixmap(QPainter& painter, int x, int y,
                                 const QRect& cellRect) {
  int cell = controller_->get_field(y, x);
  switch (cell) {
    case -1:
      painter.drawPixmap(cellRect, cakePixmap_);
      break;
    case -2:
      painter.drawPixmap(cellRect, diamondPixmap_);
      break;
    case -3:
      painter.drawPixmap(cellRect, grapePixmap_);
      break;
    case -4:
      painter.drawPixmap(cellRect, rubyPixmap_);
      break;
    case -5:
      painter.drawPixmap(cellRect, strawberryPixmap_);
      break;
    default:
      break;
  }
}

// Отрисовка обводки головы и хвоста
void SnakeWidget::drawCellOutline(QPainter& painter, int x, int y,
                                  const QColor& color, const QRect& cellRect) {
  if (isSnakeHead(x, y) || color == Qt::darkGreen) {
    painter.setPen(QPen(Qt::black, 2));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(cellRect.adjusted(1, 1, -1, -1));
    painter.setPen(Qt::NoPen);
  }
}

// Отрисовка границы вокруг поля
void SnakeWidget::drawFieldInnerBorder(QPainter& painter, int x, int y,
                                       const QRect& cellRect) {
  if (x == 0 || x == WIDTH + 1 || y == 0 || y == HEIGHT + 1) {
    painter.setPen(QPen(Qt::darkGray, 2));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(cellRect.adjusted(1, 1, -1, -1));
    // Рисуем линии границы
    painter.setPen(QPen(Qt::darkGray, 1));
    if (x == 0)
      painter.drawLine(cellRect.right() - 1, cellRect.top() + 1,
                       cellRect.right() - 1, cellRect.bottom() - 1);
    else if (x == WIDTH + 1)
      painter.drawLine(cellRect.left() + 1, cellRect.top() + 1,
                       cellRect.left() + 1, cellRect.bottom() - 1);
    if (y == 0)
      painter.drawLine(cellRect.left() + 1, cellRect.bottom() - 1,
                       cellRect.right() - 1, cellRect.bottom() - 1);
    else if (y == HEIGHT + 1)
      painter.drawLine(cellRect.left() + 1, cellRect.top() + 1,
                       cellRect.right() - 1, cellRect.top() + 1);
    painter.setPen(Qt::NoPen);
  }
}

// Общая граница игрового поля
void SnakeWidget::drawFieldBorder(QPainter& painter, int fieldWidth,
                                  int fieldHeight) {
  painter.setPen(QPen(Qt::darkGray, 4));
  painter.setBrush(Qt::NoBrush);
  painter.drawRect(0, 0, fieldWidth, fieldHeight);
}

// Отрисовка одной ячейки с тенью
void SnakeWidget::drawCell(QPainter& painter, int x, int y, QColor color) {
  QRect cellRect(x * DOT_SIZE, y * DOT_SIZE, DOT_SIZE, DOT_SIZE);
  painter.setPen(Qt::NoPen);
  painter.setBrush(QColor(0, 0, 0, 50));  // тень
  painter.drawRect(cellRect.adjusted(2, 2, 2, 2));
  painter.setBrush(color);
  painter.drawRect(cellRect);
}

// Панель информации (очки, уровень и подсказки)
void SnakeWidget::drawInfo(QPainter& painter) {
  int xOffset = (WIDTH + 2) * DOT_SIZE + 30;
  painter.setFont(QFont("Arial", 18, QFont::Bold));
  painter.setPen(Qt::black);
  painter.drawText(
      xOffset, 30,
      QString("High Score: %1").arg(controller_->get_high_score()));
  painter.drawText(xOffset, 60,
                   QString("Score: %1").arg(controller_->get_score()));
  painter.drawText(xOffset, 90,
                   QString("Level: %1").arg(controller_->get_level()));
  painter.drawText(xOffset, 120, QString("Speed: %1").arg(getHRSpeed()));

  painter.setFont(QFont("Arial", 14, QFont::Bold));
  painter.drawText(xOffset, 400, "|^|  MOVE UP");
  painter.drawText(xOffset, 430, "|v|  MOVE DOWN");
  painter.drawText(xOffset, 460, "|<|  MOVE LEFT");
  painter.drawText(xOffset, 490, "|>|  MOVE RIGHT");
  painter.drawText(xOffset, 520, "|P|  PAUSE");
  painter.drawText(xOffset, 550, "|ENTER|  SPEED UP");
  painter.drawText(xOffset, 610, "|ESC|  QUIT");
}

// Человекочитаемое значение скорости
int SnakeWidget::getHRSpeed() {
  int hr_speed = 0;
  for (int i = MAX_DELAY, j = 10; i >= MIN_DELAY; i -= PACE_DELAY, j += 10) {
    if (i == controller_->get_speed()) {
      hr_speed = j;
    }
  }
  return hr_speed;
}

// Экран начала игры
void SnakeWidget::drawStartScreen(QPainter& painter) {
  painter.setPen(Qt::blue);
  painter.setFont(QFont("Arial", 24, QFont::Bold));
  painter.drawText(rect(), Qt::AlignCenter, "Press SPACE to start...");
}

// Надпись "Пауза"
void SnakeWidget::drawPauseLabel(QPainter& painter) {
  QFont font("Arial", 24, QFont::Bold);
  painter.setFont(font);
  QString text = "PAUSE";
  QRect textRect = rect().adjusted(120, 0, 0, 0);
  QPen outlinePen(Qt::black);
  outlinePen.setWidth(2);
  painter.setPen(outlinePen);

  // Отрисовка тени
  for (int dx = -1; dx <= 1; ++dx) {
    for (int dy = -1; dy <= 1; ++dy) {
      if (dx != 0 || dy != 0) {
        painter.drawText(textRect.translated(dx, dy),
                         Qt::AlignVCenter | Qt::AlignLeft, text);
      }
    }
  }

  painter.setPen(Qt::blue);
  painter.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, text);
}

}  // namespace s21
