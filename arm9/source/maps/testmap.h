#ifndef TESTMAP_H
#define TESTMAP_H

#include <nds.h>

#define nullTile 7
#define blckTile 17
#define killTile 22

// Map size (tile count): 32x12

u8 testMap[32*15] = {
	nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile,
	nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile,
	nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile,
	nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile,
	nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile,
	nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, blckTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile,
	nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, blckTile, blckTile, blckTile, blckTile, blckTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile,
	nullTile, nullTile, nullTile, nullTile, blckTile, nullTile, nullTile, blckTile, blckTile, blckTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile,
	nullTile, nullTile, nullTile, nullTile, blckTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile,
	nullTile, nullTile, nullTile, nullTile, nullTile, blckTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile,
	nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, blckTile, blckTile, blckTile, blckTile, blckTile, blckTile, blckTile, blckTile, blckTile, nullTile, blckTile, blckTile, nullTile, nullTile, blckTile, blckTile, nullTile, nullTile, nullTile, blckTile, blckTile, nullTile, nullTile, nullTile, nullTile, nullTile,
	nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile,
	// Unseen
	nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile,
	nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile, nullTile,
	killTile, killTile, killTile, killTile, killTile, killTile, killTile, killTile, killTile, killTile, killTile, killTile, killTile, killTile, killTile, killTile, killTile, killTile, killTile, killTile, killTile, killTile, killTile, killTile, killTile, killTile, killTile, killTile, killTile, killTile, killTile, killTile,
};

#endif	// TESTMAP_H