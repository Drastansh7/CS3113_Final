/*
 * Dungeon Cleanup Crew CS3113 Drastansh Nadola
 * WASD move SPACE mop the floor Q quit
 * Clear every mess tile then walk out the exit
 */

#include "levels/Level1.h"
#include "levels/Level2.h"
#include "levels/Level3.h"
#include "Scene.h"
#include "GameAssets.h"
#include "ShaderProgram.h"
#include "AppCommon.h"
#include <vector>

constexpr int SCREEN_WIDTH = 1000;
constexpr int SCREEN_HEIGHT = 600;
constexpr int FPS = 120;
constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;

static AppStatus gAppStatus = RUNNING;
static float gPreviousTicks = 0.0f;
static float gTimeAccumulator = 0.0f;

static Camera2D gCamera = { 0 };
static Scene *gCurrentScene = nullptr;
static std::vector<Scene *> gLevels;
static int gLevelIndex = 0;

static Level1 *gLevel1 = nullptr;
static Level2 *gLevel2 = nullptr;
static Level3 *gLevel3 = nullptr;

static GameTextures gTex = {};

static ShaderProgram gShader;
static bool gShaderOk = false;

static bool gWin = false;
static bool gLose = false;

static void switchToScene(Scene *scene)
{
	if (gCurrentScene) gCurrentScene->shutdown();
	gCurrentScene = scene;
	if (gCurrentScene) gCurrentScene->initialise();
	if (gCurrentScene && gCurrentScene->getState().player) {
		gCamera.target = gCurrentScene->getState().player->getPosition();
	}
}

static void restartFromBeginning(void)
{
	gWin = false;
	gLose = false;
	gLevelIndex = 0;
	switchToScene(gLevels[0]);
	gCamera.offset = (Vector2){ SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };
	gCamera.rotation = 0.0f;
	gCamera.zoom = 1.0f;
}

static void restartCurrentLevel(void)
{
	if (!gCurrentScene || gLevelIndex < 0 || gLevelIndex >= (int)gLevels.size()) return;
	gLose = false;
	Scene *s = gLevels[gLevelIndex];
	switchToScene(s);
}

static void initialise(void)
{
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Dungeon Cleanup Crew, Drastansh Nadola");
	InitAudioDevice();
	SetTargetFPS(FPS);

	LoadGameTextures(&gTex);

	Vector2 origin = { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };

	gLevel1 = new Level1(origin, gTex.tileset, gTex.player, gTex.slime, gTex.ghost, gTex.charger);
	gLevel2 = new Level2(origin, gTex.tileset, gTex.player, gTex.slime, gTex.ghost, gTex.charger);
	gLevel3 = new Level3(origin, gTex.tileset, gTex.player, gTex.slime, gTex.ghost, gTex.charger);

	gLevels.push_back(gLevel1);
	gLevels.push_back(gLevel2);
	gLevels.push_back(gLevel3);

	restartFromBeginning();

	gShaderOk = gShader.load("", "shaders/curse_fragment.glsl");

	gCamera.offset = (Vector2){ SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };
	gCamera.rotation = 0.0f;
	gCamera.zoom = 1.0f;
}

static void shutdown(void)
{
	if (gCurrentScene) {
		gCurrentScene->shutdown();
		gCurrentScene = nullptr;
	}

	delete gLevel1;
	delete gLevel2;
	delete gLevel3;
	gLevel1 = nullptr;
	gLevel2 = nullptr;
	gLevel3 = nullptr;
	gLevels.clear();

	gShader.unload();

	UnloadGameTextures(&gTex);

	CloseAudioDevice();
	CloseWindow();
}

static void processInput(void)
{
	if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;

	if (gWin && IsKeyPressed(KEY_R)) restartFromBeginning();

	if (gLose && IsKeyPressed(KEY_R)) restartCurrentLevel();
}

static void update(void)
{
	if (gWin) return;

	float ticks = (float)GetTime();
	float deltaTime = ticks - gPreviousTicks;
	gPreviousTicks = ticks;

	deltaTime += gTimeAccumulator;
	if (deltaTime < FIXED_TIMESTEP) {
		gTimeAccumulator = deltaTime;
		return;
	}

	while (deltaTime >= FIXED_TIMESTEP) {
		if (gCurrentScene && !gLose) {
			gCurrentScene->update(FIXED_TIMESTEP);

			GameState &st = gCurrentScene->getState();
			const int pending = st.nextSceneId;

			if (pending == 3) {
				gWin = true;
				gCurrentScene->shutdown();
				gCurrentScene = nullptr;
				deltaTime -= FIXED_TIMESTEP;
				break;
			}

			if (pending >= 1 && pending <= 2) {
				gLevelIndex = pending;
				st.nextSceneId = 0;
				switchToScene(gLevels[(size_t)gLevelIndex]);
			}

			if (gCurrentScene && gCurrentScene->getState().playerDead) gLose = true;

			if (gCurrentScene && gCurrentScene->getState().player) {
				Vector2 camFollow = { gCurrentScene->getState().player->getPosition().x,
						      SCREEN_HEIGHT / 2.0f };
				panCamera(&gCamera, &camFollow);
			}
		}

		deltaTime -= FIXED_TIMESTEP;
	}
	gTimeAccumulator = deltaTime;
}

static void render(void)
{
	BeginDrawing();

	if (gWin) {
		ClearBackground((Color){ 24, 26, 38, 255 });
		DrawText("Dungeon clear you win", 120, 220, 28, RAYWHITE);
		DrawText("R restarts from level one  Q quits", 120, 280, 22, LIGHTGRAY);
		EndDrawing();
		return;
	}

	if (!gCurrentScene) {
		ClearBackground(BLACK);
		EndDrawing();
		return;
	}

	BeginMode2D(gCamera);

	float curse = 0.0f;
	if (gCurrentScene->getState().map) curse = gCurrentScene->getState().map->curseRatio();

	if (gShaderOk) {
		gShader.begin();
		gShader.setFloat("curseStrength", curse);
		gCurrentScene->render();
		gShader.end();
	} else {
		gCurrentScene->render();
	}

	EndMode2D();

	GameState &st = gCurrentScene->getState();
	if (st.player && st.map) {
		int mess = st.map->countMessTiles();
		DrawText(TextFormat("HP: %i", st.player->getHp()), 16, 12, 22, RAYWHITE);
		DrawText(TextFormat("Mess left: %i", mess), 16, 38, 22, LIGHTGRAY);
		DrawText("WASD move SPACE scrub open exit when no mess left", 16, 64, 18, (Color){ 200, 200, 200, 220 });
	}

	if (gLose) {
		DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){ 0, 0, 0, 160 });
		DrawText("You died press R to retry this floor", 180, SCREEN_HEIGHT / 2 - 20, 28, RAYWHITE);
	}

	EndDrawing();
}

int main(void)
{
	initialise();

	while (gAppStatus == RUNNING) {
		processInput();
		update();
		render();
	}

	shutdown();
	return 0;
}
