
#SOURCES=main.cpp metaball.cpp vulkan_instance.cpp
SOURCES=$(shell find src -type f -name '*.cpp' )

SRC_DIR=src
BUILD_DIR=build
BIN_DIR=bin
INCLUDE_DIR=include
VULKAN_SDK_PATH = /home/kims/VulkanSDK/1.0.65.0/x86_64
VULKAN_SDK_INCLUDE_PATH = $(VULKAN_SDK_PATH)/include

CC=g++
CFLAGS=-g -Wall \
	-Wfloat-equal \
	-Wundef \
	-Wshadow \
	-Wpointer-arith \
	-Wcast-qual \
	-Wcast-align \
	-Wunreachable-code \
	-I$(INCLUDE_DIR) \
	-I$(VULKAN_SDK_INCLUDE_PATH)
LDFLAGS = -L$(VULKAN_SDK_PATH)/lib -lvulkan -lxcb

OBJECT_FILES=$(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

TARGET=Metaballs

all: clean $(TARGET)
$(TARGET): $(OBJECT_FILES)
	@echo "Compiling target: $(TARGET)"
	@$(CC) $(OBJECT_FILES) -o $@ $(LDFLAGS)
$(BUILD_DIR)/%.o:$(SRC_DIR)/%.cpp
	@echo "Compiling file: $<"
	@$(CC) $(CFLAGS) -c $< -o $@
clean:
	@find . -type f \( -name '*~' \) -delete
	@cd $(BUILD_DIR)/; rm -fr *
	@cd $(BIN_DIR)/; rm -f *
install:
	sudo apt-get update
	sudo apt-get dist-upgrade
	sudo apt-get install libglm-dev graphviz libxcb-dri3-0 libxcb-present0 \
	libpciaccess0 cmake libpng-dev libxcb-keysyms1-dev libxcb-dri3-dev \
	libx11-dev libmirclient-dev libwayland-dev libxrandr-dev
	sudo apt-get install git libpython2.7
	sudo apt-get install libglm-dev
output:
	echo $(OBJECT_FILES)
