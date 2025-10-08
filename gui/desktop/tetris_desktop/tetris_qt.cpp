#include "tetris_qt.h"

// Конструктор виджета TetrisWidget
TetrisWidget::TetrisWidget(QWidget *parent) : QWidget(parent) {
  setWindowTitle("Tetris");
  // Установка размера окна
  resize(DOT_SIZE * (FIELD_WIDTH - 2) * 2, DOT_SIZE * (FIELD_HEIGHT));

  // Настройка таймера и соединение с игровым обновлением
  timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this, &TetrisWidget::updateGame);

  setupLayout();      // Создание компоновки UI
  init_game(&state);  // Инициализация состояния игры
  state.state = START;  // Установка начального состояния

  music = nullptr;
  play_music(&music, AUDIO_FILE);  // Воспроизведение музыки
}

// Деструктор TetrisWidget
TetrisWidget::~TetrisWidget() {
  stop_music();  // Остановка музыки
  if (music) {
    Mix_FreeMusic(music);
    music = nullptr;
  }
  Mix_CloseAudio();
  SDL_Quit();
  delete timer;
}

// Обновление состояния игры, вызывается таймером
void TetrisWidget::updateGame() {
  if (state.state == PLAY) {
    move_and_level_check(&state, &timerCount);  // Логика движения и уровня
    update();                                   // Перерисовка
  }
}

// Настройка визуального интерфейса и панелей информации
void TetrisWidget::setupLayout() {
  QFont f1("Arial", 14, QFont::Bold);
  QFont bigFont("Arial", 18, QFont::Bold);

  QHBoxLayout *mainLayout = new QHBoxLayout(this);
  mainLayout->setContentsMargins(120, 0, 120, 0);

  QWidget *gameField = new QWidget(this);
  gameField->setFixedSize(DOT_SIZE * (FIELD_WIDTH - 2),
                          DOT_SIZE * (FIELD_HEIGHT));
  gameField->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

  infoWidget = new QWidget(this);
  QVBoxLayout *infoLayout = new QVBoxLayout(infoWidget);

  // Создание всех меток
  scoreLabel = new QLabel();
  highScoreLabel = new QLabel();
  levelLabel = new QLabel();
  next = new QLabel();
  drop = new QLabel();
  rotate = new QLabel();
  moveLeft = new QLabel();
  moveRight = new QLabel();
  muteMusic = new QLabel();
  pauseInfo = new QLabel();
  quit = new QLabel();
  pause = new QLabel();
  victory = new QLabel();

  // Добавление меток в layout
  addLabel(infoLayout, scoreLabel, f1);
  addLabel(infoLayout, highScoreLabel, f1);
  addLabel(infoLayout, levelLabel, f1);
  addLabel(infoLayout, next, f1);

  QSpacerItem *spacer =
      new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
  infoLayout->addItem(spacer);

  addLabel(infoLayout, drop, f1);
  addLabel(infoLayout, rotate, f1);
  addLabel(infoLayout, moveLeft, f1);
  addLabel(infoLayout, moveRight, f1);
  addLabel(infoLayout, muteMusic, f1);
  addLabel(infoLayout, pauseInfo, f1);
  addLabel(infoLayout, quit, f1);
  addLabel(infoLayout, pause, f1);
  addLabel(infoLayout, victory, bigFont);

  mainLayout->addWidget(gameField);
  mainLayout->addWidget(infoWidget);
}

// Метод отрисовки интерфейса в зависимости от состояния игры
void TetrisWidget::paintEvent(QPaintEvent *e) {
  Q_UNUSED(e);
  switch (state.state) {
    case START:
      drawStartScreen();
      break;
    case GAME_OVER:
      drawGameOverScreen();
      break;
    case PAUSE:
      drawFigures();
      drawPauseScreen();
      break;
    case PLAY:
      infoWidget->show();
      drawFigures();
      printInformation();
      break;
    default:
      break;
  }
}

// Отрисовка стартового экрана
void TetrisWidget::drawStartScreen() {
  QPainter p(this);
  p.setPen(Qt::blue);
  p.setFont(QFont("Arial", 24, QFont::Bold));
  p.drawText(rect(), Qt::AlignCenter, "PRESS SPACE TO START");
}

// Отрисовка экрана паузы
void TetrisWidget::drawPauseScreen() {
  QPainter p(this);
  p.setPen(Qt::yellow);
  p.setFont(QFont("Arial", 24, QFont::Bold));

  int offsetX = 120;
  QRect textRect = rect();
  textRect.moveLeft(textRect.left() + offsetX);

  p.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, "PAUSE");
}

// Отрисовка экрана окончания игры
void TetrisWidget::drawGameOverScreen() {
  infoWidget->hide();

  QPainter p(this);
  p.fillRect(rect(), Qt::white);

  p.setPen(Qt::red);
  p.setFont(QFont("Arial", 24, QFont::Bold));
  p.drawText(rect(), Qt::AlignCenter,
             "GAME OVER\nPress SPACE to restart\nPress Q to quit");
}

// Обработка нажатия клавиш
void TetrisWidget::keyPressEvent(QKeyEvent *event) {
  switch (event->key()) {
    case Qt::Key_Space:
      if (state.state == START) {
        state.state = PLAY;
        timer->start(80);
      } else if (state.state == GAME_OVER) {
        restartGame();
      }
      break;
    case Qt::Key_Q:
      if (state.state == GAME_OVER || PLAY) close();
      break;
    case Qt::Key_Left:
      if (state.state == PLAY) {
        move_figure_horizontal(&state, -1);
      }
      break;
    case Qt::Key_Right:
      if (state.state == PLAY) {
        move_figure_horizontal(&state, 1);
      }
      break;
    case Qt::Key_Up:
      if (state.state == PLAY) {
        rotate_figure(&state);
      }
      break;
    case Qt::Key_Down:
      if (!state.is_figure_fixed && state.state == PLAY) {
        hard_drop(&state);
      }
      break;
    case Qt::Key_S:
      if (!state.mute) {
        pause_music();
        state.mute = true;
      } else {
        resume_music();
        state.mute = false;
      }
      break;
    case Qt::Key_P:
      if (state.state == PAUSE) {
        state.state = PLAY;
      } else if (state.state == PLAY) {
        state.state = PAUSE;
      }
      break;
    default:
      break;
  }
  update();  // Перерисовка после изменения состояния
}

// Отрисовка игрового поля и фигур
void TetrisWidget::drawFigures() {
  QPainter p(this);
  p.setRenderHint(QPainter::Antialiasing);

  // Отрисовка ячеек поля
  for (int y = 0; y < FIELD_HEIGHT; ++y) {
    for (int x = 0; x < FIELD_WIDTH; ++x) {
      QRect cellRect(x * DOT_SIZE, y * DOT_SIZE, DOT_SIZE, DOT_SIZE);
      if (y == 0 || y == FIELD_HEIGHT - 1 || x == 0 || x == FIELD_WIDTH - 1) {
        p.setBrush(QColor(0, 150, 0));  // Границы
      } else if (state.field[y][x]) {
        p.setBrush(QColor(211, 211, 211));  // Заполненные ячейки
      } else {
        p.setBrush(Qt::black);  // Пустые ячейки
      }
      p.drawRect(cellRect);
    }
  }

  // Отрисовка текущей фигуры
  for (int i = 0; i < FIGURE_SIZE; ++i) {
    for (int j = 0; j < FIGURE_SIZE; ++j) {
      if (state.current_figure[i][j]) {
        int x = state.figure_x + j;
        int y = state.figure_y + i;
        if (x >= 0 && x < FIELD_WIDTH && y >= 0 && y < FIELD_HEIGHT) {
          p.setBrush(Qt::blue);
          p.drawRect(x * DOT_SIZE, y * DOT_SIZE, DOT_SIZE, DOT_SIZE);
        }
      }
    }
  }

  // Отрисовка следующей фигуры сбоку
  int nextOffsetX = FIELD_WIDTH * DOT_SIZE + 80;
  int nextOffsetY = 130;
  for (int i = 0; i < FIGURE_SIZE; ++i) {
    for (int j = 0; j < FIGURE_SIZE; ++j) {
      if (state.next_figure[i][j]) {
        p.setBrush(QColor(200, 100, 200));
        p.drawRect(nextOffsetX + j * DOT_SIZE, nextOffsetY + i * DOT_SIZE,
                   DOT_SIZE, DOT_SIZE);
      }
    }
  }
}

// Обновление текста на информационных метках
void TetrisWidget::printInformation() {
  scoreLabel->setText(QString("SCORE: %1").arg(state.score));
  highScoreLabel->setText(QString("MAX: %1").arg(state.high_score));
  levelLabel->setText(QString("LEVEL: %1").arg(state.level));
  next->setText("NEXT:");
  drop->setText("|v| DROP");
  rotate->setText("|^| ROTATE");
  moveLeft->setText("|<| MOVE LEFT");
  moveRight->setText("|>| MOVE RIGHT");
  muteMusic->setText("|s| MUTE MUSIC");
  pauseInfo->setText("|p| PAUSE");
  quit->setText("|q| QUIT");
}

// Перезапуск игры (после проигрыша)
void TetrisWidget::restartGame() {
  init_game(&state);
  state.state = PLAY;
  timerCount = 0;
  timer->start(80);
  update();
}

// Добавление метки с заданным шрифтом в layout
void TetrisWidget::addLabel(QVBoxLayout *layout, QLabel *label,
                            const QFont &font) {
  label->setFont(font);
  layout->addWidget(label);
}
