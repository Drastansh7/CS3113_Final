#include "Scene.h"

DungeonLevel::DungeonLevel(Vector2 origin, const char *bgHex, Texture2D tileset, Texture2D playerAtlas,
			   Texture2D slimeAtlas, Texture2D ghostAtlas, Texture2D chargerAtlas, int nextSceneOnExit)
	: Scene(origin, bgHex, tileset, playerAtlas, slimeAtlas, ghostAtlas, chargerAtlas),
	  mNextSceneOnExit(nextSceneOnExit)
{
}

void DungeonLevel::beginLevelInitialise()
{
	unloadAudio();
	mState.nextSceneId = 0;
	mState.playerDead = false;
	mState.victory = false;
	mState.exitLatched = false;
	mState.cleanCooldown = 0.0f;
}

void DungeonLevel::freeLevelObjects()
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
}

void DungeonLevel::renderDungeon()
{
	ClearBackground(ColorFromHex(mBgHex));
	if (mState.map) mState.map->render();
	if (mState.player) mState.player->render();
	for (size_t i = 0; i < mState.enemies.size(); i++) {
		if (mState.enemies[i] && mState.enemies[i]->isAlive()) mState.enemies[i]->render();
	}
}

void DungeonLevel::update(float deltaTime)
{
	if (mState.victory || mState.playerDead) return;

	if (mState.hasBgm) UpdateMusicStream(mState.bgm);

	if (mState.player) mState.player->moveFromKeyboard();

	mState.cleanCooldown -= deltaTime;
	if (IsKeyPressed(KEY_SPACE) && mState.cleanCooldown <= 0.0f && mState.player && mState.map) {
		mState.cleanCooldown = 0.32f;
		if (mState.map->tryCleanMessNear(mState.player->getPosition(), 52.0f)) {
			if (mState.hasSfxClean) PlaySound(mState.sfxClean);
		}
	}

	if (mState.map) mState.map->unlockExitsIfClear();

	if (mState.player && mState.map) {
		int packs = mState.map->tryCollectHealthNear(mState.player->getPosition(), 30.0f);
		if (packs > 0) mState.player->heal(28 * packs);
	}

	if (mState.player) mState.player->update(deltaTime, mState.map, mState.player);

	for (size_t i = 0; i < mState.enemies.size(); i++) {
		Entity *e = mState.enemies[i];
		if (!e || !e->isAlive()) continue;
		e->update(deltaTime, mState.map, mState.player);
	}

	if (mState.player && mState.player->isAlive()) {
		for (size_t i = 0; i < mState.enemies.size(); i++) {
			Entity *e = mState.enemies[i];
			if (!e || !e->isAlive()) continue;
			if (mState.player->overlaps(e)) {
				mState.player->takeDamage(12);
				if (mState.hasSfxHurt) PlaySound(mState.sfxHurt);
			}
		}
	}

	if (mState.player && mState.player->getHp() <= 0) {
		mState.playerDead = true;
	}

	if (mState.player && mState.map && !mState.exitLatched &&
	    mState.map->isOnOpenExit(mState.player->getPosition(), mState.player->getHalfSize())) {
		mState.nextSceneId = mNextSceneOnExit;
		mState.exitLatched = true;
		if (mState.hasSfxExit) PlaySound(mState.sfxExit);
	}
}
