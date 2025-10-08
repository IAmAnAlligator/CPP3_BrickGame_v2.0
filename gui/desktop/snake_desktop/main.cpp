#include "snake_qt.h"

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  s21::SnakeModel model;
  s21::SnakeController controller(&model);
  s21::SnakeWidget window(&controller);

  window.show();
  window.setFocus();

  return app.exec();
}
