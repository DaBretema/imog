
                            # ################## #
                            # ------------------ #
                            #   MAKEFILE BASE    #
                            # ------------------ #
                            #   By @cambalamas   #
                            # ------------------ #
                            # ################## #


# $(VERBOSE).SILENT: # Uncomment this to avoid verbosity output


# ########################################################################### #
#                             - CONFIGURATION -

# 1.- Setup PROJECT_NAME

# 2.- Setup that directories:
#      INCL_DIRS ,LIB_DIRS ,SOURCE_DIR ,BUILD_DIR and if
#      your app have icon, also ICON_DIR (Also uncomment *ICON Target*).

# 3.- Check SHELL var between "bash/zsh" for Linux or Mac
#      and "powershell.exe" for Windows.

# 4.- Check the c++ version that want to be used on CXX_VERSION var.

# 5.- Add include directories to INCL_DIRS and libs directories to LIB_DIRS.

# 6.- Add used libraries to LIBS as LIBS += -lib..., do it on new lines
#      for better readability.

# 7.- To end, just uncommet one of two (!) Targets, one for static lib
#      compilation and the other to get a executable file.

# ########################################################################### #


# --- PROJECT NAME ---------------------------------------------------------- #

PROJECT_NAME = Dac

# --- MAKE SETTINGS --------------------------------------------------------- #

SHELL = powershell.exe

# --- CPP SETTINGS ---------------------------------------------------------- #

CXX            = g++
CXX_VERSION    = 17
CXX_FLAGS      = -std='c++$(CXX_VERSION)' -fopenmp

DEBUG_FLAGS    = -DDEBUG -g -Wall -O0
RELEASE_FLAGS  = -DNDEBUG -O3 -static -static-libgcc -static-libstdc++

# --- PATHS ----------------------------------------------------------------- #

ICON_DIR      = assets/icon
INCL_DIRS     = lib
LIB_DIRS      = lib

BIN_DIR       = bin
SOURCE_DIR    = src
BUILD_DIR     = build

INCLUDES      = $(INCL_DIRS:%=-I%)
LIBS          = $(LIB_DIRS:%=-L%)
LIBS += -lopengl32 -lgdi32
LIBS += -lglfw3
LIBS += -lDac

SOURCES       = $(wildcard $(SOURCE_DIR)/*.c*)
OBJECTS       = $(SOURCES:%=$(BUILD_DIR)/%.o)

# --- TARGETS --------------------------------------------------------------- #

.PHONY: clean
default_target: debug

# CLEAN
clean:
	-rm *.a
	-rm *.exe
	-rm $(BUILD_DIR)/*.o

# DEBUG
debug: CXX_FLAGS += $(DEBUG_FLAGS)
debug: $(PROJECT_NAME)

# RELEASE
release: CXX_FLAGS += $(RELEASE_FLAGS)
release: $(PROJECT_NAME)

# #! STATIC LIB
# $(PROJECT_NAME): $(OBJECTS)
# 	ar crvs lib$(PROJECT_NAME).a $^
	-mkdir -p $(dir $(BIN_DIR))
# 	mv ./*.a ./$(BIN_DIR)
# 	cp ./*.h* ./$(BIN_DIR)/$(PROJECT_NAME)

# #! EXECUTABLE
# $(PROJECT_NAME): $(OBJECTS)
# 	$(CXX) $(CXXFLAGS) $^ -o $@ $(INCLUDES) $(LIBS)


# #* ICON COMPILATION
# $(BUILD_DIR)/icon.o: $(ICON_DIR)/icon.rc
# 	windres $^ $@

# SOURCES COMPILATION : Called on $(OBJECTS)
$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.cpp $(SOURCE_DIR)/%.cc $(SOURCE_DIR)/%.c
	-mkdir -p $(dir $@)
	$(CXX) $(CXX_FLAGS) -c $^ -o $@ $(INCLUDES) $(LIBS)
