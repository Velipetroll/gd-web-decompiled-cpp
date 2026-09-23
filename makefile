
ifeq ($(OS),Windows_NT)
    DETECTED_OS := Windows
else
    UNAME_S := $(shell uname -s 2>/dev/null)
    ifeq ($(UNAME_S),Linux)
        DETECTED_OS := Linux
    else ifeq ($(UNAME_S),Darwin)
        DETECTED_OS := macOS
    else
        DETECTED_OS := Linux
    endif
endif

SRC_DIR = src
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj

CXX = g++
CXXFLAGS = -std=c++17 -O2 -MMD -MP

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))
DEPS = $(OBJS:.o=.d)

ifeq ($(DETECTED_OS),Windows)
    TARGET = $(BUILD_DIR)/GeometryDash.exe
    WINDRES = windres
    RC_SRC = resource.rc
    RC_OBJ = $(if $(wildcard $(RC_SRC)),$(OBJ_DIR)/resource.o,)

    SDL_CFLAGS = $(shell sdl2-config --cflags 2>/dev/null || pkg-config --cflags sdl2 2>/dev/null)
    SDL_LIBS   = $(shell sdl2-config --libs 2>/dev/null || pkg-config --libs sdl2 2>/dev/null)
    ifeq ($(SDL_LIBS),)
        SDL_LIBS = -lmingw32 -lSDL2main -lSDL2
    endif
    LDFLAGS = $(SDL_LIBS) -lopengl32 -lz -lpthread -lm
else ifeq ($(DETECTED_OS),macOS)
    TARGET = $(BUILD_DIR)/GeometryDash
    SDL_CFLAGS = $(shell sdl2-config --cflags 2>/dev/null || pkg-config --cflags sdl2 2>/dev/null)
    SDL_LIBS   = $(shell sdl2-config --libs 2>/dev/null || pkg-config --libs sdl2 2>/dev/null)
    ifeq ($(SDL_LIBS),)
        SDL_LIBS = -lSDL2
    endif
    LDFLAGS = $(SDL_LIBS) -framework OpenGL -lz -lpthread
else
    # Linux
    TARGET = $(BUILD_DIR)/GeometryDash
    SDL_CFLAGS = $(shell sdl2-config --cflags 2>/dev/null || pkg-config --cflags sdl2 2>/dev/null)
    SDL_LIBS   = $(shell sdl2-config --libs 2>/dev/null || pkg-config --libs sdl2 2>/dev/null)
    ifeq ($(SDL_LIBS),)
        SDL_LIBS = -lSDL2
    endif
    LDFLAGS = $(SDL_LIBS) -lGL -lz -lpthread -ldl -lm
endif

CXXFLAGS += $(SDL_CFLAGS)

.PHONY: all clean copy_assets print_os

all: print_os $(TARGET) copy_assets

$(TARGET): $(OBJS) $(RC_OBJ) | $(BUILD_DIR)
	$(CXX) $(OBJS) $(RC_OBJ) -o $(TARGET) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/resource.o: $(RC_SRC) | $(OBJ_DIR)
	$(WINDRES) $< -O coff -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

copy_assets: | $(BUILD_DIR)
	@if [ -d "assets" ]; then \
		mkdir -p $(BUILD_DIR)/assets; \
		cp -rf assets/. $(BUILD_DIR)/assets/; \
	fi

-include $(DEPS)

clean:
	rm -rf $(BUILD_DIR)
