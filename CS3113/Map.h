#ifndef MAP_H
#define MAP_H

#include "cs3113.h"

// 0 = TILE_VOID (nothing drawn). Otherwise atlas column is id minus 1.
static const unsigned int TILE_VOID = 0;
static const unsigned int TILE_FLOOR = 1;
static const unsigned int TILE_WALL = 2;
static const unsigned int TILE_MESS = 3;
static const unsigned int TILE_EXIT_LOCKED = 4;
static const unsigned int TILE_EXIT_OPEN = 5;
static const unsigned int TILE_HEALTH = 6;

class Map
{
private:
	int mMapColumns;
	int mMapRows;
	unsigned int *mLevelData;
	Texture2D mTextureAtlas;
	float mTileSize;
	int mTextureColumns;
	int mTextureRows;
	std::vector<Rectangle> mTextureAreas;
	Vector2 mOrigin;
	float mLeftBoundary;
	float mRightBoundary;
	float mTopBoundary;
	float mBottomBoundary;

public:
	Map(int mapColumns, int mapRows, unsigned int *levelData, Texture2D sharedAtlas,
	    float tileSize, int textureColumns, int textureRows, Vector2 origin,
	    bool ownsTexture);
	~Map();

	void build();
	void render();

	bool blocksTile(unsigned int tile) const;
	bool isWorldBlocked(Vector2 center, Vector2 halfSize) const;
	bool worldToCell(Vector2 world, int *outCol, int *outRow) const;
	unsigned int getTile(int col, int row) const;
	void setTile(int col, int row, unsigned int tile);

	int countMessTiles() const;
	void unlockExitsIfClear();
	bool tryCleanMessNear(Vector2 worldPos, float radiusPx);
	int tryCollectHealthNear(Vector2 worldPos, float radiusPx);

	bool isOnOpenExit(Vector2 center, Vector2 halfSize) const;

	int getMapColumns() const { return mMapColumns; }
	int getMapRows() const { return mMapRows; }
	float getTileSize() const { return mTileSize; }
	Vector2 getOrigin() const { return mOrigin; }
	float getLeftBoundary() const { return mLeftBoundary; }
	float getTopBoundary() const { return mTopBoundary; }

	float curseRatio() const;

private:
	bool mOwnsTexture;
};

#endif
