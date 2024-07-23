# Compilador e flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Iinclude

# Diretórios
SRC_DIR = src
OBJ_DIR = obj

# Arquivos fonte e objeto
COMMON_SRC = $(filter-out $(SRC_DIR)/ligador.cpp $(SRC_DIR)/montador.cpp, $(wildcard $(SRC_DIR)/*.cpp))
LIGADOR_SRC = $(SRC_DIR)/ligador.cpp $(COMMON_SRC)
MONTADOR_SRC = $(SRC_DIR)/montador.cpp $(COMMON_SRC)

LIGADOR_OBJ = $(LIGADOR_SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
MONTADOR_OBJ = $(MONTADOR_SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Executáveis
LIGADOR_EXEC = ligador
MONTADOR_EXEC = montador

# Regras
all: $(LIGADOR_EXEC) $(MONTADOR_EXEC)

# Regra para compilar os arquivos de código fonte
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Linkar os objetos para criar o executável ligador
$(LIGADOR_EXEC): $(LIGADOR_OBJ)
	$(CXX) $(LIGADOR_OBJ) -o $@

# Linkar os objetos para criar o executável montador
$(MONTADOR_EXEC): $(MONTADOR_OBJ)
	$(CXX) $(MONTADOR_OBJ) -o $@

# Limpeza dos arquivos objeto e executáveis
clean:
	rm -rf $(OBJ_DIR)/*.o $(LIGADOR_EXEC) $(MONTADOR_EXEC)

# Rodar teste (caso necessário)
test: $(TEST_EXEC)
	./$(TEST_EXEC)
