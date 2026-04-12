#include "GameAssets.h"

static const char *kPathTileset = "assets/textures/tileset.png";
static const char *kPathPlayer = "assets/textures/player.png";
static const char *kPathSlime = "assets/textures/slime.png";
static const char *kPathGhost = "assets/textures/ghost.png";
static const char *kPathCharger = "assets/textures/charger.png";

static void pointSample(Texture2D *t)
{
	if (t && t->id != 0) SetTextureFilter(*t, TEXTURE_FILTER_POINT);
}

void LoadGameTextures(GameTextures *out)
{
	out->tileset = (Texture2D){ 0 };
	out->player = (Texture2D){ 0 };
	out->slime = (Texture2D){ 0 };
	out->ghost = (Texture2D){ 0 };
	out->charger = (Texture2D){ 0 };

	if (FileExists(kPathTileset)) {
		out->tileset = LoadTexture(kPathTileset);
		pointSample(&out->tileset);
	} else {
		out->tileset = GenerateTilesetTexture();
		pointSample(&out->tileset);
	}

	if (FileExists(kPathPlayer)) {
		out->player = LoadTexture(kPathPlayer);
		pointSample(&out->player);
	} else {
		out->player = GenerateJanitorAtlas();
		pointSample(&out->player);
	}

	if (FileExists(kPathSlime)) {
		out->slime = LoadTexture(kPathSlime);
		pointSample(&out->slime);
	} else {
		out->slime = GenerateSlimeAtlas();
		pointSample(&out->slime);
	}

	if (FileExists(kPathGhost)) {
		out->ghost = LoadTexture(kPathGhost);
		pointSample(&out->ghost);
	} else {
		out->ghost = GenerateGhostAtlas();
		pointSample(&out->ghost);
	}

	if (FileExists(kPathCharger)) {
		out->charger = LoadTexture(kPathCharger);
		pointSample(&out->charger);
	} else {
		out->charger = GenerateChargerAtlas();
		pointSample(&out->charger);
	}
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
