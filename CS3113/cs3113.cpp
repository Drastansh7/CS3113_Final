#include "cs3113.h"

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

static void imageFillOval(Image *dst, int cx, int cy, int rx, int ry, Color c)
{
	if (rx < 1) rx = 1;
	if (ry < 1) ry = 1;
	for (int y = -ry; y <= ry; y++) {
		float t = (float)y / (float)ry;
		float inside = 1.0f - t * t;
		if (inside < 0.0f) inside = 0.0f;
		float w = (float)rx * sqrtf(inside);
		int half = (int)(w + 0.5f);
		if (half < 1) half = 1;
		ImageDrawRectangle(dst, cx - half, cy + y, half * 2, 1, c);
	}
}

static void drawBrickWall(Image *img, int x, int y, int w, int h)
{
	Color stone = (Color){ 48, 46, 58, 255 };
	Color mortar = (Color){ 28, 26, 34, 255 };
	Color hi = (Color){ 62, 60, 74, 255 };
	ImageDrawRectangle(img, x, y, w, h, stone);
	int rowH = h / 4;
	for (int r = 0; r < 4; r++) {
		int yo = y + r * rowH;
		int offset = (r % 2) * (w / 6);
		for (int c = -1; c < 6; c++) {
			int brickW = w / 3;
			int xo = x + offset + c * brickW;
			Rectangle br = { (float)xo, (float)yo, (float)(brickW - 2), (float)(rowH - 2) };
			ImageDrawRectangle(img, (int)br.x + 1, (int)br.y + 1, (int)br.width - 1, (int)br.height - 1, hi);
			ImageDrawRectangleLines(img, br, 1, mortar);
		}
	}
}

static void drawStoneFloor(Image *img, int x, int y, int w, int h)
{
	Color base = (Color){ 58, 56, 68, 255 };
	Color crack = (Color){ 44, 42, 54, 255 };
	Color spot = (Color){ 70, 68, 82, 255 };
	ImageDrawRectangle(img, x, y, w, h, base);
	for (int i = 0; i < 5; i++) {
		int px = x + 3 + (i * 17) % (w - 8);
		int py = y + 5 + (i * 11) % (h - 8);
		ImageDrawRectangle(img, px, py, 4, 3, crack);
	}
	ImageDrawRectangle(img, x + w / 3, y + h / 2, 5, 4, spot);
	ImageDrawRectangle(img, x + w / 2, y + h / 4, 3, 5, crack);
}

static void drawSlimePuddle(Image *img, int x, int y, int w, int h)
{
	Color deep = (Color){ 28, 110, 72, 255 };
	Color mid = (Color){ 42, 170, 98, 255 };
	Color hi = (Color){ 120, 230, 160, 255 };
	int mx = x + w / 2;
	int my = y + h / 2 + 2;
	imageFillOval(img, mx, my, w / 2 - 4, h / 2 - 3, deep);
	imageFillOval(img, mx, my - 1, w / 2 - 6, h / 2 - 5, mid);
	ImageDrawCircle(img, x + w / 2 - 4, y + h / 2 - 2, 4, hi);
	ImageDrawCircle(img, x + w / 2 + 6, y + h / 2 + 3, 2, hi);
}

static void drawLockedDoor(Image *img, int x, int y, int w, int h)
{
	Color wood = (Color){ 86, 52, 42, 255 };
	Color iron = (Color){ 55, 58, 70, 255 };
	Color glow = (Color){ 140, 70, 160, 255 };
	ImageDrawRectangle(img, x + 4, y + 4, w - 8, h - 8, wood);
	ImageDrawRectangle(img, x + w / 2 - 3, y + 8, 6, h - 16, iron);
	ImageDrawCircle(img, x + w / 2, y + h / 2 + 4, 5, iron);
	ImageDrawRectangleLines(
		img, (Rectangle){ (float)x + 3, (float)y + 3, (float)w - 6, (float)h - 6 }, 2, glow);
}

static void drawOpenDoor(Image *img, int x, int y, int w, int h)
{
	Color light = (Color){ 240, 220, 140, 255 };
	Color frame = (Color){ 70, 66, 52, 255 };
	ImageDrawRectangle(img, x + 2, y + 2, w - 4, h - 4, (Color){ 36, 34, 40, 255 });
	ImageDrawRectangle(img, x + 6, y + 4, w - 12, h - 8, light);
	ImageDrawRectangleLines(
		img, (Rectangle){ (float)x + 2, (float)y + 2, (float)w - 4, (float)h - 4 }, 2, frame);
	ImageDrawRectangle(img, x + 4, y + 4, 5, h - 8, frame);
}

// Filled heart via the usual implicit curve (+y = up in math space).
static bool heartImplicitInside(float x, float y)
{
	float x2 = x * x;
	float y2 = y * y;
	float v = x2 + y2 - 1.0f;
	return (v * v * v - x2 * y2 * y) <= 0.0f;
}

static void drawHealthPickup(Image *img, int x, int y, int w, int h)
{
	drawStoneFloor(img, x, y, w, h);
	int cx = x + w / 2;
	int cy = y + h / 2 + 1;
	const float scale = 11.8f;
	Color fill = (Color){ 228, 58, 88, 255 };
	Color hi = (Color){ 255, 150, 168, 255 };
	Color shade = (Color){ 180, 35, 70, 255 };
	Color edge = (Color){ 110, 18, 42, 255 };

	int x0 = x + 3;
	int y0 = y + 3;
	int x1 = x + w - 3;
	int y1 = y + h - 3;

	for (int py = y0; py < y1; py++) {
		for (int px = x0; px < x1; px++) {
			float mx = (float)(px - cx) / scale;
			float my = -(float)(py - cy) / scale;
			if (!heartImplicitInside(mx, my)) continue;
			Color c = fill;
			if (mx < -0.15f && my > -0.1f) c = hi;
			else if (mx > 0.2f || my < -0.35f) c = shade;
			ImageDrawPixel(img, px, py, c);
		}
	}

	for (int py = y0; py < y1; py++) {
		for (int px = x0; px < x1; px++) {
			float mx = (float)(px - cx) / scale;
			float my = -(float)(py - cy) / scale;
			if (!heartImplicitInside(mx, my)) continue;
			bool boundary = false;
			for (int dy = -1; dy <= 1 && !boundary; dy++) {
				for (int dx = -1; dx <= 1; dx++) {
					if (dx == 0 && dy == 0) continue;
					float mx2 = (float)(px + dx - cx) / scale;
					float my2 = -(float)(py + dy - cy) / scale;
					if (!heartImplicitInside(mx2, my2)) {
						boundary = true;
						break;
					}
				}
			}
			if (boundary) ImageDrawPixel(img, px, py, edge);
		}
	}
}

Texture2D GenerateTilesetTexture(void)
{
	const int tw = 48;
	const int th = 48;
	const int cols = 6;
	Image img = GenImageColor(cols * tw, th, (Color){ 22, 20, 28, 255 });

	drawStoneFloor(&img, 0 * tw, 0, tw, th);
	drawBrickWall(&img, 1 * tw, 0, tw, th);
	drawSlimePuddle(&img, 2 * tw, 0, tw, th);
	drawLockedDoor(&img, 3 * tw, 0, tw, th);
	drawOpenDoor(&img, 4 * tw, 0, tw, th);
	drawHealthPickup(&img, 5 * tw, 0, tw, th);

	Texture2D tex = LoadTextureFromImage(img);
	UnloadImage(img);
	return tex;
}

static void janitorCell(Image *img, int ox, int oy, int cw, int ch, int face, int frame)
{
	Color bg = (Color){ 46, 48, 58, 255 };
	Color skin = (Color){ 230, 198, 168, 255 };
	Color overall = (Color){ 52, 98, 188, 255 };
	Color overallDark = (Color){ 38, 72, 140, 255 };
	Color hat = (Color){ 118, 74, 46, 255 };
	Color mopPole = (Color){ 160, 150, 130, 255 };
	Color mopHead = (Color){ 210, 210, 200, 255 };

	ImageDrawRectangle(img, ox, oy, cw, ch, bg);

	int bob = (frame == 0) ? 0 : 1;
	int cx = ox + cw / 2;
	int headY = oy + 7 + bob;

	if (face == 0) {
		ImageDrawRectangle(img, cx - 10, oy + 18 + bob, 20, 14, overall);
		ImageDrawRectangle(img, cx - 6, oy + 28 + bob, 4, 8, overallDark);
		ImageDrawRectangle(img, cx + 2, oy + 28 + bob, 4, 8, overallDark);
		ImageDrawCircle(img, cx, headY + 4, 8, skin);
		ImageDrawRectangle(img, cx - 9, headY - 2, 18, 6, hat);
		ImageDrawRectangle(img, cx + 10, oy + 14 + bob, 3, 18, mopPole);
		ImageDrawRectangle(img, cx + 8, oy + 30 + bob, 10, 6, mopHead);
	} else if (face == 1) {
		ImageDrawRectangle(img, cx - 10, oy + 18 + bob, 20, 14, overallDark);
		ImageDrawRectangle(img, cx - 6, oy + 28 + bob, 4, 8, overallDark);
		ImageDrawRectangle(img, cx + 2, oy + 28 + bob, 4, 8, overallDark);
		ImageDrawRectangle(img, cx - 9, headY - 2, 18, 8, hat);
		ImageDrawRectangle(img, cx - 3, oy + 22 + bob, 6, 4, mopPole);
	} else if (face == 2) {
		ImageDrawRectangle(img, cx - 4, oy + 18 + bob, 10, 14, overall);
		ImageDrawRectangle(img, cx - 12, headY + 2, 10, 10, skin);
		ImageDrawRectangle(img, cx - 14, headY, 8, 5, hat);
		ImageDrawRectangle(img, cx - 18, oy + 16 + bob, 3, 20, mopPole);
		ImageDrawRectangle(img, cx - 22, oy + 12 + bob, 6, 8, mopHead);
	} else {
		ImageDrawRectangle(img, cx - 6, oy + 18 + bob, 10, 14, overall);
		ImageDrawRectangle(img, cx + 2, headY + 2, 10, 10, skin);
		ImageDrawRectangle(img, cx + 6, headY, 8, 5, hat);
		ImageDrawRectangle(img, cx + 15, oy + 16 + bob, 3, 20, mopPole);
		ImageDrawRectangle(img, cx + 16, oy + 12 + bob, 6, 8, mopHead);
	}
}

Texture2D GenerateJanitorAtlas(void)
{
	const int cols = 2;
	const int rows = 4;
	const int cw = 40;
	const int ch = 40;
	Image img = GenImageColor(cols * cw, rows * ch, (Color){ 40, 42, 52, 255 });

	for (int row = 0; row < rows; row++) {
		for (int col = 0; col < cols; col++) {
			janitorCell(&img, col * cw, row * ch, cw, ch, row, col);
		}
	}

	Texture2D tex = LoadTextureFromImage(img);
	UnloadImage(img);
	return tex;
}

Texture2D GenerateSlimeAtlas(void)
{
	const int cellW = 36;
	const int cellH = 36;
	const int cols = 4;
	const int rows = 1;
	Image img = GenImageColor(cols * cellW, rows * cellH, (Color){ 36, 40, 48, 255 });
	Color body = (Color){ 48, 210, 120, 255 };
	Color shade = (Color){ 28, 140, 82, 255 };
	Color hi = (Color){ 160, 255, 200, 255 };

	for (int i = 0; i < cols; i++) {
		int x = i * cellW + 2;
		int y = 2;
		int w = cellW - 4;
		int h = cellH - 4;
		float squash = 1.0f - 0.12f * (float)(i % 2);
		int nh = (int)((float)h * squash);
		int ny = y + (h - nh) / 2;
		int cx = x + w / 2;
		int cy = ny + nh / 2;
		imageFillOval(&img, cx, cy + 1, w / 2 - 3, nh / 2 - 2, shade);
		imageFillOval(&img, cx, cy, w / 2 - 4, nh / 2 - 3, body);
		ImageDrawCircle(&img, cx - 5, cy - 2, 3, hi);
		ImageDrawCircle(&img, cx + 4, cy + 2, 2, (Color){ 20, 90, 55, 255 });
	}

	Texture2D tex = LoadTextureFromImage(img);
	UnloadImage(img);
	return tex;
}

Texture2D GenerateGhostAtlas(void)
{
	const int cellW = 40;
	const int cellH = 40;
	const int cols = 4;
	const int rows = 1;
	Image img = GenImageColor(cols * cellW, rows * cellH, (Color){ 32, 36, 48, 255 });

	for (int f = 0; f < cols; f++) {
		int ox = f * cellW;
		int oy = 0;
		Color sheet = (Color){ 200, 210, 255, 255 };
		Color inner = (Color){ 170, 185, 240, 255 };
		int wobble = (f % 2 == 0) ? 0 : 1;
		int cx = ox + cellW / 2;
		int bodyTop = oy + 8;
		imageFillOval(&img, cx, bodyTop + 10, 14, 12 + wobble, sheet);
		imageFillOval(&img, cx, bodyTop + 11, 10, 8, inner);
		ImageDrawCircle(&img, cx - 4, bodyTop + 8, 2, (Color){ 30, 30, 40, 255 });
		ImageDrawCircle(&img, cx + 5, bodyTop + 8, 2, (Color){ 30, 30, 40, 255 });
		int tailW = 4 + f;
		for (int t = 0; t < 4; t++) {
			int tx = cx - 10 + t * 6 + (f + t) % 2;
			int ty = bodyTop + 18 + t * 2;
			ImageDrawRectangle(&img, tx, ty, tailW, 3, sheet);
		}
		ImageDrawRectangleLines(
			&img, (Rectangle){ (float)ox + 4, (float)oy + 6, (float)cellW - 8, (float)cellH - 10 },
			1, (Color){ 220, 230, 255, 60 });
	}

	Texture2D tex = LoadTextureFromImage(img);
	UnloadImage(img);
	return tex;
}

Texture2D GenerateChargerAtlas(void)
{
	const int cellW = 40;
	const int cellH = 40;
	const int cols = 4;
	const int rows = 1;
	Image img = GenImageColor(cols * cellW, rows * cellH, (Color){ 36, 32, 44, 255 });

	for (int f = 0; f < cols; f++) {
		int ox = f * cellW;
		int oy = 0;
		int cx = ox + cellW / 2;
		int cy = oy + cellH / 2;
		float tilt = (float)(f - 1) * 0.25f;
		Color aura = (Color){ 200, 60, 90, 255 };
		Color handle = (Color){ 90, 58, 42, 255 };
		Color bristle = (Color){ 190, 175, 140, 255 };

		ImageDrawCircle(&img, cx, cy, 16, (Color){ (unsigned char)(aura.r / 4), (unsigned char)(aura.g / 4),
							   (unsigned char)(aura.b / 4), 255 });
		int lean = (int)(tilt * 8.0f);
		ImageDrawRectangle(&img, cx - 2 + lean, cy - 14, 4, 22, handle);
		ImageDrawRectangle(&img, cx - 10 + lean, cy + 6, 20, 8, bristle);
		ImageDrawRectangle(&img, cx - 8 + lean, cy + 4, 16, 3, (Color){ 160, 140, 100, 255 });
		ImageDrawRectangle(&img, cx + 6 + lean / 2, cy - 10 - f, 3, 6, aura);
		ImageDrawRectangle(&img, cx - 2 + lean / 2, cy - 12 - f, 4, 5, aura);
	}

	Texture2D tex = LoadTextureFromImage(img);
	UnloadImage(img);
	return tex;
}
