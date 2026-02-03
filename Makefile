CC = gcc
CFLAGS = -W -Wall -pedantic -ansi
LDFLAGS =

SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

TARGET = $(BIN_DIR)/2xedu_terminal_rpg

# Regra principal
all: $(TARGET)

# Link dos objetos para criar o executável
$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Compilação de cada arquivo .c em .o
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Criar diretórios se não existirem
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Limpeza
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Executar o programa
run: $(TARGET)
	./$(TARGET)

# Compilar e executar
build-and-run: clean all run

# Depurar com gdb
debug: $(TARGET)
	gdb ./$(TARGET)

.PHONY: all clean run debug help