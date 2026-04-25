#include "AppCommon.h"

#include <cmath>

Color ColorFromHex(const char *hex)
{
	if (hex[0] == '#') hex++;

	unsigned int r = 0, g = 0, b = 0, a = 255;

	if (sscanf(hex, "%02x%02x%02x", &r, &g, &b) == 3) {
		return (Color){ (unsigned char)r, (unsigned char)g, (unsigned char)b, (unsigned char)a };
	}

	if (sscanf(hex, "%02x%02x%02x%02x", &r, &g, &b, &a) == 4) {
		return (Color){ (unsigned char)r, (unsigned char)g, (unsigned char)b, (unsigned char)a };
	}

	return RAYWHITE;
}

float GetLength(const Vector2 vector)
{
	return sqrtf(vector.x * vector.x + vector.y * vector.y);
}

void Normalise(Vector2 *vector)
{
	float magnitude = GetLength(*vector);
	if (magnitude < 1e-6f) return;
	vector->x /= magnitude;
	vector->y /= magnitude;
}

Rectangle getUVRectangle(const Texture2D *texture, int index, int rows, int cols)
{
	float uCoord = (float)(index % cols) / (float)cols * (float)texture->width;
	float vCoord = (float)(index / cols) / (float)rows * (float)texture->height;
	float sliceWidth = (float)texture->width / (float)cols;
	float sliceHeight = (float)texture->height / (float)rows;

	return (Rectangle){ uCoord, vCoord, sliceWidth, sliceHeight };
}

void panCamera(Camera2D *camera, const Vector2 *targetPosition)
{
	Vector2 positionDifference = Vector2Subtract(*targetPosition, camera->target);
	camera->target = Vector2Add(camera->target, Vector2Scale(positionDifference, 0.12f));
}
