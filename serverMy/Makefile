CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -O2 -g
INCLUDES = -I./include
LIBS = -lssl -lcrypto -lpthread

# Основной сервер
SRC = src/Server.cpp src/ClientDB.cpp src/Logger.cpp src/Protocol.cpp src/VectorProcessor.cpp
OBJ = $(SRC:src/%.cpp=obj/%.o)
EXEC = bin/vcalc_server

# Тесты
TEST_DIR = tests
# Добавим новые тестовые файлы в TEST_SRC
TEST_SRC = $(TEST_DIR)/test_main.cpp \
           $(TEST_DIR)/test_clientdb.cpp \
           $(TEST_DIR)/test_vectorprocessor.cpp \
           $(TEST_DIR)/test_logger.cpp \
           $(TEST_DIR)/test_config.cpp \
           $(TEST_DIR)/test_protocol.cpp \
           $(TEST_DIR)/test_server.cpp \
           $(TEST_DIR)/test_filesystem.cpp
TEST_OBJ = $(TEST_SRC:$(TEST_DIR)/%.cpp=obj/test_%.o)
TEST_EXEC = bin/run_tests

# Правила по умолчанию
all: $(EXEC)

# Основной сервер (с отдельным main.cpp)
$(EXEC): obj/main.o $(OBJ)
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) obj/main.o $(OBJ) -o $@ $(LIBS)

obj/main.o: src/main.cpp
	@mkdir -p obj
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

obj/%.o: src/%.cpp
	@mkdir -p obj
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Тесты (без obj/main.o)
test: $(TEST_EXEC)
	@echo "Running tests..."
	./$(TEST_EXEC)

$(TEST_EXEC): $(TEST_OBJ) $(OBJ)  # Только объектные файлы библиотек
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) $(TEST_OBJ) $(OBJ) -o $@ $(LIBS)

obj/test_%.o: tests/%.cpp
	@mkdir -p obj
	$(CXX) $(CXXFLAGS) $(INCLUDES) -I$(TEST_DIR) -c $< -o $@

# Запуск сервера
run: $(EXEC)
	./$(EXEC) -c ./data/clients.conf -l ./vcalc.log -p 33333

# Настройка тестовой среды
setup:
	@mkdir -p data obj bin
	@echo "user:P@ssW0rd" > data/clients.conf
	@echo "testuser:testpass" >> data/clients.conf
	@echo "# Тестовый пользователь" >> data/clients.conf
	@echo "Файл конфигурации создан"

# Очистка
clean:
	rm -rf obj bin *.log
	find . -name "*.tmp" -delete 2>/dev/null || true

.PHONY: all test run setup clean
