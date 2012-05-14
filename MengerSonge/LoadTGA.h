#ifndef __TGA_LOADER__
#define __TGA_LOADER__

#ifdef __APPLE__
	#include <OpenGL/gl3.h>
#else
	#include <GL/gl.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif

#ifndef bool
#define bool char
#endif

typedef struct TextureData		// Create A Structure for .tga loading.
{
	GLubyte	*imageData;			// Image Data (Up To 32 Bits)
	GLuint	bpp;				// Image Color Depth In Bits Per Pixel.
	GLuint	width;				// Image Width
	GLuint	height;				// Image Height
	GLuint	texID;				// Texture ID Used To Select A Texture
	GLfloat	texWidth, texHeight;
} TextureData, *TextureDataPtr;					// Structure Name

bool LoadTGATexture(char *filename, TextureData *texture);
void LoadTGATextureSimple(char *filename, GLuint *tex);

#endif

