# Detect Homebrew installation
BREW_PREFIX := $(shell brew --prefix 2>/dev/null || echo "/usr/local")
RAYLIB_PATH := $(BREW_PREFIX)
JSON_PATH := $(BREW_PREFIX)

# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
INCLUDES = -I$(RAYLIB_PATH)/include -I$(JSON_PATH)/include -Isrc/ -Iinclude/
LIBS = -L$(RAYLIB_PATH)/lib -lraylib

# Platform-specific settings
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    LIBS += -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
endif

# Source files
SRCDIR = src
OBJDIR = build
SOURCES := $(shell find $(SRCDIR) -name '*.cpp')
OBJECTS := $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

# Main target
TARGET = rogue-like

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(TARGET)

.PHONY: all clean 