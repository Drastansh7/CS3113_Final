#ifndef SCENE_H
#define SCENE_H

#include "Entity.h"

struct GameState
{
	Entity *player;
	std::vector<Entity *> enemies;
	Map *map;
	Music bgm;
	Sound sfxClean;
	Sound sfxHurt;
	Sound sfxExit;
	int nextSceneId;
	bool playerDead;
	bool victory;
	float cleanCooldown;
	float messTotalAtStart;
	bool hasBgm;
	bool hasSfxClean;
	bool hasSfxHurt;
	bool hasSfxExit;
	bool exitLatched;
};

class Scene
{
protected:
	GameState mState;
	Vector2 mOrigin;
	const char *mBgHex;
	Texture2D mTileset;
	Texture2D mPlayerAtlas;
	Texture2D mSlimeAtlas;
	Texture2D mGhostAtlas;
	Texture2D mChargerAtlas;

	void unloadAudio();

public:
	Scene(Vector2 origin, const char *bgHex, Texture2D tileset, Texture2D playerAtlas, Texture2D slimeAtlas,
	      Texture2D ghostAtlas, Texture2D chargerAtlas);
	virtual ~Scene();

	virtual void initialise() = 0;
	virtual void update(float deltaTime) = 0;
	virtual void render() = 0;
	virtual void shutdown() = 0;

	GameState &getState() { return mState; }
	Vector2 getOrigin() const { return mOrigin; }
};

class DungeonLevel : public Scene
{
protected:
	int mNextSceneOnExit;

	// reset level state
	void beginLevelInitialise();
	// free map / player / enemies. audio is caller’s job
	void freeLevelObjects();
	void renderDungeon();

public:
	DungeonLevel(Vector2 origin, const char *bgHex, Texture2D tileset, Texture2D playerAtlas,
		     Texture2D slimeAtlas, Texture2D ghostAtlas, Texture2D chargerAtlas, int nextSceneOnExit);
	void update(float deltaTime) override;
};

#endif
