.PHONY: all clean install uninstall run gdb

SRC_DIR = src
BUILD_DIR= build
HEADERS_DIR = include

DIST_DIR = release
PKG_NAME = ponio
PKG_DIR = $(DIST_DIR)/$(PKG_NAME)

CC = gcc
CFLAGS = -Wall -Wextra -O2 -pthread $(shell pkg-config --cflags libevdev)
LIBS = -lpthread $(shell pkg-config --libs libevdev)

TARGET = ponio
GDBTARGET = ponioGdb

UI_SRC = $(SRC_DIR)/gamepad_ui.c
CORE_SRC = $(SRC_DIR)/gamepad_core.c $(SRC_DIR)/server_discovery.c
HEADERS = $(HEADERS_DIR)/gamepad_types.h $(HEADERS_DIR)/gamepad_core.h $(HEADERS_DIR)/server_discovery.h

all: $(TARGET)

$(TARGET): $(CORE_SRC) $(UI_SRC) $(HEADERS)
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$(TARGET) $(CORE_SRC) $(UI_SRC) $(LIBS)

gdb:$(CORE_SRC) $(UI_SRC) $(HEADERS)
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -ggdb -o $(BUILD_DIR)/$(GDBTARGET) $(CORE_SRC) $(UI_SRC) $(LIBS)

release: $(TARGET)
	@echo "Building release $(PKG_NAME)..."
	mkdir -p $(PKG_DIR)
	cp build/$(TARGET) $(PKG_DIR)/
	cp install-uinput.sh $(PKG_DIR)/
	chmod +x $(PKG_DIR)/install-uinput.sh
	cp README.md $(PKG_DIR)/ 2>/dev/null || true
	cd $(DIST_DIR) && tar -czvf $(PKG_NAME).tar.gz $(PKG_NAME)
	@echo "Done: $(DIST_DIR)/$(PKG_NAME).tar.gz"


install:
	cp $(BUILD_DIR)/$(TARGET) ~/.local/bin/

clean:
	rm -r $(BUILD_DIR)
	rm -r release


uninstall:
	rm -f ~/.local/bin/$(TARGET)

run: $(TARGET)
	./$(BUILD_DIR)/$(TARGET)
