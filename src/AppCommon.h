#ifndef APP_COMMON_H
#define APP_COMMON_H

#define LOG(argument) std::cout << argument << '\n'

#include "raylib.h"
#include "raymath.h"
#include <cmath>
#include <cstdio>
#include <iostream>
#include <map>
#include <string>
#include <vector>

enum AppStatus { TERMINATED, RUNNING };
enum TextureType { SINGLE, ATLAS };

Color ColorFromHex(const char *hex);
void Normalise(Vector2 *vector);
float GetLength(const Vector2 vector);
Rectangle getUVRectangle(const Texture2D *texture, int index, int rows, int cols);
void panCamera(Camera2D *camera, const Vector2 *targetPosition);

Texture2D GenerateTilesetTexture(void);
Texture2D GenerateJanitorAtlas(void);
Texture2D GenerateSlimeAtlas(void);
Texture2D GenerateGhostAtlas(void);
Texture2D GenerateChargerAtlas(void);

#endif /* APP_COMMON_H */
