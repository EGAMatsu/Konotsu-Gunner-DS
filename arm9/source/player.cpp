#include <nds.h>
#include "gl2d.h"
#include <maxmod9.h>

#include "graphics/fontHandler.h"
#include "graphics/graphics.h"

#include "spr_hud.h"
#include "spr_ammocrate.h"
#include "spr_player.h"
#include "spr_pistol.h"
#include "spr_bullet.h"

extern void sndShoot();
extern void sndAmmoOut();
extern void sndHurt();

extern u8* mapLocation;

extern int mapHsize;
extern int mapVsize;

extern int cameraXpos;
extern int cameraYpos;

static int eyeColor[3] = {0, 170, 0};
static int hairColor[3] = {170, 85, 0};
static int skinColor[3] = {255, 255, 85};
static int torsoColor[3] = {255, 85, 85};
static int legColor[3] = {170, 85, 0};
static int feetColor[3] = {0, 0, 170};

static bool playerJump = false;
static bool allowPlayerJump = false;
static bool jumpFallFrame = true;
static bool playerDirection = true;
static bool moveDirection = true;
static bool animateLegs = false;
static bool moveMore = false;
static bool yMoveDelay = false;
static int hudTexID, crateTexID, playerTexID, pistolTexID, bulletTexID;
static glImage hudImage[64 * 32];
static glImage ammoCrateImage[16 * 16];
static glImage playerImage[(256 / 16) * (16 / 16)];
static glImage pistolImage[(64 / 16) * (16 / 16)];
static glImage bulletImage[8 * 8];

static touchPosition touch;
int playerX, playerY, playerStartX, playerStartY;
static int bulletX[2], bulletY[2], aimDir=2, bulletDir[2], shootDelay;
static bool currentBullet = false;
static bool bulletActive[2] = {false};
static bool bulletDirection[2] = {false};

static int health=100, ammoCount=999, playerYmoveSpeed=1, legAniFrame, legAniDelay;
static char healthText[8], ammoText[8];

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
	hudTexID = glLoadTileSet(hudImage, // pointer to glImage array
							64, // sprite width
							32, // sprite height
							64, // bitmap width
							32, // bitmap height
							GL_RGB16, // texture type for glTexImage2D() in videoGL.h
							TEXTURE_SIZE_64, // sizeX for glTexImage2D() in videoGL.h
							TEXTURE_SIZE_32, // sizeY for glTexImage2D() in videoGL.h
							TEXGEN_OFF | GL_TEXTURE_COLOR0_TRANSPARENT, // param for glTexImage2D() in videoGL.h
							3, // Length of the palette to use (3 colors)
							(u16*) spr_hudPal, // Load our 16 color tiles palette
							(u8*) spr_hudBitmap // image data generated by GRIT
							);

	crateTexID = glLoadTileSet(ammoCrateImage, // pointer to glImage array
							16, // sprite width
							16, // sprite height
							16, // bitmap width
							16, // bitmap height
							GL_RGB16, // texture type for glTexImage2D() in videoGL.h
							TEXTURE_SIZE_16, // sizeX for glTexImage2D() in videoGL.h
							TEXTURE_SIZE_16, // sizeY for glTexImage2D() in videoGL.h
							TEXGEN_OFF | GL_TEXTURE_COLOR0_TRANSPARENT, // param for glTexImage2D() in videoGL.h
							3, // Length of the palette to use (3 colors)
							(u16*) spr_ammocratePal, // Load our 16 color tiles palette
							(u8*) spr_ammocrateBitmap // image data generated by GRIT
							);

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

	pistolTexID = glLoadTileSet(pistolImage, // pointer to glImage array
							16, // sprite width
							16, // sprite height
							64, // bitmap width
							16, // bitmap height
							GL_RGB16, // texture type for glTexImage2D() in videoGL.h
							TEXTURE_SIZE_64, // sizeX for glTexImage2D() in videoGL.h
							TEXTURE_SIZE_16, // sizeY for glTexImage2D() in videoGL.h
							TEXGEN_OFF | GL_TEXTURE_COLOR0_TRANSPARENT, // param for glTexImage2D() in videoGL.h
							4, // Length of the palette to use (3 colors)
							(u16*) spr_pistolPal, // Load our 16 color tiles palette
							(u8*) spr_pistolBitmap // image data generated by GRIT
							);

	bulletTexID = glLoadTileSet(bulletImage, // pointer to glImage array
							8, // sprite width
							8, // sprite height
							8, // bitmap width
							8, // bitmap height
							GL_RGB16, // texture type for glTexImage2D() in videoGL.h
							TEXTURE_SIZE_8, // sizeX for glTexImage2D() in videoGL.h
							TEXTURE_SIZE_8, // sizeY for glTexImage2D() in videoGL.h
							TEXGEN_OFF | GL_TEXTURE_COLOR0_TRANSPARENT, // param for glTexImage2D() in videoGL.h
							4, // Length of the palette to use (3 colors)
							(u16*) spr_bulletPal, // Load our 16 color tiles palette
							(u8*) spr_bulletBitmap // image data generated by GRIT
							);
}

void bulletLoop(void) {
	for (int i = 0; i < 2; i++) {
		if (!bulletActive[i]) continue;

		if ((bulletX[i] > 0) && (bulletX[i] < mapHsize*16)
		&& ((mapLocation[((bulletY[i]/16)*mapHsize)+(bulletX[i]/16)] == 7)
		|| (mapLocation[((bulletY[i]/16)*mapHsize)+((bulletX[i]+4)/16)] == 7))) {
			switch (bulletDir[i]) {
				case 0:
					bulletY[i] -= 8;
					break;
				case 1:
					(bulletDirection[i] ? (bulletX[i] += 8) : (bulletX[i] -= 8));
					bulletY[i] -= 8;
					break;
				case 2:
					(bulletDirection[i] ? (bulletX[i] += 8) : (bulletX[i] -= 8));
					break;
				case 3:
					(bulletDirection[i] ? (bulletX[i] += 8) : (bulletX[i] -= 8));
					bulletY[i] += 8;
					break;
			}
		} else {
			sndAmmoOut();
			bulletActive[i] = false;
		}
	}
}

void playerLoop(int pressed, int held) {
	sprintf(ammoText, "%i", ammoCount);
	sprintf(healthText, "%i", health);
	printLarge(true, 16, 12, ammoText);
	printLarge(true, 16, 20, healthText);

	touchRead(&touch);

	if (playerJump) {
		allowPlayerJump = false;
		jumpFallFrame = true;
		if ((mapLocation[((playerY/16)*mapHsize)+(playerX/16)] == 17)
		|| (mapLocation[((playerY/16)*mapHsize)+((playerX+4)/16)] == 17)
		|| playerY<0) {
			// Touched solid tile
			playerY++;
			playerJump = false;
			playerYmoveSpeed = 1;
		}
		if ((mapLocation[((playerY/16)*mapHsize)+(playerX/16)] == 7)
		|| (mapLocation[((playerY/16)*mapHsize)+((playerX+4)/16)] == 7)) {
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
		if ((mapLocation[(((playerY+31)/16)*mapHsize)+(playerX/16)] == 7)
		|| (mapLocation[(((playerY+31)/16)*mapHsize)+((playerX+4)/16)] == 7)) {
			// Make the player fall
			if (playerYmoveSpeed == 2) {
				jumpFallFrame = true;
			}
			yMoveDelay = !yMoveDelay;
			if (yMoveDelay) {
				playerY += playerYmoveSpeed;
				playerYmoveSpeed++;
			}
			if (playerYmoveSpeed > 16) playerYmoveSpeed = 16;
		}
		if ((mapLocation[(((playerY+31)/16)*mapHsize)+(playerX/16)] == 17)
		|| (mapLocation[(((playerY+31)/16)*mapHsize)+((playerX+4)/16)] == 17)) {
			// On solid tile
			playerY -= (playerY % 16);
			playerYmoveSpeed = 1;
			allowPlayerJump = true;
			jumpFallFrame = false;
		}
		if ((mapLocation[(((playerY+31)/16)*mapHsize)+(playerX/16)] == 22)
		|| (mapLocation[(((playerY+31)/16)*mapHsize)+((playerX+4)/16)] == 22)) {
			// Player is killed
			sndHurt();
			resetPlayerPosition();
			playerYmoveSpeed = 1;
			allowPlayerJump = false;
			playerDirection = true;
		}
	}

	if (held & KEY_TOUCH) {
		if (touch.py >= 48 && touch.py < 80) {
			if (touch.px >= 80 && touch.px < 112) {
				// Aim up-left
				playerDirection = false;
				aimDir = 1;
			}
			if (touch.px >= 112 && touch.px < 144) {
				// Aim up
				aimDir = 0;
			}
			if (touch.px >= 144 && touch.px < 186) {
				// Aim up-right
				playerDirection = true;
				aimDir = 1;
			}
		}
		if (touch.py >= 80 && touch.py < 112) {
			if (touch.px >= 80 && touch.px < 112) {
				// Aim left
				playerDirection = false;
				aimDir = 2;
			}
			if (touch.px >= 144 && touch.px < 186) {
				// Aim right
				playerDirection = true;
				aimDir = 2;
			}
		}
		if (touch.py >= 112 && touch.py < 144) {
			if (touch.px >= 80 && touch.px < 112) {
				// Aim down-left
				playerDirection = false;
				aimDir = 3;
			}
			if (touch.px >= 144 && touch.px < 186) {
				// Aim down-right
				playerDirection = true;
				aimDir = 3;
			}
		}
	}

	if (held & KEY_LEFT) {
		moveMore = !moveMore;
		moveDirection = false;
		//playerDirection = false;
		playerX -= 1+moveMore;
	} else if (held & KEY_RIGHT) {
		moveMore = !moveMore;
		moveDirection = true;
		//playerDirection = true;
		playerX += 1+moveMore;
	}
	
	if (((pressed & KEY_UP) || (pressed & KEY_B)) && allowPlayerJump) {
		playerJump = true;
		playerYmoveSpeed = 10;
	}
	
	if ((held & KEY_L) && !bulletActive[currentBullet]) {
		shootDelay++;
		if (ammoCount > 0 && shootDelay==6*2) {
			switch (aimDir) {
				case 0:
					bulletX[currentBullet] = playerX+(playerDirection ? 1 : 5);
					bulletY[currentBullet] = playerY+9;
					break;
				case 1:
					bulletX[currentBullet] = playerX+(playerDirection ? 8 : -4);
					bulletY[currentBullet] = playerY+5;
					break;
				case 2:
					bulletX[currentBullet] = playerX+(playerDirection ? 8 : 0);
					bulletY[currentBullet] = playerY+9;
					break;
				case 3:
					bulletX[currentBullet] = playerX+(playerDirection ? 8 : -4);
					bulletY[currentBullet] = playerY+14;
					break;
			}
			bulletDirection[currentBullet] = playerDirection;
			bulletDir[currentBullet] = aimDir;
			sndShoot();
			ammoCount--;
			bulletActive[currentBullet] = true;
			currentBullet = !currentBullet;
			shootDelay = 0;
		} else if (ammoCount == 0 && shootDelay==6*2) {
			ammoCount = 999;	// Reload ammo
		}
	} else {
		shootDelay = 5*2;
	}
	
	if (moveDirection) {
		if (mapLocation[(((playerY+31)/16)*mapHsize)+((playerX+8)/16)] == 17) {
			playerX -= 1+moveMore;
		}
	} else {
		if (mapLocation[(((playerY+31)/16)*mapHsize)+(playerX/16)] == 17) {
			playerX += 1+moveMore;
		}
	}

	animateLegs = ((held & KEY_LEFT) || (held & KEY_RIGHT));

	bulletLoop();
}

void renderPlayer(void) {
	glBoxFilled((playerX+2)-cameraXpos, (playerY+6)-cameraYpos, (playerX+8)-cameraXpos, (playerY+8)-cameraYpos, RGB15(eyeColor[0]/8, eyeColor[1]/8, eyeColor[2]/8));	// Eye color
	glColor(RGB15(31, 31, 31));																																// Back pistol color
	// Back pistol
	if (aimDir==0) {
		glSprite((playerDirection ? playerX+2 : playerX-8)-cameraXpos, (playerY-3)-cameraYpos, playerDirection ? GL_FLIP_NONE : GL_FLIP_H, &pistolImage[2]);
	} else if (aimDir==1) {
		glSprite((playerDirection ? playerX+10 : playerX-18)-cameraXpos, (playerY)-cameraYpos, playerDirection ? GL_FLIP_NONE : GL_FLIP_H, &pistolImage[1]);
	} else if (aimDir==2) {
		glSprite((playerDirection ? playerX+11 : playerX-17)-cameraXpos, (playerY+9)-cameraYpos, playerDirection ? GL_FLIP_NONE : GL_FLIP_H, &pistolImage[0]);
	} else if (aimDir==3) {
		glSprite((playerDirection ? playerX+10 : playerX-18)-cameraXpos, (playerY+13)-cameraYpos, playerDirection ? GL_FLIP_NONE : GL_FLIP_H, &pistolImage[3]);
	}
	glColor(RGB15(skinColor[0]/8, skinColor[1]/8, skinColor[2]/8));																							// Back arm/Head color
	// Back arm
	if (aimDir==0) {
		glSprite((playerDirection ? playerX+5 : playerX-11)-cameraXpos, (playerY+2)-cameraYpos, playerDirection ? GL_FLIP_NONE : GL_FLIP_H, &playerImage[13]);
	} else if (aimDir==1) {
		glSprite((playerDirection ? playerX+4 : playerX-10)-cameraXpos, (playerY+5)-cameraYpos, playerDirection ? GL_FLIP_NONE : GL_FLIP_H, &playerImage[12]);
	} else if (aimDir==2) {
		glSprite((playerDirection ? playerX+4 : playerX-10)-cameraXpos, (playerY+5)-cameraYpos, playerDirection ? GL_FLIP_NONE : GL_FLIP_H, &playerImage[8]);
	} else if (aimDir==3) {
		glSprite((playerDirection ? playerX+4 : playerX-10)-cameraXpos, (playerY+5)-cameraYpos, playerDirection ? GL_FLIP_NONE : GL_FLIP_H, &playerImage[14]);
	}
	glSprite((playerX-3)-cameraXpos, (playerY+2)-cameraYpos, playerDirection ? GL_FLIP_NONE : GL_FLIP_H, &playerImage[0]);									// Head
	glColor(RGB15(hairColor[0]/8, hairColor[1]/8, hairColor[2]/8));																							// Back arm/Head color
	glSprite((playerX-3)-cameraXpos, (playerY-1)-cameraYpos, playerDirection ? GL_FLIP_NONE : GL_FLIP_H, &playerImage[9]);									// Hair 1
	glSprite((playerX-3)-cameraXpos, (playerY-1)-cameraYpos, playerDirection ? GL_FLIP_NONE : GL_FLIP_H, &playerImage[10]);									// Hair 2
	glSprite((playerX-3)-cameraXpos, (playerY-1)-cameraYpos, playerDirection ? GL_FLIP_NONE : GL_FLIP_H, &playerImage[11]);									// Hair 3
	glColor(RGB15(torsoColor[0]/8, torsoColor[1]/8, torsoColor[2]/8));																						// Torso color
	glSprite((playerX-3)-cameraXpos, (playerY+11)-cameraYpos, playerDirection ? GL_FLIP_NONE : GL_FLIP_H, &playerImage[1]);									// Torso
	glColor(RGB15(legColor[0]/8, legColor[1]/8, legColor[2]/8));																								// Leg color
	glSprite((playerX-3)-cameraXpos, (playerY+19)-cameraYpos, playerDirection ? GL_FLIP_NONE : GL_FLIP_H, &playerImage[!jumpFallFrame ? 2+legAniFrame : 3]);	// Legs
	glColor(RGB15(feetColor[0]/8, feetColor[1]/8, feetColor[2]/8));																							// Feet color
	glSprite((playerX-3)-cameraXpos, (playerY+19)-cameraYpos, playerDirection ? GL_FLIP_NONE : GL_FLIP_H, &playerImage[!jumpFallFrame ? 5+legAniFrame : 6]);	// Feet
	glColor(RGB15(31, 31, 31));																																// Front pistol color
	// Front pistol
	if (aimDir==0) {
		glSprite((playerDirection ? playerX-3 : playerX-3)-cameraXpos, (playerY-3)-cameraYpos, playerDirection ? GL_FLIP_NONE : GL_FLIP_H, &pistolImage[2]);
	} else if (aimDir==1) {
		glSprite((playerDirection ? playerX+4 : playerX-12)-cameraXpos, (playerY)-cameraYpos, playerDirection ? GL_FLIP_NONE : GL_FLIP_H, &pistolImage[1]);
	} else if (aimDir==2) {
		glSprite((playerDirection ? playerX+5 : playerX-11)-cameraXpos, (playerY+9)-cameraYpos, playerDirection ? GL_FLIP_NONE : GL_FLIP_H, &pistolImage[0]);
	} else if (aimDir==3) {
		glSprite((playerDirection ? playerX+4 : playerX-12)-cameraXpos, (playerY+13)-cameraYpos, playerDirection ? GL_FLIP_NONE : GL_FLIP_H, &pistolImage[3]);
	}
	glColor(RGB15(skinColor[0]/8, skinColor[1]/8, skinColor[2]/8));																							// Front arm color
	// Front arm
	if (aimDir==0) {
		glSprite((playerDirection ? playerX-1 : playerX-5)-cameraXpos, (playerY+2)-cameraYpos, playerDirection ? GL_FLIP_NONE : GL_FLIP_H, &playerImage[13]);
	} else if (aimDir==1) {
		glSprite((playerDirection ? playerX-1 : playerX-5)-cameraXpos, (playerY+5)-cameraYpos, playerDirection ? GL_FLIP_NONE : GL_FLIP_H, &playerImage[12]);
	} else if (aimDir==2) {
		glSprite((playerDirection ? playerX-1 : playerX-5)-cameraXpos, (playerY+5)-cameraYpos, playerDirection ? GL_FLIP_NONE : GL_FLIP_H, &playerImage[8]);
	} else if (aimDir==3) {
		glSprite((playerDirection ? playerX-1 : playerX-5)-cameraXpos, (playerY+5)-cameraYpos, playerDirection ? GL_FLIP_NONE : GL_FLIP_H, &playerImage[14]);
	}
	glColor(RGB15(31, 31, 31));
	for (int i = 0; i < 2; i++) {
		if (bulletActive[i]) {
			glSprite(bulletX[i]-cameraXpos, bulletY[i]-cameraYpos, GL_FLIP_NONE, bulletImage);
		}
	}

	glBoxFilled(0, 9, 46, 31, RGB15(15, 15, 15));	// Rectangle
	if (ammoCount == 0) {
		glSprite(32, 10, GL_FLIP_NONE, ammoCrateImage);
	}
	glSprite(0, 8, GL_FLIP_NONE, hudImage);

	if (animateLegs) {
		legAniDelay++;
		if (legAniDelay==2) {
			legAniFrame++;
			if (legAniFrame > 2) {
				legAniFrame = 0;
			}
			legAniDelay = 0;
		}
	} else {
		legAniFrame = 0;
		legAniDelay = 1;
	}
}
