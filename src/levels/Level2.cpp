#include "GameAssets.h"
#include "Level2.h"
#include "LevelHelpers.h"

Level2::Level2(Vector2 origin, Texture2D tileset, Texture2D playerAtlas, Texture2D slimeAtlas,
	       Texture2D ghostAtlas, Texture2D chargerAtlas)
	: DungeonLevel(origin, "#3D405B", tileset, playerAtlas, slimeAtlas, ghostAtlas, chargerAtlas, 2)
{
}

void Level2::initialise()
{
	// harder than 1
	beginLevelInitialise();

	mLevelData.assign(kWidth * kHeight, TILE_WALL);
	levelGridFillRect(mLevelData, kWidth, 1, 1, kWidth - 2, kHeight - 2, TILE_FLOOR);

	// mess split up
	levelGridFillRect(mLevelData, kWidth, 3, 3, 7, 5, TILE_MESS);
	levelGridFillRect(mLevelData, kWidth, 12, 6, 16, 9, TILE_MESS);
	mLevelData[9 + 11 * kWidth] = TILE_MESS;
	mLevelData[10 + 12 * kWidth] = TILE_MESS;
	mLevelData[11 + 4 * kWidth] = TILE_MESS;

	mLevelData[(kWidth - 2) + 3 * kWidth] = TILE_EXIT_LOCKED;
	mLevelData[3 + 7 * kWidth] = TILE_HEALTH;
	mLevelData[17 + 4 * kWidth] = TILE_HEALTH;
	mLevelData[19 + 2 * kWidth] = TILE_HEALTH;

	mState.map = new Map(
		kWidth, kHeight, &mLevelData[0], mTileset, 48.0f, kDungeonTilesetCols, kDungeonTilesetRows, mOrigin, false);
	mState.messTotalAtStart = (float)mState.map->countMessTiles();
	if (mState.messTotalAtStart < 1.0f) mState.messTotalAtStart = 1.0f;

	std::map<Direction, std::vector<int> > playerAnim = makeDefaultPlayerAnimMap();
	mState.player = new Entity(
		(Vector2){ mOrigin.x - 260.0f, mOrigin.y - 40.0f },
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

	{
		Entity *slimeA = new Entity(
			(Vector2){ mOrigin.x + 180.0f, mOrigin.y - 80.0f },
			(Vector2){ 14.0f, 14.0f },
			ROLE_SLIME,
			mSlimeAtlas,
			ATLAS,
			(Vector2){ (float)kEnemyStripCols, (float)kEnemyStripRows },
			slimeAnim,
			100.0f);
		mState.enemies.push_back(slimeA);
	}
	// charger
	Entity *charger = new Entity(
		(Vector2){ mOrigin.x + 40.0f, mOrigin.y + 140.0f },
		(Vector2){ 18.0f, 18.0f },
		ROLE_CHARGER,
		mChargerAtlas,
		ATLAS,
		(Vector2){ (float)kEnemyStripCols, (float)kEnemyStripRows },
		slimeAnim,
		140.0f);
	mState.enemies.push_back(charger);

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
		(Vector2){ mOrigin.x - 120.0f, mOrigin.y + 20.0f },
		(Vector2){ 22.0f, 22.0f },
		ROLE_GHOST,
		mGhostAtlas,
		ATLAS,
		(Vector2){ (float)kEnemyStripCols, (float)kEnemyStripRows },
		ghostAnim,
		95.0f);
	ghost->setPatrol(
		(Vector2){ mOrigin.x - 220.0f, mOrigin.y + 20.0f },
		(Vector2){ mOrigin.x + 80.0f, mOrigin.y + 20.0f });
	mState.enemies.push_back(ghost);

	Entity *ghostB = new Entity(
		(Vector2){ mOrigin.x + 150.0f, mOrigin.y - 100.0f },
		(Vector2){ 22.0f, 22.0f },
		ROLE_GHOST,
		mGhostAtlas,
		ATLAS,
		(Vector2){ (float)kEnemyStripCols, (float)kEnemyStripRows },
		ghostAnim,
		88.0f);
	ghostB->setPatrol(
		(Vector2){ mOrigin.x + 40.0f, mOrigin.y - 130.0f },
		(Vector2){ mOrigin.x + 200.0f, mOrigin.y - 80.0f });
	mState.enemies.push_back(ghostB);

	loadDungeonLevelAudio(&mState, 2);
}

void Level2::render()
{
	renderDungeon();
}

void Level2::shutdown()
{
	freeLevelObjects();
	unloadAudio();
}
