all : fract

fract : fract.c GL_utilities.c 
	gcc -Wall -std=c99 -o fract -lm -lGL -lglut -DGL_GLEXT_PROTOTYPES fract.c VectorUtils2.c GL_utilities.c LoadTGA.c loadobj.c

clean :
	rm fract
