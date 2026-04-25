#ifndef LEVEL_HELPERS_H
#define LEVEL_HELPERS_H

// level helpers

#include "Scene.h"

#include <cstdio>
#include <map>
#include <vector>

// fill a rect on the tile grid
inline void levelGridFillRect(std::vector<unsigned int> &g, int W, int c0, int r0, int c1, int r1, unsigned int t)
{
	for (int r = r0; r <= r1; r++) {
		for (int c = c0; c <= c1; c++) {
			g[r * W + c] = t;
		}
	}
}

// default 2 frames per direction (shared across levels)
inline std::map<Direction, std::vector<int> > makeDefaultPlayerAnimMap()
{
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
	return playerAnim;
}

// bgm per level; sfx are global
inline void loadDungeonLevelAudio(GameState *state, int levelNumber)
{
	char bgmPath[64];
	std::snprintf(bgmPath, sizeof(bgmPath), "assets/audio/level%d.ogg", levelNumber);
	if (FileExists(bgmPath)) {
		state->bgm = LoadMusicStream(bgmPath);
		state->hasBgm = true;
		PlayMusicStream(state->bgm);
	}
	if (FileExists("assets/audio/clean.wav")) {
		state->sfxClean = LoadSound("assets/audio/clean.wav");
		state->hasSfxClean = true;
	}
	if (FileExists("assets/audio/hurt.wav")) {
		state->sfxHurt = LoadSound("assets/audio/hurt.wav");
		state->hasSfxHurt = true;
	}
	if (FileExists("assets/audio/exit.wav")) {
		state->sfxExit = LoadSound("assets/audio/exit.wav");
		state->hasSfxExit = true;
	}
}

#endif
