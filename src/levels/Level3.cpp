#include "GameAssets.h"
#include "Level3.h"
#include "LevelHelpers.h"

Level3::Level3(Vector2 origin, Texture2D tileset, Texture2D playerAtlas, Texture2D slimeAtlas,
	       Texture2D ghostAtlas, Texture2D chargerAtlas)
	: DungeonLevel(origin, "#1A1B2E", tileset, playerAtlas, slimeAtlas, ghostAtlas, chargerAtlas, 3)
{
}

void Level3::initialise()
{
	// last / hardest
	beginLevelInitialise();

	mLevelData.assign(kWidth * kHeight, TILE_WALL);
	levelGridFillRect(mLevelData, kWidth, 1, 1, kWidth - 2, kHeight - 2, TILE_FLOOR);

	// 3 mess blobs
	levelGridFillRect(mLevelData, kWidth, 4, 4, 10, 7, TILE_MESS);
	levelGridFillRect(mLevelData, kWidth, 14, 5, 19, 10, TILE_MESS);
	levelGridFillRect(mLevelData, kWidth, 6, 11, 14, 13, TILE_MESS);
	mLevelData[8 + 9 * kWidth] = TILE_MESS;
	mLevelData[16 + 12 * kWidth] = TILE_MESS;

	mLevelData[(kWidth - 2) + 4 * kWidth] = TILE_EXIT_LOCKED;
	mLevelData[5 + 6 * kWidth] = TILE_HEALTH;
	mLevelData[18 + 7 * kWidth] = TILE_HEALTH;
	mLevelData[12 + 2 * kWidth] = TILE_HEALTH;

	mState.map = new Map(
		kWidth, kHeight, &mLevelData[0], mTileset, 48.0f, kDungeonTilesetCols, kDungeonTilesetRows, mOrigin, false);
	mState.messTotalAtStart = (float)mState.map->countMessTiles();
	if (mState.messTotalAtStart < 1.0f) mState.messTotalAtStart = 1.0f;

	std::map<Direction, std::vector<int> > playerAnim = makeDefaultPlayerAnimMap();
	mState.player = new Entity(
		(Vector2){ mOrigin.x - 300.0f, mOrigin.y },
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

	for (int i = 0; i < 3; i++) {
		float ox = -140.0f + i * 130.0f;
		Entity *slime = new Entity(
			(Vector2){ mOrigin.x + ox, mOrigin.y - 120.0f + (float)(i * 20) },
			(Vector2){ 14.0f, 14.0f },
			ROLE_SLIME,
			mSlimeAtlas,
			ATLAS,
			(Vector2){ (float)kEnemyStripCols, (float)kEnemyStripRows },
			slimeAnim,
			105.0f);
		mState.enemies.push_back(slime);
	}

	Entity *charger = new Entity(
		(Vector2){ mOrigin.x + 200.0f, mOrigin.y + 160.0f },
		(Vector2){ 18.0f, 18.0f },
		ROLE_CHARGER,
		mChargerAtlas,
		ATLAS,
		(Vector2){ (float)kEnemyStripCols, (float)kEnemyStripRows },
		slimeAnim,
		155.0f);
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
		(Vector2){ mOrigin.x, mOrigin.y + 180.0f },
		(Vector2){ 22.0f, 22.0f },
		ROLE_GHOST,
		mGhostAtlas,
		ATLAS,
		(Vector2){ (float)kEnemyStripCols, (float)kEnemyStripRows },
		ghostAnim,
		100.0f);
	ghost->setPatrol(
		(Vector2){ mOrigin.x - 200.0f, mOrigin.y + 180.0f },
		(Vector2){ mOrigin.x + 200.0f, mOrigin.y + 180.0f });
	mState.enemies.push_back(ghost);

	Entity *ghostB = new Entity(
		(Vector2){ mOrigin.x - 50.0f, mOrigin.y - 140.0f },
		(Vector2){ 22.0f, 22.0f },
		ROLE_GHOST,
		mGhostAtlas,
		ATLAS,
		(Vector2){ (float)kEnemyStripCols, (float)kEnemyStripRows },
		ghostAnim,
		92.0f);
	ghostB->setPatrol(
		(Vector2){ mOrigin.x - 180.0f, mOrigin.y - 120.0f },
		(Vector2){ mOrigin.x + 120.0f, mOrigin.y - 150.0f });
	mState.enemies.push_back(ghostB);

	Entity *ghostC = new Entity(
		(Vector2){ mOrigin.x + 120.0f, mOrigin.y + 30.0f },
		(Vector2){ 22.0f, 22.0f },
		ROLE_GHOST,
		mGhostAtlas,
		ATLAS,
		(Vector2){ (float)kEnemyStripCols, (float)kEnemyStripRows },
		ghostAnim,
		96.0f);
	ghostC->setPatrol(
		(Vector2){ mOrigin.x + 100.0f, mOrigin.y - 90.0f },
		(Vector2){ mOrigin.x + 200.0f, mOrigin.y + 120.0f });
	mState.enemies.push_back(ghostC);

	loadDungeonLevelAudio(&mState, 3);
}

void Level3::render()
{
	renderDungeon();
}

void Level3::shutdown()
{
	freeLevelObjects();
	unloadAudio();
}
