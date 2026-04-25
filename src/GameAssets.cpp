#include "GameAssets.h"

static const char *kPathTileset = "assets/textures/tileset.png";
static const char *kPathPlayer = "assets/textures/player.png";
static const char *kPathSlime = "assets/textures/slime.png";
static const char *kPathGhost = "assets/textures/ghost.png";
static const char *kPathCharger = "assets/textures/charger.png";

void LoadGameTextures(GameTextures *out)
{
	out->tileset = (Texture2D){ 0 };
	out->player = (Texture2D){ 0 };
	out->slime = (Texture2D){ 0 };
	out->ghost = (Texture2D){ 0 };
	out->charger = (Texture2D){ 0 };

	out->tileset = LoadTexture(kPathTileset);
	out->player = LoadTexture(kPathPlayer);
	out->slime = LoadTexture(kPathSlime);
	out->ghost = LoadTexture(kPathGhost);
	out->charger = LoadTexture(kPathCharger);
}

void UnloadGameTextures(GameTextures *t)
{
	if (!t) return;
	if (t->tileset.id != 0) UnloadTexture(t->tileset);
	if (t->player.id != 0) UnloadTexture(t->player);
	if (t->slime.id != 0) UnloadTexture(t->slime);
	if (t->ghost.id != 0) UnloadTexture(t->ghost);
	if (t->charger.id != 0) UnloadTexture(t->charger);
	t->tileset = t->player = t->slime = t->ghost = t->charger = (Texture2D){ 0 };
}
