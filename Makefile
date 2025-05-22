# Имя исполняемого файла
TARGET = CG_Project

# Путь к исходникам
SRC_DIR = src
SRCS = $(SRC_DIR)/main.c $(SRC_DIR)/transforms.c $(SRC_DIR)/input.c $(SRC_DIR)/rendering.c

# Флаги компилятора
CFLAGS = $(shell pkg-config --cflags x11)
LDFLAGS = $(shell pkg-config --libs x11)

# Полный путь к исполняемому файлу
BUILD_PATH = $(TARGET)

.PHONY: all run clean

all: $(BUILD_PATH)

$(BUILD_PATH): $(SRCS)
	$(CC) $(SRCS) -o $(BUILD_PATH) $(CFLAGS) $(LDFLAGS)

run: $(BUILD_PATH)
	@echo "Launching XQuartz..."
	@export DISPLAY=:0 && open -a XQuartz
	@sleep 1
	@$(BUILD_PATH)
	@pkill -f Xquartz

clean:
	rm -f $(BUILD_PATH)