# Dungeon Cleanup Crew — CS3113-style build
SRCS = main.cpp \
	CS3113/cs3113.cpp \
	CS3113/GameAssets.cpp \
	CS3113/Map.cpp \
	CS3113/Entity.cpp \
	CS3113/Scene.cpp \
	CS3113/DungeonLevel.cpp \
	CS3113/ShaderProgram.cpp \
	CS3113/Level1.cpp \
	CS3113/Level2.cpp \
	CS3113/Level3.cpp

TARGET = dungeon_cleanup_crew

UNAME_S := $(shell uname -s)

CXX = g++
CXXFLAGS = -std=c++11 -I.

RAYLIB_CFLAGS = $(shell pkg-config --cflags raylib 2>/dev/null)
RAYLIB_LIBS = $(shell pkg-config --libs raylib 2>/dev/null)

ifeq ($(UNAME_S), Darwin)
	CXXFLAGS += -arch arm64 $(RAYLIB_CFLAGS)
	LIBS = $(RAYLIB_LIBS) -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
	EXEC = ./$(TARGET)
else ifneq (,$(findstring MINGW,$(UNAME_S)))
	CXXFLAGS += -IC:/raylib/include
	LIBS = -LC:/raylib/lib -lraylib -lopengl32 -lgdi32 -lwinmm
	TARGET := $(TARGET).exe
	EXEC = ./$(TARGET)
else
	LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
	EXEC = ./$(TARGET)
endif

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS) $(LIBS)

clean:
	@if [ -f "$(TARGET)" ]; then rm -f $(TARGET); fi
	@if [ -f "$(TARGET).exe" ]; then rm -f $(TARGET).exe; fi

run: $(TARGET)
	$(EXEC)

.PHONY: clean run
