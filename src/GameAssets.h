#ifndef GAME_ASSETS_H
#define GAME_ASSETS_H

#include "AppCommon.h"

// Swap in your own PNGs under assets/textures/ — SOURCES.txt spells out the layout.
// Anything missing gets drawn at runtime instead (no crash).

struct GameTextures
{
	Texture2D tileset;
	Texture2D player;
	Texture2D slime;
	Texture2D ghost;
	Texture2D charger;
};

// Point sampling so resized sprites don't go mushy.
void LoadGameTextures(GameTextures *out);
void UnloadGameTextures(GameTextures *t);

// sheet dims: .x = columns across, .y = rows down (UVs in Entity.cpp).
static const int kPlayerAtlasCols = 2;
static const int kPlayerAtlasRows = 4;
static const int kEnemyStripCols = 4;
static const int kEnemyStripRows = 1;

// Tile strip, left to right: floor, wall, mess, locked door, open door, health
static const int kDungeonTilesetCols = 6;
static const int kDungeonTilesetRows = 1;

#endif
