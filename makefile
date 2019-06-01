$(VERBOSE).SILENT:

# os-pecific
ifeq ($(OS),Windows_NT)
SHELL := powershell.exe
else
UNAME := $(shell uname -s)
endif

# settings
PROJECT_NAME := Brave

# cpp
CXX       := g++
CXX_FLAGS := -std=c++17 -Wno-unused-command-line-argument

# opt-flags
DEBUG_FLAGS   := -DDEBUG -ggdb3 -Wall -O0
RELEASE_FLAGS := -DNDEBUG -O3 -static -static-libgcc -static-libstdc++

# paths
DIST_DIR    := dist
SOURCE_DIR  := src
BUILD_DIR   := .bin
INCL_DIR    := $(SOURCE_DIR)/submodules
INCLUDES    := $(patsubst %,-I%,$(INCL_DIR))
SOURCES     := $(wildcard $(SOURCE_DIR)/*.cpp)
OBJECTS     := $(patsubst $(SOURCE_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SOURCES))


# libs
LIBS := $(patsubst %,-L%,$(SOURCE_DIR)/submodules)

ifeq ($(OS),Windows_NT)
LIBS += -lglfw3_win
LIBS += -lopengl32 -lgdi32
else
ifeq ($(UNAME),Darwin)
CXX       := clang++
LIBS      += -L/usr/local/lib
INCLUDES  += -I/usr/local/include
LIBS      += -lglfw3_mac -framework Cocoa -framework OpenGL -framework IOKit -framework CoreFoundation -framework CoreVideo
endif
endif


# targets

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	 @mkdir -p $(dir $@)
	 $(CXX) $(CXX_FLAGS) -c $^ -o $@ $(INCLUDES) $(LIBS)

default_target: releaseExe

releaseExe releaseLib: CXX_FLAGS += $(RELEASE_FLAGS)
debugExe debugLib: CXX_FLAGS += $(DEBUG_FLAGS)

debugExe releaseExe: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $(PROJECT_NAME) $(INCLUDES) $(LIBS)

debugLib releaseLib: $(OBJECTS)
	@ar crs lib$(PROJECT_NAME).a $^
	@mkdir -p $(DIST_DIR)/$(PROJECT_NAME)
	@mv ./*.a ./$(DIST_DIR)
	@cp -r ./$(INCL_DIR)/* ./$(DIST_DIR)/$(PROJECT_NAME)

clean:
ifeq ($(OS),Windows_NT)
	@rm -rf $(DIST_DIR)/$(PROJECT_NAME).exe > $(BUILD_DIR)/ignoreErrs 2>&1
else
	@rm -rf $(DIST_DIR)/$(PROJECT_NAME) > $(BUILD_DIR)/ignoreErrs 2>&1
endif
	@rm -rf $(BUILD_DIR)/* > $(BUILD_DIR)/ignoreErrs 2>&1
