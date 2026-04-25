#include "GameAssets.h"
#include "Level1.h"
#include "LevelHelpers.h"

Level1::Level1(Vector2 origin, Texture2D tileset, Texture2D playerAtlas, Texture2D slimeAtlas,
	       Texture2D ghostAtlas, Texture2D chargerAtlas)
	: DungeonLevel(origin, "#2B2D42", tileset, playerAtlas, slimeAtlas, ghostAtlas, chargerAtlas, 1)
{
}

void Level1::initialise()
{
	// easy intro
	beginLevelInitialise();

	// room
	mLevelData.assign(kWidth * kHeight, TILE_WALL);
	levelGridFillRect(mLevelData, kWidth, 1, 1, kWidth - 2, kHeight - 2, TILE_FLOOR);

	// mess
	levelGridFillRect(mLevelData, kWidth, 5, 3, 8, 6, TILE_MESS);
	mLevelData[10 + 4 * kWidth] = TILE_MESS;
	mLevelData[11 + 5 * kWidth] = TILE_MESS;
	mLevelData[12 + 8 * kWidth] = TILE_MESS;

	mLevelData[(kWidth - 2) + 2 * kWidth] = TILE_EXIT_LOCKED;
	mLevelData[2 + 2 * kWidth] = TILE_HEALTH;
	mLevelData[14 + 9 * kWidth] = TILE_HEALTH;

	mState.map = new Map(
		kWidth, kHeight, &mLevelData[0], mTileset, 48.0f, kDungeonTilesetCols, kDungeonTilesetRows, mOrigin, false);
	mState.messTotalAtStart = (float)mState.map->countMessTiles();
	if (mState.messTotalAtStart < 1.0f) mState.messTotalAtStart = 1.0f;

	std::map<Direction, std::vector<int> > playerAnim = makeDefaultPlayerAnimMap();
	mState.player = new Entity(
		(Vector2){ mOrigin.x - 200.0f, mOrigin.y },
		(Vector2){ 20.0f, 20.0f },
		ROLE_PLAYER,
		mPlayerAtlas,
		ATLAS,
		(Vector2){ (float)kPlayerAtlasCols, (float)kPlayerAtlasRows },
		playerAnim,
		(float)Entity::DEFAULT_PLAYER_SPEED);

	std::map<Direction, std::vector<int> > slimeAnim;
	slimeAnim[DIR_LEFT] = std::vector<int>(4);
	slimeAnim[DIR_LEFT][0] = 0;
	slimeAnim[DIR_LEFT][1] = 1;
	slimeAnim[DIR_LEFT][2] = 2;
	slimeAnim[DIR_LEFT][3] = 3;

	Entity *slime = new Entity(
		(Vector2){ mOrigin.x + 120.0f, mOrigin.y - 40.0f },
		(Vector2){ 14.0f, 14.0f },
		ROLE_SLIME,
		mSlimeAtlas,
		ATLAS,
		(Vector2){ (float)kEnemyStripCols, (float)kEnemyStripRows },
		slimeAnim,
		95.0f);
	mState.enemies.push_back(slime);

	std::vector<int> ghostFrames;
	ghostFrames.push_back(0);
	ghostFrames.push_back(1);
	ghostFrames.push_back(2);
	ghostFrames.push_back(3);
	std::map<Direction, std::vector<int> > ghostAnim;
	ghostAnim[DIR_LEFT] = ghostFrames;
	ghostAnim[DIR_RIGHT] = ghostFrames;
	ghostAnim[DIR_UP] = ghostFrames;
	ghostAnim[DIR_DOWN] = ghostFrames;

	Entity *ghost = new Entity(
		(Vector2){ mOrigin.x - 80.0f, mOrigin.y + 100.0f },
		(Vector2){ 22.0f, 22.0f },
		ROLE_GHOST,
		mGhostAtlas,
		ATLAS,
		(Vector2){ (float)kEnemyStripCols, (float)kEnemyStripRows },
		ghostAnim,
		80.0f);
	ghost->setPatrol(
		(Vector2){ mOrigin.x - 160.0f, mOrigin.y + 100.0f },
		(Vector2){ mOrigin.x + 40.0f, mOrigin.y + 100.0f });
	mState.enemies.push_back(ghost);

	loadDungeonLevelAudio(&mState, 1);
}

void Level1::render()
{
	renderDungeon();
}

void Level1::shutdown()
{
	freeLevelObjects();
	unloadAudio();
}
