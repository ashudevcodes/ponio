.PHONY: all clean install uninstall run


SRC_DIR = src
BUILD_DIR= build
HEADERS_DIR = include
RAYLIB_DIR ?= lib/raylib/src

CC = gcc
CFLAGS = -Wall -Wextra -O2 -pthread -I$(RAYLIB_DIR)
LIBS = -L$(RAYLIB_DIR) -l:libraylib.a -lm -lpthread -ldl -lrt -lX11 -levdev -I/usr/include/libevdev-1.0

TARGET = ponio

UI_SRC = $(SRC_DIR)/gamepad_ui.c
CORE_SRC = $(SRC_DIR)/gamepad_core.c
HEADERS = $(HEADERS_DIR)/gamepad_types.h $(HEADERS_DIR)/gamepad_core.h

all: $(TARGET)

$(TARGET): $(CORE_SRC) $(UI_SRC) $(HEADERS)
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$(TARGET) $(CORE_SRC) $(UI_SRC) $(LIBS)

clean:
	rm -r $(BUILD_DIR)

install:
	cp $(BUILD_DIR)/$(TARGET) ~/.local/bin/

uninstall:
	rm -f ~/.local/bin/$(TARGET)

run: $(BUILD_DIR) $(TARGET)
	./$(BUILD_DIR)/$(TARGET)

