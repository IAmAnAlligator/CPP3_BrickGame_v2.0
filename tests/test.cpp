#include <gtest/gtest.h>

#include "../brick_game/snake/controller/controller.h"
#include "../brick_game/snake/model/model.h"

TEST(model, collision_test) {
  s21::SnakeModel m{};
  bool res;

  m.initialize();
  m.snake_head.y = 0;
  m.snake_head.x = 0;
  res = m.check_collision();
  EXPECT_TRUE(res);

  m.snake_head.y = 2;
  m.snake_head.x = 2;
  m.tail_x[1] = m.snake_head.x;
  m.tail_y[1] = m.snake_head.y;
  res = m.check_collision();
  EXPECT_TRUE(res);

  m.snake_head.y = 5;
  m.snake_head.x = 5;
  m.tail_x[4] = m.snake_head.x;
  m.tail_y[4] = m.snake_head.y;
  res = m.check_collision();
  EXPECT_FALSE(res);
}

TEST(model, eat_fruit_test) {
  s21::SnakeModel model{};
  model.initialize();

  model.snake_head.y = model.fruit.y;
  model.snake_head.x = model.fruit.x;

  model.tail_x[1] = model.fruit.x;
  model.tail_y[1] = model.fruit.y;

  model.collision_to_fruit();

  EXPECT_EQ(model.tail_size, 5);
  EXPECT_EQ(model.game.score, 1);
}

TEST(model, save_score_test) {
  s21::SnakeModel m{};
  m.game.high_score = 20;
  m.write_to_file();
  m.game.high_score = 30;
  m.write_to_file();

  int res = m.load_high_score();
  EXPECT_EQ(res, 30);
}

TEST(model, update_level_test) {
  s21::SnakeModel m{};
  m.initialize();
  bool res;

  m.game.score = 5;
  m.game.high_score = 3;
  res = m.update_level();

  EXPECT_EQ(m.game.level, 2);
  EXPECT_EQ(m.game.speed, 750000);
  EXPECT_FALSE(res);

  m.game.level = FINAL_LEVEL + 1;
  res = m.update_level();
  EXPECT_TRUE(res);

  m.initialize();
  m.tail_size = 200;
  res = m.update_level();
  EXPECT_TRUE(res);
}

TEST(model, speed_limit_test) {
  s21::SnakeModel m{};

  m.game.score = 1000;
  m.game.level = 1;
  m.game.speed = MIN_DELAY - 10;
  m.tail_size = 10;
  m.game.high_score = 0;

  bool result = m.update_level();

  EXPECT_EQ(m.game.speed, MIN_DELAY);
  EXPECT_FALSE(result);
}

TEST(model, move_tail_test) {
  s21::SnakeModel m{};
  m.initialize();
  m.move_tail();
  EXPECT_EQ(m.tail_x[0], m.snake_head.x);
  EXPECT_EQ(m.tail_y[0], m.snake_head.y);
}

TEST(model, place_on_head_test) {
  s21::SnakeModel m{};
  m.snake_head = {5, 5};
  m.tail_size = 0;
  std::memset(m.game.field, 0, sizeof(m.game.field));

  bool fruit_placed_on_head = false;
  for (int attempts = 0; attempts < 100 && !fruit_placed_on_head; ++attempts) {
    m.place_fruit();
    if (m.fruit.x == m.snake_head.x && m.fruit.y == m.snake_head.y) {
      fruit_placed_on_head = true;
    }
  }

  EXPECT_FALSE(fruit_placed_on_head);
}

TEST(model, place_on_tail_test) {
  s21::SnakeModel m{};

  // Настройка головы и хвоста змеи
  m.snake_head = {1, 1};  // где угодно, но не на хвосте
  m.tail_size = 3;
  m.tail_x[0] = 4;
  m.tail_y[0] = 4;  // Занятая хвостом клетка
  m.tail_x[1] = 5;
  m.tail_y[1] = 5;
  m.tail_x[2] = 6;
  m.tail_y[2] = 6;

  std::memset(m.game.field, 0, sizeof(m.game.field));  // Очистить поле

  // Многократно проверим, размещается ли фрукт на хвосте
  bool fruit_on_tail = false;
  for (int i = 0; i < 100 && !fruit_on_tail; ++i) {
    m.place_fruit();
    for (int j = 0; j < m.tail_size && !fruit_on_tail; ++j) {
      if (m.fruit.x == m.tail_x[j] && m.fruit.y == m.tail_y[j]) {
        fruit_on_tail = true;
        // break;
      }
    }
    // if (fruit_on_tail) break;
  }

  EXPECT_FALSE(fruit_on_tail);
}

TEST(model, start_test) {
  s21::SnakeModel m{};
  m.initialize();
  m.start();
  EXPECT_EQ(m.game.pause, s21::GameState::START);
}

TEST(model, restart_test) {
  s21::SnakeModel m{};
  m.initialize();
  m.restart();
  EXPECT_EQ(m.game.pause, s21::GameState::PLAY);
}

TEST(model, constant_movement_test) {
  s21::SnakeModel m{};
  m.initialize();
  m.direction = 3;
  m.constant_movement();
  EXPECT_EQ(m.snake_head.x, 5);

  m.direction = 4;
  m.constant_movement();
  EXPECT_EQ(m.snake_head.x, 6);

  m.direction = 5;
  m.constant_movement();
  EXPECT_EQ(m.snake_head.y, 9);

  m.direction = 6;
  m.constant_movement();
  EXPECT_EQ(m.snake_head.y, 10);
}

TEST(model, update_current_state_test) {
  s21::SnakeModel m{};
  m.initialize();
  m.restart();
  m.updateCurrentState();
  EXPECT_EQ(m.game.pause, s21::GameState::PLAY);

  m.snake_head.y = 0;
  m.snake_head.x = 0;
  m.updateCurrentState();
  EXPECT_EQ(m.game.pause, s21::GameState::GAME_OVER);

  m.restart();
  m.game.level = FINAL_LEVEL + 1;
  m.updateCurrentState();
  EXPECT_EQ(m.game.pause, s21::GameState::VICTORY);
}

TEST(model, controller_input_test) {
  s21::SnakeModel m{};
  s21::SnakeController c{&m};

  m.initialize();

  c.userInput(s21::Start, 1);
  EXPECT_EQ(m.game.pause, s21::GameState::PLAY);

  c.userInput(s21::Left, 1);
  EXPECT_EQ(m.direction, 3);

  c.userInput(s21::Up, 1);
  EXPECT_EQ(m.direction, 5);

  c.userInput(s21::Right, 1);
  EXPECT_EQ(m.direction, 4);

  c.userInput(s21::Down, 1);
  EXPECT_EQ(m.direction, 6);

  c.userInput(s21::Action, 1);
  EXPECT_EQ(m.game.speed, 750000);

  c.userInput(s21::Pause, 1);
  EXPECT_EQ(m.game.pause, s21::GameState::PAUSE);

  c.userInput(s21::Pause, 1);
  EXPECT_EQ(m.game.pause, s21::GameState::PLAY);

  c.userInput(s21::Terminate, 1);
  EXPECT_EQ(m.game.pause, s21::GameState::EXIT);
}

int main(int argc, char *argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}