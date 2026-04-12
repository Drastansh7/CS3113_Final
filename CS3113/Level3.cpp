#include "GameAssets.h"
#include "Level3.h"

static void fillRect3(std::vector<unsigned int> &g, int W, int c0, int r0, int c1, int r1, unsigned int t)
{
	for (int r = r0; r <= r1; r++) {
		for (int c = c0; c <= c1; c++) {
			g[r * W + c] = t;
		}
	}
}

Level3::Level3(Vector2 origin, Texture2D tileset, Texture2D playerAtlas, Texture2D slimeAtlas,
	       Texture2D ghostAtlas, Texture2D chargerAtlas)
	: DungeonLevel(origin, "#1A1B2E", tileset, playerAtlas, slimeAtlas, ghostAtlas, chargerAtlas, 3)
{
}

void Level3::initialise()
{
	unloadAudio();
	mState.nextSceneId = 0;
	mState.playerDead = false;
	mState.victory = false;
	mState.exitLatched = false;
	mState.cleanCooldown = 0.0f;

	mLevelData.assign(kWidth * kHeight, TILE_WALL);
	fillRect3(mLevelData, kWidth, 1, 1, kWidth - 2, kHeight - 2, TILE_FLOOR);

	fillRect3(mLevelData, kWidth, 4, 4, 10, 7, TILE_MESS);
	fillRect3(mLevelData, kWidth, 14, 5, 19, 10, TILE_MESS);
	fillRect3(mLevelData, kWidth, 6, 11, 14, 13, TILE_MESS);
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

	if (FileExists("assets/audio/level3.ogg")) {
		mState.bgm = LoadMusicStream("assets/audio/level3.ogg");
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

void Level3::render()
{
	ClearBackground(ColorFromHex(mBgHex));
	if (mState.map) mState.map->render();
	if (mState.player) mState.player->render();
	for (size_t i = 0; i < mState.enemies.size(); i++) {
		if (mState.enemies[i] && mState.enemies[i]->isAlive()) mState.enemies[i]->render();
	}
}

void Level3::shutdown()
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
