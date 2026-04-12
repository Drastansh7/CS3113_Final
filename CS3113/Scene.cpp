#include "Scene.h"

Scene::Scene(Vector2 origin, const char *bgHex, Texture2D tileset, Texture2D playerAtlas, Texture2D slimeAtlas,
	     Texture2D ghostAtlas, Texture2D chargerAtlas)
	: mOrigin(origin),
	  mBgHex(bgHex),
	  mTileset(tileset),
	  mPlayerAtlas(playerAtlas),
	  mSlimeAtlas(slimeAtlas),
	  mGhostAtlas(ghostAtlas),
	  mChargerAtlas(chargerAtlas)
{
	mState.player = nullptr;
	mState.map = nullptr;
	mState.bgm = (Music){ 0 };
	mState.sfxClean = (Sound){ 0 };
	mState.sfxHurt = (Sound){ 0 };
	mState.sfxExit = (Sound){ 0 };
	mState.nextSceneId = 0;
	mState.playerDead = false;
	mState.victory = false;
	mState.cleanCooldown = 0.0f;
	mState.messTotalAtStart = 1.0f;
	mState.hasBgm = false;
	mState.hasSfxClean = false;
	mState.hasSfxHurt = false;
	mState.hasSfxExit = false;
	mState.exitLatched = false;
}

Scene::~Scene() {}

void Scene::unloadAudio()
{
	if (mState.hasBgm) {
		UnloadMusicStream(mState.bgm);
		mState.bgm = (Music){ 0 };
		mState.hasBgm = false;
	}
	if (mState.hasSfxClean) {
		UnloadSound(mState.sfxClean);
		mState.sfxClean = (Sound){ 0 };
		mState.hasSfxClean = false;
	}
	if (mState.hasSfxHurt) {
		UnloadSound(mState.sfxHurt);
		mState.sfxHurt = (Sound){ 0 };
		mState.hasSfxHurt = false;
	}
	if (mState.hasSfxExit) {
		UnloadSound(mState.sfxExit);
		mState.sfxExit = (Sound){ 0 };
		mState.hasSfxExit = false;
	}
}
