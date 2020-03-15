#include <nds.h>
#include "gl2d.h"
#include <maxmod9.h>

#include "graphics/graphics.h"
#include "graphics/fontHandler.h"

#include "bg_title.h"
#include "logo_title.h"
#include "logo_title_chn.h"
#include "buttn_battle.h"
#include "buttn_create.h"

extern int language;

extern bool wideScreen;
extern bool fadeType;
extern int screenMode;
extern void playPromiseMusic();

static bool inited = false;

static float bgYpos = 0.0;

static touchPosition touch;

static int titleTexID;

extern glImage tileImage[(128 / 16) * (128 / 16)];
static glImage titleImage[(256 / 32) * (192 / 32)];

void menuGraphicLoad(void) {
	if (language == 6) {
	titleTexID = glLoadTileSet(titleImage, // pointer to glImage array
							32, // sprite width
							32, // sprite height
							256, // bitmap width
							192, // bitmap height
							GL_RGB16, // texture type for glTexImage2D() in videoGL.h
							TEXTURE_SIZE_256, // sizeX for glTexImage2D() in videoGL.h
							TEXTURE_SIZE_256, // sizeY for glTexImage2D() in videoGL.h
							TEXGEN_OFF | GL_TEXTURE_COLOR0_TRANSPARENT, // param for glTexImage2D() in videoGL.h
							4, // Length of the palette to use (16 colors)
							(u16*) logo_title_chnPal, // Load our 16 color tiles palette
							(u8*) logo_title_chnBitmap // image data generated by GRIT
							);
	} else {
	titleTexID = glLoadTileSet(titleImage, // pointer to glImage array
							32, // sprite width
							32, // sprite height
							256, // bitmap width
							192, // bitmap height
							GL_RGB16, // texture type for glTexImage2D() in videoGL.h
							TEXTURE_SIZE_256, // sizeX for glTexImage2D() in videoGL.h
							TEXTURE_SIZE_256, // sizeY for glTexImage2D() in videoGL.h
							TEXGEN_OFF | GL_TEXTURE_COLOR0_TRANSPARENT, // param for glTexImage2D() in videoGL.h
							4, // Length of the palette to use (16 colors)
							(u16*) logo_titlePal, // Load our 16 color tiles palette
							(u8*) logo_titleBitmap // image data generated by GRIT
							);
	}
}

void menuGraphicUnload(void) {
	glDeleteTextures(1, &titleTexID);
}

void menuScreen(void) {
	if (!inited) {
		menuGraphicLoad();
		decompress(bg_titleBitmap, bgGetGfxPtr(bg3), LZ77Vram);
		bgSetScroll(bg3, 0, 8);

		gfxSub = oamAllocateGfx(&oamSub, SpriteSize_64x64, SpriteColorFormat_16Color);

		memcpy(gfxSub, buttn_battleTiles, buttn_battleTilesLen);

		for (int i = 0; i < 16; i++) {
			SPRITE_PALETTE_SUB[i] = buttn_battlePal[i];
		}

		oamSet(&oamSub, 
			0, 
			32, 64, 
			0, 
			0,
			SpriteSize_64x64, 
			SpriteColorFormat_16Color, 
			gfxSub, 
			-1, 
			false, 
			false,	
			false, false, 
			false
			);

		gfxSub = oamAllocateGfx(&oamSub, SpriteSize_64x64, SpriteColorFormat_16Color);

		memcpy(gfxSub, buttn_createTiles, buttn_createTilesLen);

		for (int i = 0; i < 16; i++) {
			SPRITE_PALETTE_SUB[i+16] = buttn_createPal[i];
		}

		oamSet(&oamSub, 
			1, 
			160, 64, 
			0, 
			1,
			SpriteSize_64x64, 
			SpriteColorFormat_16Color, 
			gfxSub, 
			-1, 
			false, 
			false,	
			false, false, 
			false
			);

		fadeType = true;
		playPromiseMusic();
		inited = true;
	}

	//printLargeCentered(false, 96, "Press START to play");

	touchRead(&touch);
	scanKeys();
	int pressed = keysDown();
	
	if (pressed & KEY_TOUCH) {
		if (touch.py >= 64 && touch.py < 64+64) {
			if (touch.px >= 32 && touch.px < 32+64) {
				fadeType = false;
				while (!screenFadedOut());
				menuGraphicUnload();
				screenMode = 2;
			}
		}
	}
}

void renderMenuTop(void) {
	for (int x = 0; x < (wideScreen ? 20 : 16); x++) {
		for (int y = 0; y < 13; y++) {
			glSprite((x*16), (y*16)+bgYpos, GL_FLIP_NONE, &tileImage[1]);
		}
	}
	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 6; y++) {
			glSprite((x*32)+(wideScreen ? 22 : 0), y*32, GL_FLIP_NONE, &titleImage[(y*8)+x]);
		}
	}

	bgYpos -= 0.5;
	if (bgYpos < -8.5) bgYpos = 0;
	bgScroll(bg3, 0, bgYpos);
}
