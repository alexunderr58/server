# Компилятор и флаги
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -O2
INCLUDES = -I./include -I/usr/include/openssl
LDFLAGS = -lssl -lcrypto -lpthread

# Директории
SRCDIR = src
OBJDIR = obj
BINDIR = bin
INCLUDEDIR = include

# Исходные файлы
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SOURCES))
EXECUTABLE = $(BINDIR)/vcalc_server

# Основная цель
all: $(EXECUTABLE)

# Создание исполняемого файла
$(EXECUTABLE): $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)

# Компиляция объектных файлов
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Очистка
clean:
	rm -rf $(OBJDIR) $(BINDIR)

# Установка (требует прав root)
install: $(EXECUTABLE)
	cp $(EXECUTABLE) /usr/local/bin/vcalc_server
	@echo "Сервер установлен в /usr/local/bin/vcalc_server"
	@echo "Не забудьте создать конфигурационные файлы:"
	@echo "  sudo mkdir -p /etc"
	@echo "  echo 'user:P@ssW0rd' | sudo tee /etc/vcalc.conf"
	@echo "  sudo mkdir -p /var/log"

# Создание пакета для распространения
dist: clean
	mkdir -p dist/vcalc_server
	cp -r include src Makefile README.md data dist/vcalc_server/
	tar -czf vcalc_server.tar.gz -C dist .
	rm -rf dist

# Запуск тестового сервера (локально)
run: $(EXECUTABLE)
	./$(EXECUTABLE) -c ./data/clients.conf -l ./logs/vcalc.log -p 33333

# Отладочная сборка
debug: CXXFLAGS += -g -DDEBUG
debug: clean $(EXECUTABLE)

# Статический анализ
check:
	cppcheck --enable=all --suppress=missingIncludeSystem $(SRCDIR) $(INCLUDEDIR)

# Зависимости для каждого объектного файла
$(OBJDIR)/main.o: $(INCLUDEDIR)/Server.h $(INCLUDEDIR)/Config.h
$(OBJDIR)/Server.o: $(INCLUDEDIR)/Server.h $(INCLUDEDIR)/Logger.h $(INCLUDEDIR)/ClientDB.h $(INCLUDEDIR)/Protocol.h $(INCLUDEDIR)/VectorProcessor.h $(INCLUDEDIR)/Config.h
$(OBJDIR)/ClientDB.o: $(INCLUDEDIR)/ClientDB.h $(INCLUDEDIR)/Config.h
$(OBJDIR)/Logger.o: $(INCLUDEDIR)/Logger.h
$(OBJDIR)/Protocol.o: $(INCLUDEDIR)/Protocol.h $(INCLUDEDIR)/Config.h
$(OBJDIR)/VectorProcessor.o: $(INCLUDEDIR)/VectorProcessor.h

.PHONY: all clean install dist run debug check
