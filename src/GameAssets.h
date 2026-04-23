#ifndef GAME_ASSETS_H
#define GAME_ASSETS_H

#include "AppCommon.h"

// Put PNGs in assets textures optional see SOURCES else procedural placeholders

struct GameTextures
{
	Texture2D tileset;
	Texture2D player;
	Texture2D slime;
	Texture2D ghost;
	Texture2D charger;
};

// point filtering so scaled art stays crisp
void LoadGameTextures(GameTextures *out);
void UnloadGameTextures(GameTextures *t);

// player sheet 2x4 enemies one strip of four frames UVs in Entity
static const int kPlayerAtlasCols = 2;
static const int kPlayerAtlasRows = 4;
static const int kEnemyStripCols = 4;
static const int kEnemyStripRows = 1;

// tile order is floor wall mess locked exit open exit health
static const int kDungeonTilesetCols = 6;
static const int kDungeonTilesetRows = 1;

#endif
