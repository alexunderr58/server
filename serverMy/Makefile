# ============================================================================
# VECTOR CALCULATOR SERVER MAKEFILE
# ============================================================================

# Компилятор и флаги
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -O2 -g
INCLUDES = -I./include
LIBS = -lssl -lcrypto -lpthread

# ============================================================================
# ОСНОВНОЙ СЕРВЕР
# ============================================================================

# Исходные файлы сервера
SRC_DIR = src
SRC = $(SRC_DIR)/main.cpp \
      $(SRC_DIR)/Server.cpp \
      $(SRC_DIR)/ClientDB.cpp \
      $(SRC_DIR)/Logger.cpp \
      $(SRC_DIR)/Protocol.cpp \
      $(SRC_DIR)/VectorProcessor.cpp

# Объектные файлы
OBJ_DIR = obj
OBJ = $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Исполняемый файл
EXEC_DIR = bin
EXEC = $(EXEC_DIR)/vcalc_server

# ============================================================================
# ТЕСТЫ
# ============================================================================

# Модульные тесты
UNIT_TEST_DIR = tests
UNIT_TEST_SRC = $(UNIT_TEST_DIR)/test_main.cpp \
                $(UNIT_TEST_DIR)/test_clientdb.cpp \
                $(UNIT_TEST_DIR)/test_vectorprocessor.cpp \
                $(UNIT_TEST_DIR)/test_logger.cpp \
                $(UNIT_TEST_DIR)/test_config.cpp \
                $(UNIT_TEST_DIR)/test_protocol.cpp \
                $(UNIT_TEST_DIR)/test_server.cpp \
                $(UNIT_TEST_DIR)/test_filesystem.cpp

UNIT_TEST_OBJ = $(UNIT_TEST_SRC:$(UNIT_TEST_DIR)/%.cpp=$(OBJ_DIR)/unit_%.o)
UNIT_TEST_EXEC = $(EXEC_DIR)/run_unit_tests

# Функциональные тесты
FUNC_TEST_DIR = tests/functional
FUNC_TEST_SRC = $(FUNC_TEST_DIR)/test_main.cpp \
                $(FUNC_TEST_DIR)/test_auth_flow.cpp \
                $(FUNC_TEST_DIR)/test_vectors_flow.cpp

FUNC_TEST_OBJ = $(FUNC_TEST_SRC:$(FUNC_TEST_DIR)/%.cpp=$(OBJ_DIR)/func_%.o)
FUNC_TEST_EXEC = $(EXEC_DIR)/run_func_tests

# ============================================================================
# ПРАВИЛА
# ============================================================================

.PHONY: all build server run setup clean test unit-test func-test help

all: build

help:
	@echo "Использование: make [цель]"
	@echo ""
	@echo "Цели:"
	@echo "  all        - Собрать сервер и все тесты"
	@echo "  build      - Собрать сервер"
	@echo "  server     - Собрать сервер (аналогично build)"
	@echo "  run        - Собрать и запустить сервер"
	@echo "  setup      - Создать тестовую конфигурацию"
	@echo ""
	@echo "  test       - Запустить все тесты"
	@echo "  unit-test  - Запустить только модульные тесты"
	@echo "  func-test  - Запустить только функциональные тесты"
	@echo ""
	@echo "  clean      - Очистить скомпилированные файлы"
	@echo "  help       - Показать эту справку"

# ============================================================================
# СБОРКА СЕРВЕРА
# ============================================================================

build: $(EXEC)

server: build

$(EXEC): $(OBJ)
	@mkdir -p $(EXEC_DIR)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $@ $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# ============================================================================
# ТЕСТЫ
# ============================================================================

test: unit-test func-test

unit-test: $(UNIT_TEST_EXEC)
	@./$(UNIT_TEST_EXEC)

$(UNIT_TEST_EXEC): $(UNIT_TEST_OBJ) $(filter-out $(OBJ_DIR)/main.o, $(OBJ))
	@mkdir -p $(EXEC_DIR)
	$(CXX) $(CXXFLAGS) $(UNIT_TEST_OBJ) $(filter-out $(OBJ_DIR)/main.o, $(OBJ)) -o $@ $(LIBS)

$(OBJ_DIR)/unit_%.o: $(UNIT_TEST_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -Itests -c $< -o $@

func-test: $(FUNC_TEST_EXEC)
	@./$(FUNC_TEST_EXEC)

$(FUNC_TEST_EXEC): $(FUNC_TEST_OBJ) $(filter-out $(OBJ_DIR)/main.o, $(OBJ))
	@mkdir -p $(EXEC_DIR)
	$(CXX) $(CXXFLAGS) $(FUNC_TEST_OBJ) $(filter-out $(OBJ_DIR)/main.o, $(OBJ)) -o $@ $(LIBS)

$(OBJ_DIR)/func_%.o: $(FUNC_TEST_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -Itests -c $< -o $@

# ============================================================================
# ЗАПУСК И НАСТРОЙКА
# ============================================================================

run: build
	@./$(EXEC) -c ./data/clients.conf -l ./vcalc.log -p 33333

setup:
	@mkdir -p data $(OBJ_DIR) $(EXEC_DIR)
	@echo "user:P@ssW0rd" > data/clients.conf
	@echo "testuser:testpass" >> data/clients.conf

# ============================================================================
# ОЧИСТКА
# ============================================================================

clean:
	rm -rf $(OBJ_DIR) $(EXEC_DIR) *.log
	find . -name "*.tmp" -delete 2>/dev/null || true
