
TARGET = CG_Project


SRC_DIR = src
SRCS = $(SRC_DIR)/main.c $(SRC_DIR)/transforms.c $(SRC_DIR)/input.c $(SRC_DIR)/rendering.c


CC = gcc

# Определение архитектуры и ОС
UNAME_S := $(shell uname -s)
UNAME_M := $(shell uname -m)

# Базовые флаги компилятора
CFLAGS = -Wall -Wextra -std=c99 -O2
LDFLAGS = -lX11 -lm

# Настройки для macOS
ifeq ($(UNAME_S),Darwin)
    # Определение архитектуры на macOS
    ifeq ($(UNAME_M),arm64)
        # Apple Silicon (M1/M2/M3)
        X11_INCLUDE = /opt/homebrew/include
        X11_LIB = /opt/homebrew/lib
        DISPLAY_CMD = export DISPLAY=:0 && open -a XQuartz 
        CLEANUP_CMD = pkill -f XQuartz || true
    else
        # Intel Mac
        X11_INCLUDE = /opt/X11/include
        X11_LIB = /opt/X11/lib
        DISPLAY_CMD = export DISPLAY=:0 && open -a XQuartz && sleep 2
        CLEANUP_CMD = pkill -f XQuartz || true
    endif
    
    # Проверка наличия pkg-config
    PKG_CONFIG_EXISTS := $(shell command -v pkg-config 2> /dev/null)
    ifdef PKG_CONFIG_EXISTS
        PKG_CONFIG_X11 := $(shell pkg-config --exists x11 2> /dev/null && echo yes || echo no)
        ifeq ($(PKG_CONFIG_X11),yes)
            CFLAGS += $(shell pkg-config --cflags x11)
            LDFLAGS = $(shell pkg-config --libs x11) -lm
        else
            CFLAGS += -I$(X11_INCLUDE)
            LDFLAGS = -L$(X11_LIB) -lX11 -lm
        endif
    else
        CFLAGS += -I$(X11_INCLUDE)
        LDFLAGS = -L$(X11_LIB) -lX11 -lm
    endif
endif

# Настройки для Linux
ifeq ($(UNAME_S),Linux)
    # Проверка наличия pkg-config
    PKG_CONFIG_EXISTS := $(shell command -v pkg-config 2> /dev/null)
    ifdef PKG_CONFIG_EXISTS
        PKG_CONFIG_X11 := $(shell pkg-config --exists x11 2> /dev/null && echo yes || echo no)
        ifeq ($(PKG_CONFIG_X11),yes)
            CFLAGS += $(shell pkg-config --cflags x11)
            LDFLAGS = $(shell pkg-config --libs x11) -lm
        else
            # Fallback для системы без pkg-config
            CFLAGS += -I/usr/include/X11
            LDFLAGS = -lX11 -lm
        endif
    else
        # Стандартные пути для Linux
        CFLAGS += -I/usr/include/X11
        LDFLAGS = -lX11 -lm
    endif
    
    DISPLAY_CMD = @echo "Убедитесь, что X11 сервер запущен"
    CLEANUP_CMD = @echo "Программа завершена"
endif

# Настройки для других UNIX систем (FreeBSD, OpenBSD и т.д.)
ifeq ($(filter $(UNAME_S),Darwin Linux),)
    CFLAGS += -I/usr/X11R6/include
    LDFLAGS = -L/usr/X11R6/lib -lX11 -lm
    DISPLAY_CMD = @echo "Убедитесь, что X11 сервер запущен"
    CLEANUP_CMD = @echo "Программа завершена"
endif

# Полный путь к исполняемому файлу
BUILD_PATH = $(TARGET)

.PHONY: all run clean info install-deps

all: $(BUILD_PATH)

$(BUILD_PATH): $(SRCS)
	@echo "Компиляция для $(UNAME_S) $(UNAME_M)..."
	$(CC) $(CFLAGS) $(SRCS) -o $(BUILD_PATH) $(LDFLAGS)
	@echo "Сборка завершена: $(BUILD_PATH)"

run: $(BUILD_PATH)
	@echo "Запуск программы..."
	$(DISPLAY_CMD)
	@./$(BUILD_PATH)
	$(CLEANUP_CMD)

clean:
	@echo "Очистка..."
	rm -f $(BUILD_PATH)
	@echo "Очистка завершена"

info:
	@echo "Система: $(UNAME_S)"
	@echo "Архитектура: $(UNAME_M)"
	@echo "Компилятор: $(CC)"
	@echo "Флаги компилятора: $(CFLAGS)"
	@echo "Флаги линковщика: $(LDFLAGS)"
	@echo "Исходники: $(SRCS)"

# Установка зависимостей
install-deps:
ifeq ($(UNAME_S),Darwin)
	@echo "Установка зависимостей для macOS..."
	@if ! command -v brew >/dev/null 2>&1; then \
		echo "Homebrew не найден. Установите Homebrew сначала:"; \
		echo "https://brew.sh"; \
		exit 1; \
	fi
	@brew install --cask xquartz || echo "XQuartz уже установлен"
	@brew install pkg-config || echo "pkg-config уже установлен"
endif

ifeq ($(UNAME_S),Linux)
	@echo "Установка зависимостей для Linux..."
	@if command -v apt-get >/dev/null 2>&1; then \
		sudo apt-get update && sudo apt-get install -y libx11-dev pkg-config build-essential; \
	elif command -v yum >/dev/null 2>&1; then \
		sudo yum install -y libX11-devel pkgconfig gcc; \
	elif command -v pacman >/dev/null 2>&1; then \
		sudo pacman -S libx11 pkgconf base-devel; \
	elif command -v zypper >/dev/null 2>&1; then \
		sudo zypper install libX11-devel pkg-config gcc; \
	else \
		echo "Неизвестный пакетный менеджер. Установите вручную:"; \
		echo "- libx11-dev (или libX11-devel)"; \
		echo "- pkg-config"; \
		echo "- build-essential (или gcc)"; \
	fi
endif

help:
	@echo "Доступные команды:"
	@echo "  make all        - Сборка проекта"
	@echo "  make run        - Сборка и запуск"
	@echo "  make clean      - Очистка файлов сборки"
	@echo "  make info       - Показать информацию о системе"
	@echo "  make install-deps - Установить зависимости"
	@echo "  make help       - Показать эту справку"