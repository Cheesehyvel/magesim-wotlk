#OBJS specifies which files to compile as part of the project
OBJS = src/buff.cpp src/common.cpp src/main.cpp src/mirror_image.cpp src/player.cpp src/simulation.cpp src/state.cpp src/target.cpp src/unit.cpp src/water_elemental.cpp

#CC specifies which compiler we're using
CC = em++

#COMPILER_FLAGS specifies the additional compilation options we're using
# -w suppresses all warnings
COMPILER_FLAGS = --bind -sMODULARIZE=1 -sNO_FILESYSTEM=1 -sASSERTIONS=1 -sEXPORT_NAME="MageSim" -O3

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS =

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = dist/js/magesim.js

#This is the target that compiles our executable
all : $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)