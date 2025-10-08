# Установите зависимости:
# sudo apt-get install libsdl2-dev libsdl2-mixer-dev lcov doxygen

CC = gcc -std=c11
CPP = g++ -std=c++20

CFLAGS = -Wall -Wextra -Werror -Wpedantic -I.
BUILD_DIR = ./build
TETRIS_SRC = ./brick_game/tetris/backend.c
TETRIS_CLI_SRC = ./gui/cli/interface/frontend.c

SNAKE_MODEL = brick_game/snake/model/*.cpp
SNAKE_CONTROLLER = brick_game/snake/controller/*.cpp
SNAKE_VIEW = gui/cli/view/*.cpp

SDL = -lSDL2 -lSDL2_mixer
MATH = -lm
LIBS = -lcheck -lncurses -lrt -lpthread -lsubunit $(MATH)
TST_CFLAGS := -g $(CFLAGS)
GCOV_FLAGS = -fprofile-arcs -ftest-coverage
COVERAGE_LIBS = -lgcov
VALGRIND_FLAGS = --trace-children=yes --track-fds=yes --track-origins=yes --leak-check=full --show-leak-kinds=all --verbose

QT_TETRIS_PROJECT_DIR = ./gui/desktop/tetris_desktop/
QT_TETRIS = $(QT_TETRIS_PROJECT_DIR)t_desktop.pro

QT_SNAKE_PROJECT_DIR = ./gui/desktop/snake_desktop/
QT_SNAKE = $(QT_SNAKE_PROJECT_DIR)s_desktop.pro

.PHONY: all clean rebuild test tetris snake tetris_qt snake_qt format dvi gcov_report

all: snake test

# Установка
install: tetris_qt snake_qt tetris snake

# Сборка Qt-проекта Tetris
tetris_qt:
	@echo "Generating Makefile for Qt Tetris..."
	mkdir -p $(BUILD_DIR)/temp
	qmake $(QT_TETRIS) -o $(BUILD_DIR)/temp/Makefile
	$(MAKE) -C $(BUILD_DIR)/temp/
	cp $(BUILD_DIR)/temp/tetris_desktop $(BUILD_DIR)/
	rm -rf $(BUILD_DIR)/temp

# Сборка Qt-проекта Snake
snake_qt:
	@echo "Generating Makefile for Qt Snake..."
	mkdir -p $(BUILD_DIR)/temp
	qmake $(QT_SNAKE) -o $(BUILD_DIR)/temp/Makefile
	$(MAKE) -C $(BUILD_DIR)/temp/
	cp $(BUILD_DIR)/temp/snake_desktop $(BUILD_DIR)/
	rm -rf $(BUILD_DIR)/temp
	@echo "Run with: ./build/snake_desktop"

# Сборка Tetris CLI
tetris: $(BUILD_DIR)/tetris

$(BUILD_DIR)/tetris: $(TETRIS_SRC) $(TETRIS_CLI_SRC)
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) $(SDL)

# Сборка Snake CLI
snake: $(BUILD_DIR)/snake

$(BUILD_DIR)/snake: $(SNAKE_MODEL) $(SNAKE_CONTROLLER) $(SNAKE_VIEW)
	mkdir -p $(BUILD_DIR)
	$(CPP) $(CFLAGS) -o $@ $^ $(LIBS)	

# Сборка и запуск тестов + покрытия
test: clean
	$(CPP) $(CFLAGS) $(GCOV_FLAGS) $(SNAKE_MODEL) $(SNAKE_CONTROLLER) tests/*.cpp -o test -lgtest $(COVERAGE_LIBS)
	./test
	@$(MAKE) gcov_report
	lcov --capture --directory . --output-file coverage.info --ignore-errors mismatch
	genhtml coverage.info --output-directory coverage
	@echo "HTML report generated at ./coverage/index.html"
	rm -rf snake_score.txt
	open ./coverage/index.html

# Генерация .gcov-файлов вручную
gcov_report:
	@echo "Generating .gcov reports..."
	find . -name "*.gcda" -exec gcov -pb {} +
	@echo "GCOV reports (*.gcov) created in current and subdirectories"

valgrind: snake
	$(CPP) $(CFLAGS) $(GCOV_FLAGS) $(SNAKE_MODEL) $(SNAKE_CONTROLLER) tests/*.cpp -o val_test.out -lgtest -lgcov $(LIBS)
	CK_FORK=no valgrind $(VALGRIND_FLAGS) --log-file=RESULT_VALGRIND.txt ./val_test.out

# Генерация документации
dvi:
	doxygen Doxyfile
	open html/index.html

dist: clean
	mkdir CPP3_BrickGame_v2.0
	cp -r ./Makefile ./tests ./gui ./brick_game ./Doxyfile CPP3_BrickGame_v2.0
	tar -czf CPP3_BrickGame_v2.0.tar.gz CPP3_BrickGame_v2.0/
	rm -rf CPP3_BrickGame_v2.0	

# Автоформатирование
format:
	clang-format -style=Google -i brick_game/tetris/*.c brick_game/tetris/*.h
	clang-format -style=Google -i brick_game/snake/snake.h
	clang-format -style=Google -i brick_game/snake/controller/*.cpp brick_game/snake/controller/*.h
	clang-format -style=Google -i brick_game/snake/model/*.cpp brick_game/snake/model/*.h
	clang-format -style=Google -i gui/cli/interface/*.c
	clang-format -style=Google -i gui/cli/view/*.cpp gui/cli/view/*.h
	clang-format -style=Google -i gui/desktop/snake_desktop/*.cpp gui/desktop/snake_desktop/*.h
	clang-format -style=Google -i gui/desktop/tetris_desktop/*.cpp gui/desktop/tetris_desktop/*.h 
	clang-format -style=Google -i tests/*.cpp

# Удаление
uninstall:
	rm -rf $(BUILD_DIR)

# Очистка
clean:
	rm -f *.o *.txt *.gcno *.gcda *.gcov *.tar.gz *.out
	rm -rf test coverage coverage.info snake_score.txt
	rm -rf html

# Пересборка
rebuild: clean all
