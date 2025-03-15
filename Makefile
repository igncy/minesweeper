# $@ target
# $< first prerequisite
# $^ all prerequisites
# ${word n,$^} n-th prerequisite

CXX=g++
CXX_FLAGS=-Wall -std=c++17
BUILD_DIR=build
SRC_DIR=src
INCLUDES=-Iinclude -Icmake-build-release/_deps/argparse-src/include
LIBS=-lncurses
MACROS=-DNO_COLORS
TGT=minesweeper

.PHONY: all run clean
all: ${TGT}

debug: CXX_FLAGS+=-O0 -g
debug: all

${BUILD_DIR}:
	mkdir -p ${BUILD_DIR}

SRC=${wildcard ${SRC_DIR}/*.cpp}
OBJ=${patsubst ${SRC_DIR}/%.cpp, ${BUILD_DIR}/%.o, ${SRC}}

${BUILD_DIR}/%.o: ${SRC_DIR}/%.cpp | ${BUILD_DIR}
	${CXX} ${CXX_FLAGS} ${INCLUDES} $< -o $@ -c

${TGT}: ${OBJ} | ${BUILD_DIR}
	${CXX} ${CXX_FLAGS} $^ -o ${BUILD_DIR}/$@ ${LIBS} ${MACROS}

run:
	./${BUILD_DIR}/${TGT} ${args}

clean:
	rm -rf ${BUILD_DIR}
