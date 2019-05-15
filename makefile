
                            # ################## #
                            # ------------------ #
                            #   MAKEFILE BASE    #
                            # ------------------ #
                            #   By @cambalamas   #
                            # ------------------ #
                            # ################## #

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

# Un/comment this to allow/avoid verbosity output
$(VERBOSE).SILENT:

# --- PROJECT NAME ---------------------------------------------------------- #

PROJECT_NAME = Brave

# --- MAKE SETTINGS --------------------------------------------------------- #

SHELL = powershell.exe

# --- CPP SETTINGS ---------------------------------------------------------- #

CXX            = g++
CXX_VERSION    = 17
CXX_FLAGS      = -std='c++$(CXX_VERSION)'

DEBUG_FLAGS    = -DDEBUG -g -Wall -O0
RELEASE_FLAGS  = -DNDEBUG -O3 -static -static-libgcc -static-libstdc++

# --- PATHS ----------------------------------------------------------------- #

ICON_DIR      = $(SOURCE_DIR)/icon

BUILD_DIR     = .bin
DIST_DIR      = .
SOURCE_DIR    = src
INCL_DIR      = $(SOURCE_DIR)/submodules
LIB_DIR       = $(SOURCE_DIR)/submodules

INCLUDES      = $(patsubst %,-I%,$(INCL_DIR))
LIBS          = $(patsubst %,-L%,$(LIB_DIR))
LIBS += -lglad
LIBS += -lglfw3
LIBS += -lopengl32 -lgdi32

SOURCES       = $(wildcard $(SOURCE_DIR)/*.cpp)
OBJECTS       = $(patsubst $(SOURCE_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SOURCES))

# --- TARGETS --------------------------------------------------------------- #

default_target: noflags

# CLEAN
clean:
	@rm -rf $(DIST_DIR)/$(PROJECT_NAME).exe
	@rm -rf $(BUILD_DIR)/*

# DEBUG
noflags: CXX_FLAGS += -DNDEBUG -O0
noflags: $(PROJECT_NAME)

# DEBUG
debug: CXX_FLAGS += $(DEBUG_FLAGS)
debug: $(PROJECT_NAME)

# RELEASE
release: CXX_FLAGS += $(RELEASE_FLAGS)
release: $(PROJECT_NAME)

# #! STATIC LIB
# $(PROJECT_NAME): $(OBJECTS)
# 	@ar crs lib$@.a $^
# 	@mkdir -p $(DIST_DIR)/$@
# 	@mv ./*.a ./$(DIST_DIR)
# 	@cp -r ./$(INCL_DIR)/* ./$(DIST_DIR)/$@

#! EXECUTABLE
$(PROJECT_NAME): $(OBJECTS) $(BUILD_DIR)/icon.o
	$(CXX) $(CXXFLAGS) $^ -o $@ $(INCLUDES) $(LIBS)

#* ICON COMPILATION
$(BUILD_DIR)/icon.o: $(ICON_DIR)/icon.rc
	windres $^ $@

# SOURCES COMPILATION : Called on $(OBJECTS)
$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXX_FLAGS) -c $^ -o $@ $(INCLUDES) $(LIBS)

.PHONY: clean debug release
