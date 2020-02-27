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

static int currentChars = 2;
static bool playerJump[2] = {false};
static bool allowPlayerJump[2] = {false};
static bool jumpFallFrame[2] = {true};
static bool playerDirection[2] = {true};
static bool moveDirection[2] = {true};
static bool animateLegs[2] = {false};
static bool moveMore[2] = {false};
static int yMoveDelay[2] = {0};
static int hudTexID, crateTexID, playerTexID, pistolTexID, bulletTexID;
static glImage hudImage[64 * 32];
static glImage ammoCrateImage[16 * 16];
static glImage playerImage[(256 / 16) * (16 / 16)];
static glImage pistolImage[(64 / 16) * (16 / 16)];
static glImage bulletImage[8 * 8];

static touchPosition touch;
int playerX[2], playerY[2], playerStartX[2], playerStartY[2];
static int bulletX[2], bulletY[2], aimDir[2]={2}, bulletDir[2], shootDelay[2];
static bool currentBullet = false;
static bool bulletActive[2] = {false};
static bool bulletDirection[2] = {false};

static int health=100, ammoCount=999, playerYmoveSpeed[2]={1}, legAniFrame[2], legAniDelay[2];
static char healthText[8], ammoText[8];

void initPlayers(int numberOfChars) {
	currentChars = numberOfChars;
	for (int i = 0; i < currentChars; i++) {
		playerJump[i] = false;
		allowPlayerJump[i] = false;
		jumpFallFrame[i] = true;
		playerDirection[i] = true;
		moveDirection[i] = true;
		animateLegs[i] = false;
		moveMore[i] = false;
		yMoveDelay[i] = false;
		aimDir[i] = 2;
		shootDelay[i] = 0;
	}
}

void setPlayerPosition(int id, int x, int y) {
	playerStartX[id] = x;
	playerStartY[id] = y;
	playerX[id] = playerStartX[id];
	playerY[id] = playerStartY[id];
}

void resetPlayerPosition(int id) {
	playerX[id] = playerStartX[id];
	playerY[id] = playerStartY[id];
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

  for (int i = 0; i < currentChars; i++) {
	if (playerJump[i]) {
		allowPlayerJump[i] = false;
		jumpFallFrame[i] = true;
		if ((mapLocation[((playerY[i]/16)*mapHsize)+(playerX[i]/16)] == 17)
		|| (mapLocation[((playerY[i]/16)*mapHsize)+((playerX[i]+4)/16)] == 17)
		|| playerY[i]<0) {
			// Touched solid tile
			playerY[i]++;
			playerJump[i] = false;
			playerYmoveSpeed[i] = 1;
		}
		if ((mapLocation[((playerY[i]/16)*mapHsize)+(playerX[i]/16)] == 7)
		|| (mapLocation[((playerY[i]/16)*mapHsize)+((playerX[i]+4)/16)] == 7)) {
			// Make the player jump
			yMoveDelay[i]++;
			playerY[i] -= playerYmoveSpeed[i];
			if (yMoveDelay[i]==4) {
				playerYmoveSpeed[i]--;
				yMoveDelay[i] = 0;
			}
			if (playerYmoveSpeed[i] < 1) {
				playerJump[i] = false;
				playerYmoveSpeed[i] = 1;
			}
		}
	} else {
		if ((mapLocation[(((playerY[i]+31)/16)*mapHsize)+(playerX[i]/16)] == 7)
		|| (mapLocation[(((playerY[i]+31)/16)*mapHsize)+((playerX[i]+4)/16)] == 7)) {
			// Make the player fall
			if (playerYmoveSpeed[i] == 2) {
				jumpFallFrame[i] = true;
			}
			yMoveDelay[i]++;
			playerY[i] += playerYmoveSpeed[i];
			if (yMoveDelay[i]==4) {
				playerYmoveSpeed[i]++;
				yMoveDelay[i] = 0;
			}
			if (playerYmoveSpeed[i] > 8) playerYmoveSpeed[i] = 8;
		}
		if ((mapLocation[(((playerY[i]+31)/16)*mapHsize)+(playerX[i]/16)] == 17)
		|| (mapLocation[(((playerY[i]+31)/16)*mapHsize)+((playerX[i]+4)/16)] == 17)) {
			// On solid tile
			playerY[i] -= (playerY[i] % 16);
			playerYmoveSpeed[i] = 1;
			allowPlayerJump[i] = true;
			jumpFallFrame[i] = false;
			yMoveDelay[i] = 0;
		}
		if ((mapLocation[(((playerY[i]+31)/16)*mapHsize)+(playerX[i]/16)] == 22)
		|| (mapLocation[(((playerY[i]+31)/16)*mapHsize)+((playerX[i]+4)/16)] == 22)) {
			// Player is killed
			sndHurt();
			resetPlayerPosition(i);
			playerYmoveSpeed[i] = 1;
			allowPlayerJump[i] = false;
			playerDirection[i] = true;
			yMoveDelay[i] = 0;
		}
	}
  }

	if (held & KEY_TOUCH) {
		if (touch.py >= 48 && touch.py < 80) {
			if (touch.px >= 80 && touch.px < 112) {
				// Aim up-left
				playerDirection[0] = false;
				aimDir[0] = 1;
			}
			if (touch.px >= 112 && touch.px < 144) {
				// Aim up
				aimDir[0] = 0;
			}
			if (touch.px >= 144 && touch.px < 186) {
				// Aim up-right
				playerDirection[0] = true;
				aimDir[0] = 1;
			}
		}
		if (touch.py >= 80 && touch.py < 112) {
			if (touch.px >= 80 && touch.px < 112) {
				// Aim left
				playerDirection[0] = false;
				aimDir[0] = 2;
			}
			if (touch.px >= 144 && touch.px < 186) {
				// Aim right
				playerDirection[0] = true;
				aimDir[0] = 2;
			}
		}
		if (touch.py >= 112 && touch.py < 144) {
			if (touch.px >= 80 && touch.px < 112) {
				// Aim down-left
				playerDirection[0] = false;
				aimDir[0] = 3;
			}
			if (touch.px >= 144 && touch.px < 186) {
				// Aim down-right
				playerDirection[0] = true;
				aimDir[0] = 3;
			}
		}
	}

	if (held & KEY_LEFT) {
		moveMore[0] = !moveMore[0];
		moveDirection[0] = false;
		//playerDirection[0] = false;
		playerX[0] -= 1+moveMore[0];
	} else if (held & KEY_RIGHT) {
		moveMore[0] = !moveMore[0];
		moveDirection[0] = true;
		//playerDirection = true;
		playerX[0] += 1+moveMore[0];
	}
	
	if (((pressed & KEY_UP) || (pressed & KEY_B)) && allowPlayerJump[0]) {
		playerJump[0] = true;
		playerYmoveSpeed[0] = 5;
	}
	
	if ((held & KEY_L) && !bulletActive[currentBullet]) {
		shootDelay[0]++;
		if (ammoCount > 0 && shootDelay[0]==6*2) {
			switch (aimDir[0]) {
				case 0:
					bulletX[currentBullet] = playerX[0]+(playerDirection[0] ? 1 : 5);
					bulletY[currentBullet] = playerY[0]+9;
					break;
				case 1:
					bulletX[currentBullet] = playerX[0]+(playerDirection[0] ? 8 : -4);
					bulletY[currentBullet] = playerY[0]+5;
					break;
				case 2:
					bulletX[currentBullet] = playerX[0]+(playerDirection[0] ? 8 : 0);
					bulletY[currentBullet] = playerY[0]+9;
					break;
				case 3:
					bulletX[currentBullet] = playerX[0]+(playerDirection[0] ? 8 : -4);
					bulletY[currentBullet] = playerY[0]+14;
					break;
			}
			bulletDirection[currentBullet] = playerDirection[0];
			bulletDir[currentBullet] = aimDir[0];
			sndShoot();
			ammoCount--;
			bulletActive[currentBullet] = true;
			currentBullet = !currentBullet;
			shootDelay[0] = 0;
		} else if (ammoCount == 0 && shootDelay[0]==6*2) {
			ammoCount = 999;	// Reload ammo
		}
	} else {
		shootDelay[0] = 5*2;
	}
	
  for (int i = 0; i < currentChars; i++) {
	if (moveDirection[i]) {
		if (mapLocation[(((playerY[i]+31)/16)*mapHsize)+((playerX[i]+8)/16)] == 17) {
			playerX[i] -= 1+moveMore[i];
		}
	} else {
		if (mapLocation[(((playerY[i]+31)/16)*mapHsize)+(playerX[i]/16)] == 17) {
			playerX[i] += 1+moveMore[i];
		}
	}
  }

	animateLegs[0] = ((held & KEY_LEFT) || (held & KEY_RIGHT));

	bulletLoop();
}

void renderPlayer(void) {
  for (int i = 0; i < currentChars; i++) {
	glBoxFilled((playerX[i]+2)-cameraXpos, (playerY[i]+6)-cameraYpos, (playerX[i]+8)-cameraXpos, (playerY[i]+8)-cameraYpos, RGB15(eyeColor[0]/8, eyeColor[1]/8, eyeColor[2]/8));	// Eye color
	glColor(RGB15(31, 31, 31));																																// Back pistol color
	// Back pistol
	if (aimDir[i]==0) {
		glSprite((playerDirection[i] ? playerX[i]+2 : playerX[i]-8)-cameraXpos, (playerY[i]-3)-cameraYpos, playerDirection[i] ? GL_FLIP_NONE : GL_FLIP_H, &pistolImage[2]);
	} else if (aimDir[i]==1) {
		glSprite((playerDirection[i] ? playerX[i]+10 : playerX[i]-18)-cameraXpos, (playerY[i])-cameraYpos, playerDirection[i] ? GL_FLIP_NONE : GL_FLIP_H, &pistolImage[1]);
	} else if (aimDir[i]==2) {
		glSprite((playerDirection[i] ? playerX[i]+11 : playerX[i]-17)-cameraXpos, (playerY[i]+9)-cameraYpos, playerDirection[i] ? GL_FLIP_NONE : GL_FLIP_H, &pistolImage[0]);
	} else if (aimDir[i]==3) {
		glSprite((playerDirection[i] ? playerX[i]+10 : playerX[i]-18)-cameraXpos, (playerY[i]+13)-cameraYpos, playerDirection[i] ? GL_FLIP_NONE : GL_FLIP_H, &pistolImage[3]);
	}
	glColor(RGB15(skinColor[0]/8, skinColor[1]/8, skinColor[2]/8));																							// Back arm/Head color
	// Back arm
	if (aimDir[i]==0) {
		glSprite((playerDirection[i] ? playerX[i]+5 : playerX[i]-11)-cameraXpos, (playerY[i]+2)-cameraYpos, playerDirection[i] ? GL_FLIP_NONE : GL_FLIP_H, &playerImage[13]);
	} else if (aimDir[i]==1) {
		glSprite((playerDirection[i] ? playerX[i]+4 : playerX[i]-10)-cameraXpos, (playerY[i]+5)-cameraYpos, playerDirection[i] ? GL_FLIP_NONE : GL_FLIP_H, &playerImage[12]);
	} else if (aimDir[i]==2) {
		glSprite((playerDirection[i] ? playerX[i]+4 : playerX[i]-10)-cameraXpos, (playerY[i]+5)-cameraYpos, playerDirection[i] ? GL_FLIP_NONE : GL_FLIP_H, &playerImage[8]);
	} else if (aimDir[i]==3) {
		glSprite((playerDirection[i] ? playerX[i]+4 : playerX[i]-10)-cameraXpos, (playerY[i]+5)-cameraYpos, playerDirection[i] ? GL_FLIP_NONE : GL_FLIP_H, &playerImage[14]);
	}
	glSprite((playerX[i]-3)-cameraXpos, (playerY[i]+2)-cameraYpos, playerDirection[i] ? GL_FLIP_NONE : GL_FLIP_H, &playerImage[0]);									// Head
	glColor(RGB15(hairColor[0]/8, hairColor[1]/8, hairColor[2]/8));																							// Back arm/Head color
	glSprite((playerX[i]-3)-cameraXpos, (playerY[i]-1)-cameraYpos, playerDirection[i] ? GL_FLIP_NONE : GL_FLIP_H, &playerImage[9]);									// Hair 1
	glSprite((playerX[i]-3)-cameraXpos, (playerY[i]-1)-cameraYpos, playerDirection[i] ? GL_FLIP_NONE : GL_FLIP_H, &playerImage[10]);									// Hair 2
	glSprite((playerX[i]-3)-cameraXpos, (playerY[i]-1)-cameraYpos, playerDirection[i] ? GL_FLIP_NONE : GL_FLIP_H, &playerImage[11]);									// Hair 3
	glColor(RGB15(torsoColor[0]/8, torsoColor[1]/8, torsoColor[2]/8));																						// Torso color
	glSprite((playerX[i]-3)-cameraXpos, (playerY[i]+11)-cameraYpos, playerDirection[i] ? GL_FLIP_NONE : GL_FLIP_H, &playerImage[1]);									// Torso
	glColor(RGB15(legColor[0]/8, legColor[1]/8, legColor[2]/8));																								// Leg color
	glSprite((playerX[i]-3)-cameraXpos, (playerY[i]+19)-cameraYpos, playerDirection[i] ? GL_FLIP_NONE : GL_FLIP_H, &playerImage[!jumpFallFrame[i] ? 2+legAniFrame[i] : 3]);	// Legs
	glColor(RGB15(feetColor[0]/8, feetColor[1]/8, feetColor[2]/8));																							// Feet color
	glSprite((playerX[i]-3)-cameraXpos, (playerY[i]+19)-cameraYpos, playerDirection[i] ? GL_FLIP_NONE : GL_FLIP_H, &playerImage[!jumpFallFrame[i] ? 5+legAniFrame[i] : 6]);	// Feet
	glColor(RGB15(31, 31, 31));																																// Front pistol color
	// Front pistol
	if (aimDir[i]==0) {
		glSprite((playerDirection[i] ? playerX[i]-3 : playerX[i]-3)-cameraXpos, (playerY[i]-3)-cameraYpos, playerDirection[i] ? GL_FLIP_NONE : GL_FLIP_H, &pistolImage[2]);
	} else if (aimDir[i]==1) {
		glSprite((playerDirection[i] ? playerX[i]+4 : playerX[i]-12)-cameraXpos, (playerY[i])-cameraYpos, playerDirection[i] ? GL_FLIP_NONE : GL_FLIP_H, &pistolImage[1]);
	} else if (aimDir[i]==2) {
		glSprite((playerDirection[i] ? playerX[i]+5 : playerX[i]-11)-cameraXpos, (playerY[i]+9)-cameraYpos, playerDirection[i] ? GL_FLIP_NONE : GL_FLIP_H, &pistolImage[0]);
	} else if (aimDir[i]==3) {
		glSprite((playerDirection[i] ? playerX[i]+4 : playerX[i]-12)-cameraXpos, (playerY[i]+13)-cameraYpos, playerDirection[i] ? GL_FLIP_NONE : GL_FLIP_H, &pistolImage[3]);
	}
	glColor(RGB15(skinColor[0]/8, skinColor[1]/8, skinColor[2]/8));																							// Front arm color
	// Front arm
	if (aimDir[i]==0) {
		glSprite((playerDirection[i] ? playerX[i]-1 : playerX[i]-5)-cameraXpos, (playerY[i]+2)-cameraYpos, playerDirection[i] ? GL_FLIP_NONE : GL_FLIP_H, &playerImage[13]);
	} else if (aimDir[i]==1) {
		glSprite((playerDirection[i] ? playerX[i]-1 : playerX[i]-5)-cameraXpos, (playerY[i]+5)-cameraYpos, playerDirection[i] ? GL_FLIP_NONE : GL_FLIP_H, &playerImage[12]);
	} else if (aimDir[i]==2) {
		glSprite((playerDirection[i] ? playerX[i]-1 : playerX[i]-5)-cameraXpos, (playerY[i]+5)-cameraYpos, playerDirection[i] ? GL_FLIP_NONE : GL_FLIP_H, &playerImage[8]);
	} else if (aimDir[i]==3) {
		glSprite((playerDirection[i] ? playerX[i]-1 : playerX[i]-5)-cameraXpos, (playerY[i]+5)-cameraYpos, playerDirection[i] ? GL_FLIP_NONE : GL_FLIP_H, &playerImage[14]);
	}
	glColor(RGB15(31, 31, 31));
  }
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

  for (int i = 0; i < currentChars; i++) {
	if (animateLegs[i]) {
		legAniDelay[i]++;
		if (legAniDelay[i]==4) {
			legAniFrame[i]++;
			if (legAniFrame[i] > 2) {
				legAniFrame[i] = 0;
			}
			legAniDelay[i] = 0;
		}
	} else {
		legAniFrame[i] = 0;
		legAniDelay[i] = 1;
	}
  }
}
