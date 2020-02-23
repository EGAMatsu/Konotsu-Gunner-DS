#include <nds.h>
#include "gl2d.h"
#include <maxmod9.h>

#include "graphics/fontHandler.h"
#include "graphics/graphics.h"

#include "spr_player.h"

extern u8* mapLocation;

extern int mapHsize;
extern int mapVsize;

extern int cameraXpos;
extern int cameraYpos;

static bool playerJump = false;
static bool allowPlayerJump = false;
static bool playerDirection = true;
static bool animateLegs = false;
static bool yMoveDelay = false;
static int playerTexID;
int playerX, playerY, playerStartX, playerStartY;
static int playerYmoveSpeed=1, legAniFrame, legAniDelay;
static glImage playerImage[(256 / 16) * (16 / 16)];

void setPlayerPosition(int x, int y) {
	playerStartX = x;
	playerStartY = y;
	playerX = playerStartX;
	playerY = playerStartY;
}

void resetPlayerPosition(void) {
	playerX = playerStartX;
	playerY = playerStartY;
}

void playerGraphicLoad(void) {
	playerTexID = glLoadTileSet(playerImage, // pointer to glImage array
							16, // sprite width
							16, // sprite height
							256, // bitmap width
							16, // bitmap height
							GL_RGB16, // texture type for glTexImage2D() in videoGL.h
							TEXTURE_SIZE_256, // sizeX for glTexImage2D() in videoGL.h
							TEXTURE_SIZE_16, // sizeY for glTexImage2D() in videoGL.h
							TEXGEN_OFF | GL_TEXTURE_COLOR0_TRANSPARENT, // param for glTexImage2D() in videoGL.h
							3, // Length of the palette to use (3 colors)
							(u16*) spr_playerPal, // Load our 16 color tiles palette
							(u8*) spr_playerBitmap // image data generated by GRIT
							);
}

void playerLoop(int pressed, int held) {
	if (playerJump) {
		allowPlayerJump = false;
		if (mapLocation[((playerY/16)*mapHsize)+((playerDirection ? playerX+8 : playerX)/16)] == 17) {
			playerY++;
			playerJump = false;
			playerYmoveSpeed = 1;
		}
		if (mapLocation[((playerY/16)*mapHsize)+((playerDirection ? playerX+8 : playerX)/16)] == 7) {
			// Make the player jump
			yMoveDelay = !yMoveDelay;
			if (yMoveDelay) {
				playerY -= playerYmoveSpeed;
				playerYmoveSpeed--;
			}
			if (playerYmoveSpeed < 1) {
				playerJump = false;
				playerYmoveSpeed = 1;
			}
		}
	} else {
		if (mapLocation[(((playerY+31)/16)*mapHsize)+((playerDirection ? playerX+8 : playerX)/16)] == 7) {
			// Make the player fall
			yMoveDelay = !yMoveDelay;
			if (yMoveDelay) {
				playerY += playerYmoveSpeed;
				playerYmoveSpeed++;
			}
			if (playerYmoveSpeed > 16) playerYmoveSpeed = 16;
		}
		if (mapLocation[(((playerY+31)/16)*mapHsize)+((playerDirection ? playerX+8 : playerX)/16)] == 17) {
			playerY -= (playerY % 16);
			playerYmoveSpeed = 1;
			allowPlayerJump = true;
		}
		if (mapLocation[(((playerY+31)/16)*mapHsize)+((playerDirection ? playerX+8 : playerX)/16)] == 22) {
			// Player is killed
			resetPlayerPosition();
			playerYmoveSpeed = 1;
			allowPlayerJump = false;
			playerDirection = true;
		}
	}

	if (held & KEY_LEFT) {
		playerDirection = false;
		playerX -= 1;
	} else if (held & KEY_RIGHT) {
		playerDirection = true;
		playerX += 1;
	}
	
	if (((pressed & KEY_UP) || (pressed & KEY_B)) && allowPlayerJump) {
		playerJump = true;
		playerYmoveSpeed = 8;
	}
	
	if (playerDirection) {
		if (mapLocation[(((playerY+24)/16)*mapHsize)+((playerX+8)/16)] == 17) {
			playerX -= 1;
		}
	} else {
		if (mapLocation[(((playerY+24)/16)*mapHsize)+(playerX/16)] == 17) {
			playerX += 1;
		}
	}

	animateLegs = ((held & KEY_LEFT) || (held & KEY_RIGHT));
}

void renderPlayer(void) {
	glBoxFilled((playerX+2)+cameraXpos, (playerY+6)+cameraYpos, (playerX+8)+cameraXpos, (playerY+8)+cameraYpos, RGB15(0, 31, 0));		// Eye color
	glColor(RGB15(219/8, 172/8, 129/8));											// Head color
	glSprite((playerDirection ? playerX+1 : playerX-7)+cameraXpos, (playerY+5)+cameraYpos, playerDirection ? GL_FLIP_NONE : GL_FLIP_H, &playerImage[8]);					// Back arm
	glSprite((playerX-3)+cameraXpos, (playerY+2)+cameraYpos, playerDirection ? GL_FLIP_NONE : GL_FLIP_H, &playerImage[0]);						// Head
	glColor(RGB15(31, 31, 31));
	glSprite((playerX-3)+cameraXpos, (playerY+11)+cameraYpos, playerDirection ? GL_FLIP_NONE : GL_FLIP_H, &playerImage[1]);					// Torso
	glSprite((playerX-3)+cameraXpos, (playerY+19)+cameraYpos, playerDirection ? GL_FLIP_NONE : GL_FLIP_H, &playerImage[2+legAniFrame]);	// Legs
	glSprite((playerX-3)+cameraXpos, (playerY+19)+cameraYpos, playerDirection ? GL_FLIP_NONE : GL_FLIP_H, &playerImage[5+legAniFrame]);	// Feet
	glColor(RGB15(219/8, 172/8, 129/8));											// Arm color
	glSprite((playerDirection ? playerX-4 : playerX-2)+cameraXpos, (playerY+5)+cameraYpos, playerDirection ? GL_FLIP_NONE : GL_FLIP_H, &playerImage[8]);					// Front arm
	glColor(RGB15(31, 31, 31));

	if (animateLegs) {
		legAniDelay++;
		if (legAniDelay==3) {
			legAniFrame++;
			if (legAniFrame > 2) {
				legAniFrame = 0;
			}
			legAniDelay = 0;
		}
	} else {
		legAniFrame = 0;
		legAniDelay = 2;
	}
}
