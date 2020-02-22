#include <nds.h>
#include "gl2d.h"
#include <maxmod9.h>

#include "graphics/fontHandler.h"
#include "graphics/graphics.h"

#include "tiles.h"

static int floorXpos = 0;

static int tileTexID;
static glImage tileImage[(128 / 16) * (128 / 16)];

//extern mm_sound_effect snd_jump;

void levelGraphicLoad(void) {
	tileTexID = glLoadTileSet(tileImage, // pointer to glImage array
							16, // sprite width
							16, // sprite height
							64, // bitmap width
							128, // bitmap height
							GL_RGB16, // texture type for glTexImage2D() in videoGL.h
							TEXTURE_SIZE_64, // sizeX for glTexImage2D() in videoGL.h
							TEXTURE_SIZE_128, // sizeY for glTexImage2D() in videoGL.h
							GL_TEXTURE_WRAP_S | GL_TEXTURE_WRAP_T | TEXGEN_OFF, // param for glTexImage2D() in videoGL.h
							16, // Length of the palette to use (16 colors)
							(u16*) tilesPal, // Load our 16 color tiles palette
							(u8*) tilesBitmap // image data generated by GRIT
							);
}

void levelMode(void) {
	scanKeys();
	int pressed = keysDown();
}

void levelGraphicDisplay(void) {
	glBoxFilled(0, 0, 256, 192, RGB15(255/8, 255/8, 255/8));
	for (int x = 0; x < 256; x += 16) {
		for (int y = 0; y < 192; y += 16) {
			glSprite(x, y, GL_FLIP_NONE, &tileImage[7]);
		}
	}
	for (int floorRenderCount = 0; floorRenderCount < 256; floorRenderCount += 16) {
		glSprite(floorRenderCount+floorXpos, 176, GL_FLIP_NONE, &tileImage[9]);
	}
}
