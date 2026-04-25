# Dungeon Cleanup Crew — CS3113-style build
SRCS = main.cpp \
	src/AppCommon.cpp \
	src/GameAssets.cpp \
	src/Map.cpp \
	src/Entity.cpp \
	src/Scene.cpp \
	src/DungeonLevel.cpp \
	src/ShaderProgram.cpp \
	src/levels/Level1.cpp \
	src/levels/Level2.cpp \
	src/levels/Level3.cpp

TARGET = dungeon_cleanup_crew

UNAME_S := $(shell uname -s)

CXX = g++
CXXFLAGS = -std=c++11 -I. -Isrc -Isrc/levels

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
