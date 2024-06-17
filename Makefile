SOURCES = *.cpp
OUTPUT = bin/main
INCLUDES = -lncursesw

build:
	g++ ${SOURCES} ${INCLUDES} -o ${OUTPUT}
