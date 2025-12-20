# ============================================================================
# VECTOR CALCULATOR SERVER MAKEFILE
# ============================================================================

# Компилятор и флаги
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -O2 -g
INCLUDES = -I./include -I/usr/include/UnitTest++
LIBS = -lssl -lcrypto -lpthread -lUnitTest++

# ============================================================================
# ОСНОВНОЙ СЕРВЕР
# ============================================================================

SRC_DIR = src
SRC = $(SRC_DIR)/main.cpp \
      $(SRC_DIR)/Server.cpp \
      $(SRC_DIR)/ClientDB.cpp \
      $(SRC_DIR)/Logger.cpp \
      $(SRC_DIR)/Protocol.cpp \
      $(SRC_DIR)/VectorProcessor.cpp

OBJ_DIR = obj
OBJ = $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

EXEC_DIR = bin
EXEC = $(EXEC_DIR)/vcalc_server

# ============================================================================
# ТЕСТЫ С UnitTest++
# ============================================================================

# Unit тесты
UNIT_TEST_DIR = tests/unit
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

# Functional тесты
FUNC_TEST_DIR = tests/functional
FUNC_TEST_SRC = $(FUNC_TEST_DIR)/test_main.cpp \
                $(FUNC_TEST_DIR)/test_auth_flow.cpp \
                $(FUNC_TEST_DIR)/test_vectors_flow.cpp

FUNC_TEST_OBJ = $(FUNC_TEST_SRC:$(FUNC_TEST_DIR)/%.cpp=$(OBJ_DIR)/func_%.o)

FUNC_TEST_EXEC = $(EXEC_DIR)/run_func_tests

# Общие объектные файлы (без main.o сервера)
COMMON_OBJ = $(filter-out $(OBJ_DIR)/main.o, $(OBJ))

# ============================================================================
# ПРАВИЛА
# ============================================================================

.PHONY: all build server run setup clean test unit-test func-test help

all: build

help:
	@echo "Использование: make [цель]"
	@echo ""
	@echo "Цели:"
	@echo "  all        - Собрать сервер"
	@echo "  build      - Собрать сервер"
	@echo "  server     - Собрать сервер (аналогично build)"
	@echo "  run        - Собрать и запустить сервер"
	@echo "  setup      - Создать тестовую конфигурацию"
	@echo ""
	@echo "  test       - Собрать и запустить все тесты"
	@echo "  unit-test  - Собрать и запустить unit тесты"
	@echo "  func-test  - Собрать и запустить functional тесты"
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
# UNIT ТЕСТЫ
# ============================================================================

unit-test: $(UNIT_TEST_EXEC)
	@echo "========================================"
	@echo "Запуск unit тестов..."
	@echo "========================================"
	@./$(UNIT_TEST_EXEC)

$(UNIT_TEST_EXEC): $(UNIT_TEST_OBJ) $(COMMON_OBJ)
	@mkdir -p $(EXEC_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBS)

$(OBJ_DIR)/unit_%.o: $(UNIT_TEST_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# ============================================================================
# FUNCTIONAL ТЕСТЫ
# ============================================================================

func-test: $(FUNC_TEST_EXEC)
	@echo "========================================"
	@echo "Запуск functional тестов..."
	@echo "========================================"
	@./$(FUNC_TEST_EXEC)

$(FUNC_TEST_EXEC): $(FUNC_TEST_OBJ) $(COMMON_OBJ)
	@mkdir -p $(EXEC_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBS)

$(OBJ_DIR)/func_%.o: $(FUNC_TEST_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# ============================================================================
# ВСЕ ТЕСТЫ
# ============================================================================

test: unit-test func-test

# ============================================================================
# ЗАПУСК И НАСТРОЙКА
# ============================================================================

run: build
	@echo "Запуск сервера..."
	@echo "Для остановки нажмите Ctrl+C"
	@echo "========================================"
	@./$(EXEC) -c ./data/clients.conf -l ./vcalc.log -p 33333

setup:
	@mkdir -p data $(OBJ_DIR) $(EXEC_DIR)
	@echo "Создание тестовой конфигурации..."
	@echo "user:P@ssW0rd" > data/clients.conf
	@echo "testuser:testpass" >> data/clients.conf
	@echo "testadmin:admin123" >> data/clients.conf
	@echo "Файл клиентов создан: data/clients.conf"

# ============================================================================
# ОЧИСТКА
# ============================================================================

clean:
	@echo "Очистка скомпилированных файлов..."
	rm -rf $(OBJ_DIR) $(EXEC_DIR)
	@find . -name "*.log" -type f -delete 2>/dev/null || true
	@find /tmp -name "test_*.conf" -type f -delete 2>/dev/null || true
	@find /tmp -name "test_*.log" -type f -delete 2>/dev/null || true
	@echo "Очистка завершена!"
