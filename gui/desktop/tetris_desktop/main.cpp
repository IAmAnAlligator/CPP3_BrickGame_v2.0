#include <QApplication>

#include "tetris_qt.h"

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  TetrisWidget tetris;
  tetris.show();
  return app.exec();
}
