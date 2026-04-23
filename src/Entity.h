#ifndef ENTITY_H
#define ENTITY_H

#include "Map.h"

enum Direction { DIR_LEFT, DIR_UP, DIR_RIGHT, DIR_DOWN };
enum DungeonRole { ROLE_PLAYER, ROLE_SLIME, ROLE_GHOST, ROLE_CHARGER };

class Entity
{
private:
	Vector2 mPosition;
	Vector2 mMovement;
	Vector2 mHalfSize;
	float mSpeed;
	int mHp;
	int mMaxHp;
	float mDamageCooldown;

	Texture2D mTexture;
	TextureType mTextureType;
	// sheet size in columns and rows see render for UVs
	Vector2 mSpriteSheetDims;
	std::map<Direction, std::vector<int> > mAnimAtlas;
	std::vector<int> mAnimIndices;
	Direction mDirection;
	int mFrameSpeed;
	int mCurrentFrame;
	float mAnimTime;

	DungeonRole mRole;

	float mAiTimer;
	Vector2 mPatrolA;
	Vector2 mPatrolB;
	bool mPatrolForward;
	int mChargePhase;
	float mChargeTimer;
	float mChargeCooldown;

	void animate(float deltaTime);
	void aiSlime(float dt, Map *map);
	void aiGhost(float dt, Map *map);
	void aiCharger(float dt, Map *map, Entity *player);
	void moveWithCollisions(float dt, Map *map);

public:
	static const int DEFAULT_PLAYER_SPEED = 220;
	static const int DEFAULT_ENEMY_SPEED = 110;
	static const int DEFAULT_FRAME_SPEED = 10;

	Entity();
	Entity(Vector2 position, Vector2 halfSize, DungeonRole role, Texture2D tex, TextureType ttype,
	       Vector2 sheetDims, std::map<Direction, std::vector<int> > animMap, float speed);
	~Entity();

	void update(float deltaTime, Map *map, Entity *player);
	void render();

	void resetMovement() { mMovement = (Vector2){ 0.0f, 0.0f }; }
	void moveFromKeyboard();

	void setPosition(Vector2 p) { mPosition = p; }
	Vector2 getPosition() const { return mPosition; }
	Vector2 getHalfSize() const { return mHalfSize; }
	DungeonRole getRole() const { return mRole; }
	int getHp() const { return mHp; }
	int getMaxHp() const { return mMaxHp; }
	bool isAlive() const { return mHp > 0; }

	void takeDamage(int amount);
	void heal(int amount);
	void setDamageCooldown(float seconds) { mDamageCooldown = seconds; }

	bool overlaps(const Entity *other) const;

	void setPatrol(Vector2 a, Vector2 b)
	{
		mPatrolA = a;
		mPatrolB = b;
	}
};

#endif
