CC=g++
CFLAGS=-g -Wall \
	-Wfloat-equal \
	-Wundef \
	-Wshadow \
	-Wpointer-arith \
	-Wcast-qual \
	-Wcast-align \
	-Wunreachable-code

SOURCES=main.cpp

SRC_DIR=src
BUILD_DIR=build
BIN_DIR=bin

OBJECTS=$(SOURCES:.cpp=.o)
BUILD_FILES=$(addprefix $(BUILD_DIR)/, $(OBJECTS))

TARGET=Metaballs
$(TARGET): $(BUILD_FILES)
	@echo "Compiling target: $(TARGET)"
	@$(CC) $(BUILD_FILES) -o $@
$(BUILD_DIR)/%.o:$(SRC_DIR)/%.cpp
	@echo "Compiling file: $<" 
	@$(CC) $(CFLAGS) -c $(subst $(BUILD_DIR),$(SRC_DIR),$(@:.o=.cpp)) -o $@
clean:
	@find . -type f \( -name '*~' \) -delete
	@cd $(BUILD_DIR)/; rm -fr *
	@cd $(BIN_DIR)/; rm -f *
