CC?=		gcc
CXX?=		g++
CPP?=		cpp

CFLAGS=		-Wall -Wextra -std=c99 -D_THREAD_SAFE -DGL_GLEXT_PROTOTYPES \
		-I/usr/local/include -I/usr/local/include/drm
CXXFLAGS=	
OPTFLAGS=	-O3 -pipe -fomit-frame-pointer -funroll-all-loops
DEBUGFLAGS=	-O0 -g
LDFLAGS=	-lm -L/usr/local/lib -lglut -lGL -lGLU

OBJS=	fract.o \
		GL_utilities.o \
		LoadTGA.o \
		VectorUtils2.o \
		loadobj.o

.PHONY: all
all: fractal Makefile

fractal: ${OBJS}
	${CXX} -o $@ ${LDFLAGS} ${OBJS}

%.o: %.c
	${CC} ${CFLAGS} ${OPTFLAGS} -c $<

%.o: %.cpp
	${CXX} ${CXXFLAGS} ${CFLAGS} ${OPTFLAGS} -c $<

.PHONY:	install
install: all

.PHONY: clean
clean :
	\rm -f fractal *.o
