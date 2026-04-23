#include "Entity.h"

Entity::Entity()
	: mPosition({ 0.0f, 0.0f }),
	  mMovement({ 0.0f, 0.0f }),
	  mHalfSize({ 16.0f, 16.0f }),
	  mSpeed(120.0f),
	  mHp(3),
	  mMaxHp(3),
	  mDamageCooldown(0.0f),
	  mTexture({ 0 }),
	  mTextureType(SINGLE),
	  mSpriteSheetDims({ 1.0f, 1.0f }),
	  mDirection(DIR_DOWN),
	  mFrameSpeed(DEFAULT_FRAME_SPEED),
	  mCurrentFrame(0),
	  mAnimTime(0.0f),
	  mRole(ROLE_SLIME),
	  mAiTimer(0.0f),
	  mPatrolA({ 0.0f, 0.0f }),
	  mPatrolB({ 0.0f, 0.0f }),
	  mPatrolForward(true),
	  mChargePhase(0),
	  mChargeTimer(0.0f),
	  mChargeCooldown(0.0f)
{
}

Entity::Entity(Vector2 position, Vector2 halfSize, DungeonRole role, Texture2D tex, TextureType ttype,
	       Vector2 sheetDims, std::map<Direction, std::vector<int> > animMap, float speed)
	: mPosition(position),
	  mMovement({ 0.0f, 0.0f }),
	  mHalfSize(halfSize),
	  mSpeed(speed),
	  mHp(role == ROLE_PLAYER ? 100 : 30),
	  mMaxHp(role == ROLE_PLAYER ? 100 : 30),
	  mDamageCooldown(0.0f),
	  mTexture(tex),
	  mTextureType(ttype),
	  mSpriteSheetDims(sheetDims),
	  mAnimAtlas(animMap),
	  mDirection(DIR_DOWN),
	  mFrameSpeed(DEFAULT_FRAME_SPEED),
	  mCurrentFrame(0),
	  mAnimTime(0.0f),
	  mRole(role),
	  mAiTimer(0.0f),
	  mPatrolA(position),
	  mPatrolB(position),
	  mPatrolForward(true),
	  mChargePhase(0),
	  mChargeTimer(0.0f),
	  mChargeCooldown(1.0f)
{
	if (mTextureType == ATLAS && !mAnimAtlas.empty()) {
		mAnimIndices = mAnimAtlas[DIR_DOWN];
	}
}

Entity::~Entity() {}

void Entity::animate(float deltaTime)
{
	if (mTextureType != ATLAS || mAnimIndices.empty()) return;

	mAnimTime += deltaTime;
	float frameLen = 1.0f / (float)mFrameSpeed;
	if (mAnimTime >= frameLen) {
		mAnimTime = 0.0f;
		mCurrentFrame++;
		mCurrentFrame %= (int)mAnimIndices.size();
	}
}

void Entity::moveWithCollisions(float dt, Map *map)
{
	Vector2 vel = { mMovement.x * mSpeed, mMovement.y * mSpeed };

	Vector2 cand = { mPosition.x + vel.x * dt, mPosition.y + vel.y * dt };

	Vector2 tryX = { cand.x, mPosition.y };
	if (!map->isWorldBlocked(tryX, mHalfSize))
		mPosition.x = cand.x;

	Vector2 tryY = { mPosition.x, cand.y };
	if (!map->isWorldBlocked(tryY, mHalfSize))
		mPosition.y = cand.y;
}

void Entity::aiSlime(float dt, Map *map)
{
	mAiTimer -= dt;
	if (mAiTimer > 0.0f) {
		moveWithCollisions(dt, map);
		return;
	}

	mMovement = (Vector2){ 0.0f, 0.0f };
	if (GetRandomValue(0, 99) < 14) {
		mAiTimer = 0.12f + (float)GetRandomValue(0, 55) / 100.0f;
	} else {
		int pick = GetRandomValue(0, 3);
		if (pick == 0) {
			mMovement.x = -1.0f;
			mDirection = DIR_LEFT;
		} else if (pick == 1) {
			mMovement.x = 1.0f;
			mDirection = DIR_RIGHT;
		} else if (pick == 2) {
			mMovement.y = -1.0f;
			mDirection = DIR_UP;
		} else {
			mMovement.y = 1.0f;
			mDirection = DIR_DOWN;
		}
		mAiTimer = 0.4f + (float)GetRandomValue(0, 150) / 100.0f;
	}

	if (!mAnimAtlas.empty()) mAnimIndices = mAnimAtlas[mDirection];
	moveWithCollisions(dt, map);
}

void Entity::aiGhost(float dt, Map *map)
{
	float minX = fminf(mPatrolA.x, mPatrolB.x) - 48.0f;
	float maxX = fmaxf(mPatrolA.x, mPatrolB.x) + 48.0f;
	float minY = fminf(mPatrolA.y, mPatrolB.y) - 48.0f;
	float maxY = fmaxf(mPatrolA.y, mPatrolB.y) + 48.0f;
	float cx = (mPatrolA.x + mPatrolB.x) * 0.5f;
	float cy = (mPatrolA.y + mPatrolB.y) * 0.5f;
	if (maxX - minX < 170.0f) {
		minX = cx - 155.0f;
		maxX = cx + 155.0f;
	}
	if (maxY - minY < 130.0f) {
		minY = cy - 115.0f;
		maxY = cy + 115.0f;
	}

	mAiTimer -= dt;
	if (mAiTimer > 0.0f) {
		const float edge = 28.0f;
		if (mPosition.x < minX + edge) mMovement.x = 1.0f;
		else if (mPosition.x > maxX - edge) mMovement.x = -1.0f;
		if (mPosition.y < minY + edge) mMovement.y = 1.0f;
		else if (mPosition.y > maxY - edge) mMovement.y = -1.0f;
		if (GetLength(mMovement) > 1.0f) Normalise(&mMovement);
		if (fabsf(mMovement.x) > fabsf(mMovement.y)) {
			mDirection = mMovement.x < 0.0f ? DIR_LEFT : DIR_RIGHT;
		} else if (fabsf(mMovement.y) > 1e-3f) {
			mDirection = mMovement.y < 0.0f ? DIR_UP : DIR_DOWN;
		}
		if (!mAnimAtlas.empty()) mAnimIndices = mAnimAtlas[mDirection];
		moveWithCollisions(dt, map);
		return;
	}

	mMovement = (Vector2){ 0.0f, 0.0f };
	if (GetRandomValue(0, 99) < 24) {
		mAiTimer = 0.18f + (float)GetRandomValue(0, 85) / 100.0f;
	} else {
		int pick = GetRandomValue(0, 3);
		if (mPosition.x < minX + 40.0f) pick = 1;
		else if (mPosition.x > maxX - 40.0f) pick = 0;
		if (mPosition.y < minY + 36.0f && GetRandomValue(0, 1) == 0) pick = 3;
		else if (mPosition.y > maxY - 36.0f && GetRandomValue(0, 1) == 0) pick = 2;

		if (pick == 0) {
			mMovement.x = -1.0f;
			mDirection = DIR_LEFT;
		} else if (pick == 1) {
			mMovement.x = 1.0f;
			mDirection = DIR_RIGHT;
		} else if (pick == 2) {
			mMovement.y = -1.0f;
			mDirection = DIR_UP;
		} else {
			mMovement.y = 1.0f;
			mDirection = DIR_DOWN;
		}
		mAiTimer = 0.32f + (float)GetRandomValue(0, 165) / 100.0f;
	}

	if (!mAnimAtlas.empty()) mAnimIndices = mAnimAtlas[mDirection];
	moveWithCollisions(dt, map);
}

void Entity::aiCharger(float dt, Map *map, Entity *player)
{
	mChargeCooldown -= dt;

	if (mChargePhase == 0) {
		bool startedCharge = false;
		if (player && mChargeCooldown <= 0.0f) {
			float d = Vector2Distance(mPosition, player->getPosition());
			if (d < 240.0f && d > 32.0f) {
				Vector2 to = Vector2Subtract(player->getPosition(), mPosition);
				Normalise(&to);
				mMovement = to;
				mChargePhase = 1;
				mChargeTimer = 0.55f;
				if (fabsf(to.x) > fabsf(to.y))
					mDirection = to.x < 0 ? DIR_LEFT : DIR_RIGHT;
				else
					mDirection = to.y < 0 ? DIR_UP : DIR_DOWN;
				if (!mAnimAtlas.empty()) mAnimIndices = mAnimAtlas[mDirection];
				startedCharge = true;
			}
		}
		if (!startedCharge) {
			mAiTimer -= dt;
			if (mAiTimer <= 0.0f) {
				if (GetRandomValue(0, 99) < 35) {
					mMovement = (Vector2){ 0.0f, 0.0f };
					mAiTimer = 0.15f + (float)GetRandomValue(0, 70) / 100.0f;
				} else {
					int pick = GetRandomValue(0, 3);
					mMovement = (Vector2){ 0.0f, 0.0f };
					if (pick == 0) {
						mMovement.x = -1.0f;
						mDirection = DIR_LEFT;
					} else if (pick == 1) {
						mMovement.x = 1.0f;
						mDirection = DIR_RIGHT;
					} else if (pick == 2) {
						mMovement.y = -1.0f;
						mDirection = DIR_UP;
					} else {
						mMovement.y = 1.0f;
						mDirection = DIR_DOWN;
					}
					mAiTimer = 0.28f + (float)GetRandomValue(0, 95) / 100.0f;
				}
				if (!mAnimAtlas.empty()) mAnimIndices = mAnimAtlas[mDirection];
			}
		}
		moveWithCollisions(dt, map);
	} else {
		mChargeTimer -= dt;
		moveWithCollisions(dt, map);
		if (mChargeTimer <= 0.0f) {
			mChargePhase = 0;
			mChargeCooldown = 2.0f;
			mMovement = (Vector2){ 0.0f, 0.0f };
			mAiTimer = 0.25f + (float)GetRandomValue(0, 80) / 100.0f;
		}
	}
}

void Entity::update(float deltaTime, Map *map, Entity *player)
{
	if (mDamageCooldown > 0.0f) mDamageCooldown -= deltaTime;

	if (mRole == ROLE_PLAYER) {
		if (GetLength(mMovement) > 1.0f) Normalise(&mMovement);
		if (fabsf(mMovement.x) > fabsf(mMovement.y)) {
			if (mMovement.x < 0)
				mDirection = DIR_LEFT;
			else if (mMovement.x > 0)
				mDirection = DIR_RIGHT;
		} else if (fabsf(mMovement.y) > 1e-3f) {
			if (mMovement.y < 0)
				mDirection = DIR_UP;
			else
				mDirection = DIR_DOWN;
		}
		if (!mAnimAtlas.empty()) mAnimIndices = mAnimAtlas[mDirection];
		moveWithCollisions(deltaTime, map);
		if (GetLength(mMovement) > 0.05f) animate(deltaTime);
	} else if (mRole == ROLE_SLIME) {
		aiSlime(deltaTime, map);
		if (GetLength(mMovement) > 0.05f) animate(deltaTime);
	} else if (mRole == ROLE_GHOST) {
		aiGhost(deltaTime, map);
		animate(deltaTime);
	} else if (mRole == ROLE_CHARGER) {
		aiCharger(deltaTime, map, player);
		if (GetLength(mMovement) > 0.05f) animate(deltaTime);
	}
}

void Entity::render()
{
	if (mTexture.id == 0) return;

	Rectangle textureArea;
	if (mTextureType == SINGLE) {
		textureArea = (Rectangle){ 0.0f, 0.0f, (float)mTexture.width, (float)mTexture.height };
	} else {
		if (mAnimIndices.empty()) return;
		int idx = mAnimIndices[mCurrentFrame];
		textureArea = getUVRectangle(
			&mTexture, idx, (int)mSpriteSheetDims.y, (int)mSpriteSheetDims.x);
	}

	float drawW = mHalfSize.x * 2.0f;
	float drawH = mHalfSize.y * 2.0f;
	// draw position matches collider centre keep them in sync
	Rectangle dest = { mPosition.x - drawW * 0.5f, mPosition.y - drawH * 0.5f, drawW, drawH };
	Vector2 origin = { 0.0f, 0.0f };
	DrawTexturePro(mTexture, textureArea, dest, origin, 0.0f, WHITE);
}

void Entity::moveFromKeyboard()
{
	resetMovement();
	if (IsKeyDown(KEY_A)) mMovement.x = -1.0f;
	if (IsKeyDown(KEY_D)) mMovement.x = 1.0f;
	if (IsKeyDown(KEY_W)) mMovement.y = -1.0f;
	if (IsKeyDown(KEY_S)) mMovement.y = 1.0f;
}

void Entity::takeDamage(int amount)
{
	if (mDamageCooldown > 0.0f) return;
	mHp -= amount;
	if (mHp < 0) mHp = 0;
	mDamageCooldown = 0.75f;
}

void Entity::heal(int amount)
{
	if (amount <= 0 || mRole != ROLE_PLAYER) return;
	mHp += amount;
	if (mHp > mMaxHp) mHp = mMaxHp;
}

bool Entity::overlaps(const Entity *other) const
{
	float dx = fabsf(mPosition.x - other->mPosition.x) - (mHalfSize.x + other->mHalfSize.x);
	float dy = fabsf(mPosition.y - other->mPosition.y) - (mHalfSize.y + other->mHalfSize.y);
	return dx < 0.0f && dy < 0.0f;
}
