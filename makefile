
                            # ################## #
                            # ------------------ #
                            #   MAKEFILE BASE    #
                            # ------------------ #
                            #   By @cambalamas   #
                            # ------------------ #
                            # ################## #


$(VERBOSE).SILENT: # Un/comment this to avoid/allow verbosity output



# ########################################################################### #
#                             - CONFIGURATION -

# 1.- Setup PROJECT_NAME

# 2.- Setup that directories:
#      INCL_DIR ,LIB_DIR ,SOURCE_DIR ,BUILD_DIR and if
#      your app have icon, also ICON_DIR (Also uncomment *ICON Target*).

# 3.- Check SHELL var between "bash/zsh" for Linux or Mac
#      and "powershell.exe" for Windows.

# 4.- Check the c++ version that want to be used on CXX_VERSION var.

# 5.- Add include directories to INCL_DIR and libs directories to LIB_DIR.

# 6.- Add used libraries to LIBS as LIBS += -lib..., do it on new lines
#      for better readability.

# 7.- To end, just uncommet one of two (!) Targets, one for static lib
#      compilation and the other to get a executable file.

# ########################################################################### #


# --- PROJECT NAME ---------------------------------------------------------- #

PROJECT_NAME = Brave

# --- MAKE SETTINGS --------------------------------------------------------- #

SHELL = powershell.exe

# --- CPP SETTINGS ---------------------------------------------------------- #

CXX            = g++
CXX_VERSION    = 17
CXX_FLAGS      = -std='c++$(CXX_VERSION)' -fopenmp

DEBUG_FLAGS    = -DDEBUG -g -Wall -O0
RELEASE_FLAGS  = -DNDEBUG -O3 -static -static-libgcc -static-libstdc++

# --- PATHS ----------------------------------------------------------------- #

ICON_DIR      =

BUILD_DIR     = build
BIN_DIR       = bin
INCL_DIR      = incl
LIB_DIR       = lib
SOURCE_DIR    = src

INCLUDES      = $(patsubst %,-I%,$(INCL_DIR) $(LIB_DIR))
LIBS          = $(patsubst %,-L%,$(LIB_DIR))
LIBS += -lglad
LIBS += -lDac
LIBS += -lglfw3
LIBS += -lopengl32 -lgdi32

SOURCES       = $(wildcard $(SOURCE_DIR)/*.cpp)
OBJECTS       = $(patsubst $(SOURCE_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SOURCES))

# --- TARGETS --------------------------------------------------------------- #

default_target: debug

# CLEAN
clean:
	@rm -rf $(BIN_DIR)/*
	@rm -rf $(BUILD_DIR)/*

# DEBUG
debug: CXX_FLAGS += $(DEBUG_FLAGS)
debug: $(PROJECT_NAME)

# RELEASE
release: CXX_FLAGS += $(RELEASE_FLAGS)
release: $(PROJECT_NAME)

# #! STATIC LIB
# $(PROJECT_NAME): $(OBJECTS)
# 	@ar crs lib$@.a $^
# 	@mkdir -p $(BIN_DIR)/$@
# 	@mv ./*.a ./$(BIN_DIR)
# 	@cp -r ./$(INCL_DIR)/* ./$(BIN_DIR)/$@

#! EXECUTABLE
$(PROJECT_NAME): $(OBJECTS)
	@echo "Creating: $@.exe"
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(INCLUDES) $(LIBS)
	@mv ./$@.exe ./$(BIN_DIR)/$@.exe

# #* ICON COMPILATION
# $(BUILD_DIR)/icon.o: $(ICON_DIR)/icon.rc
# 	windres $^ $@

# SOURCES COMPILATION : Called on $(OBJECTS)
$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	@echo "Changes detected @ $^"
	@mkdir -p $(dir $@)
	$(CXX) $(CXX_FLAGS) -c $^ -o $@ $(INCLUDES) $(LIBS)

.PHONY: clean debug release
