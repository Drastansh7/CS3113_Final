#include "Map.h"

Map::Map(int mapColumns, int mapRows, unsigned int *levelData, Texture2D sharedAtlas,
	 float tileSize, int textureColumns, int textureRows, Vector2 origin, bool ownsTexture)
	: mMapColumns(mapColumns),
	  mMapRows(mapRows),
	  mLevelData(levelData),
	  mTextureAtlas(sharedAtlas),
	  mTileSize(tileSize),
	  mTextureColumns(textureColumns),
	  mTextureRows(textureRows),
	  mOrigin(origin),
	  mOwnsTexture(ownsTexture)
{
	build();
}

Map::~Map()
{
	if (mOwnsTexture && mTextureAtlas.id != 0) {
		UnloadTexture(mTextureAtlas);
		mTextureAtlas = (Texture2D){ 0 };
	}
}

void Map::build()
{
	mLeftBoundary = mOrigin.x - (mMapColumns * mTileSize) / 2.0f;
	mRightBoundary = mOrigin.x + (mMapColumns * mTileSize) / 2.0f;
	mTopBoundary = mOrigin.y - (mMapRows * mTileSize) / 2.0f;
	mBottomBoundary = mOrigin.y + (mMapRows * mTileSize) / 2.0f;

	mTextureAreas.clear();
	for (int row = 0; row < mTextureRows; row++) {
		for (int col = 0; col < mTextureColumns; col++) {
			Rectangle textureArea = {
				(float)col * ((float)mTextureAtlas.width / (float)mTextureColumns),
				(float)row * ((float)mTextureAtlas.height / (float)mTextureRows),
				(float)mTextureAtlas.width / (float)mTextureColumns,
				(float)mTextureAtlas.height / (float)mTextureRows
			};
			mTextureAreas.push_back(textureArea);
		}
	}
}

void Map::render()
{
	for (int row = 0; row < mMapRows; row++) {
		for (int col = 0; col < mMapColumns; col++) {
			unsigned int tile = mLevelData[row * mMapColumns + col];
			if (tile == TILE_VOID) continue;

			int atlasIndex = (int)tile - 1;
			if (atlasIndex < 0 || atlasIndex >= (int)mTextureAreas.size()) continue;

			Rectangle destinationArea = {
				mLeftBoundary + col * mTileSize,
				mTopBoundary + row * mTileSize,
				mTileSize,
				mTileSize
			};

			DrawTexturePro(
				mTextureAtlas,
				mTextureAreas[atlasIndex],
				destinationArea,
				(Vector2){ 0.0f, 0.0f },
				0.0f,
				WHITE);
		}
	}
}

bool Map::blocksTile(unsigned int tile) const
{
	return tile == TILE_WALL || tile == TILE_EXIT_LOCKED;
}

bool Map::worldToCell(Vector2 world, int *outCol, int *outRow) const
{
	if (world.x < mLeftBoundary || world.x > mRightBoundary || world.y < mTopBoundary ||
	    world.y > mBottomBoundary)
		return false;

	int col = (int)floorf((world.x - mLeftBoundary) / mTileSize);
	int row = (int)floorf((world.y - mTopBoundary) / mTileSize);
	if (col < 0 || col >= mMapColumns || row < 0 || row >= mMapRows) return false;
	*outCol = col;
	*outRow = row;
	return true;
}

unsigned int Map::getTile(int col, int row) const
{
	if (col < 0 || col >= mMapColumns || row < 0 || row >= mMapRows) return TILE_WALL;
	return mLevelData[row * mMapColumns + col];
}

void Map::setTile(int col, int row, unsigned int tile)
{
	if (col < 0 || col >= mMapColumns || row < 0 || row >= mMapRows) return;
	mLevelData[row * mMapColumns + col] = tile;
}

bool Map::isWorldBlocked(Vector2 center, Vector2 halfSize) const
{
	Vector2 corners[4] = {
		{ center.x - halfSize.x, center.y - halfSize.y },
		{ center.x + halfSize.x, center.y - halfSize.y },
		{ center.x - halfSize.x, center.y + halfSize.y },
		{ center.x + halfSize.x, center.y + halfSize.y },
	};

	for (int i = 0; i < 4; i++) {
		int c = 0, r = 0;
		if (!worldToCell(corners[i], &c, &r)) return true;
		unsigned int t = getTile(c, r);
		if (t == TILE_VOID) return true;
		if (blocksTile(t)) return true;
	}
	return false;
}

int Map::countMessTiles() const
{
	int n = 0;
	for (int i = 0; i < mMapColumns * mMapRows; i++) {
		if (mLevelData[i] == TILE_MESS) n++;
	}
	return n;
}

void Map::unlockExitsIfClear()
{
	if (countMessTiles() > 0) return;
	for (int row = 0; row < mMapRows; row++) {
		for (int col = 0; col < mMapColumns; col++) {
			if (getTile(col, row) == TILE_EXIT_LOCKED) setTile(col, row, TILE_EXIT_OPEN);
		}
	}
}

// clean mess tiles in range to unlock exits
bool Map::tryCleanMessNear(Vector2 worldPos, float radiusPx)
{
	bool cleaned = false;
	float rTiles = radiusPx / mTileSize;
	int pc = 0, pr = 0;
	if (!worldToCell(worldPos, &pc, &pr)) return false;

	int span = (int)ceilf(rTiles) + 1;
	for (int dr = -span; dr <= span; dr++) {
		for (int dc = -span; dc <= span; dc++) {
			int c = pc + dc;
			int r = pr + dr;
			if (getTile(c, r) != TILE_MESS) continue;

			float cx = mLeftBoundary + c * mTileSize + mTileSize * 0.5f;
			float cy = mTopBoundary + r * mTileSize + mTileSize * 0.5f;
			float dx = worldPos.x - cx;
			float dy = worldPos.y - cy;
			if (dx * dx + dy * dy <= radiusPx * radiusPx) {
				setTile(c, r, TILE_FLOOR);
				cleaned = true;
			}
		}
	}
	if (cleaned) unlockExitsIfClear();
	return cleaned;
}

int Map::tryCollectHealthNear(Vector2 worldPos, float radiusPx)
{
	int collected = 0;
	float rTiles = radiusPx / mTileSize;
	int pc = 0, pr = 0;
	if (!worldToCell(worldPos, &pc, &pr)) return 0;

	int span = (int)ceilf(rTiles) + 1;
	for (int dr = -span; dr <= span; dr++) {
		for (int dc = -span; dc <= span; dc++) {
			int c = pc + dc;
			int r = pr + dr;
			if (getTile(c, r) != TILE_HEALTH) continue;

			float cx = mLeftBoundary + c * mTileSize + mTileSize * 0.5f;
			float cy = mTopBoundary + r * mTileSize + mTileSize * 0.5f;
			float dx = worldPos.x - cx;
			float dy = worldPos.y - cy;
			if (dx * dx + dy * dy <= radiusPx * radiusPx) {
				setTile(c, r, TILE_FLOOR);
				collected++;
			}
		}
	}
	return collected;
}

bool Map::isOnOpenExit(Vector2 center, Vector2 halfSize) const
{
	Vector2 pts[3] = {
		center,
		{ center.x - halfSize.x * 0.5f, center.y },
		{ center.x + halfSize.x * 0.5f, center.y },
	};
	for (int i = 0; i < 3; i++) {
		int c = 0, r = 0;
		if (worldToCell(pts[i], &c, &r) && getTile(c, r) == TILE_EXIT_OPEN) return true;
	}
	return false;
}

float Map::curseRatio() const
{
	int total = 0, messy = 0;
	for (int i = 0; i < mMapColumns * mMapRows; i++) {
		unsigned int t = mLevelData[i];
		if (t == TILE_FLOOR || t == TILE_MESS) {
			total++;
			if (t == TILE_MESS) messy++;
		}
	}
	if (total <= 0) return 0.0f;
	return (float)messy / (float)total;
}
