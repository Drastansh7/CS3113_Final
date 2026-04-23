#include "GameAssets.h"
#include "Level2.h"

static void fillRect2(std::vector<unsigned int> &g, int W, int c0, int r0, int c1, int r1, unsigned int t)
{
	for (int r = r0; r <= r1; r++) {
		for (int c = c0; c <= c1; c++) {
			g[r * W + c] = t;
		}
	}
}

Level2::Level2(Vector2 origin, Texture2D tileset, Texture2D playerAtlas, Texture2D slimeAtlas,
	       Texture2D ghostAtlas, Texture2D chargerAtlas)
	: DungeonLevel(origin, "#3D405B", tileset, playerAtlas, slimeAtlas, ghostAtlas, chargerAtlas, 2)
{
}

void Level2::initialise()
{
	unloadAudio();
	mState.nextSceneId = 0;
	mState.playerDead = false;
	mState.victory = false;
	mState.exitLatched = false;
	mState.cleanCooldown = 0.0f;

	mLevelData.assign(kWidth * kHeight, TILE_WALL);
	fillRect2(mLevelData, kWidth, 1, 1, kWidth - 2, kHeight - 2, TILE_FLOOR);

	fillRect2(mLevelData, kWidth, 3, 3, 7, 5, TILE_MESS);
	fillRect2(mLevelData, kWidth, 12, 6, 16, 9, TILE_MESS);
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

	std::map<Direction, std::vector<int> > playerAnim;
	playerAnim[DIR_DOWN] = std::vector<int>(2);
	playerAnim[DIR_DOWN][0] = 0;
	playerAnim[DIR_DOWN][1] = 1;
	playerAnim[DIR_UP] = std::vector<int>(2);
	playerAnim[DIR_UP][0] = 2;
	playerAnim[DIR_UP][1] = 3;
	playerAnim[DIR_LEFT] = std::vector<int>(2);
	playerAnim[DIR_LEFT][0] = 4;
	playerAnim[DIR_LEFT][1] = 5;
	playerAnim[DIR_RIGHT] = std::vector<int>(2);
	playerAnim[DIR_RIGHT][0] = 6;
	playerAnim[DIR_RIGHT][1] = 7;

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

	if (FileExists("assets/audio/level2.ogg")) {
		mState.bgm = LoadMusicStream("assets/audio/level2.ogg");
		mState.hasBgm = true;
		PlayMusicStream(mState.bgm);
	}
	if (FileExists("assets/audio/clean.wav")) {
		mState.sfxClean = LoadSound("assets/audio/clean.wav");
		mState.hasSfxClean = true;
	}
	if (FileExists("assets/audio/hurt.wav")) {
		mState.sfxHurt = LoadSound("assets/audio/hurt.wav");
		mState.hasSfxHurt = true;
	}
	if (FileExists("assets/audio/exit.wav")) {
		mState.sfxExit = LoadSound("assets/audio/exit.wav");
		mState.hasSfxExit = true;
	}
}

void Level2::render()
{
	ClearBackground(ColorFromHex(mBgHex));
	if (mState.map) mState.map->render();
	if (mState.player) mState.player->render();
	for (size_t i = 0; i < mState.enemies.size(); i++) {
		if (mState.enemies[i] && mState.enemies[i]->isAlive()) mState.enemies[i]->render();
	}
}

void Level2::shutdown()
{
	delete mState.player;
	mState.player = nullptr;
	for (size_t i = 0; i < mState.enemies.size(); i++) {
		delete mState.enemies[i];
		mState.enemies[i] = nullptr;
	}
	mState.enemies.clear();
	delete mState.map;
	mState.map = nullptr;
	unloadAudio();
}
