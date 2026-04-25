#!/usr/bin/env python3
# quick script to make placeholder textures for the game
# run from the project root

import os
import sys

try:
    from PIL import Image, ImageDraw
except ImportError:
    print("need Pillow (pip install pillow)", file=sys.stderr)
    sys.exit(1)

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT = os.path.join(ROOT, "assets", "textures")

# sizes in GameAssets.h
TILE_COLS, TILE_ROWS = 6, 1
PL_COLS, PL_ROWS = 2, 4
ENEMY_COLS, ENEMY_ROWS = 4, 1
CELL = 48  # same as map tile size


def cell_rect(col, row):
    return (col * CELL, row * CELL, (col + 1) * CELL, (row + 1) * CELL)


def make_tileset():
    w, h = TILE_COLS * CELL, TILE_ROWS * CELL
    im = Image.new("RGBA", (w, h), (0, 0, 0, 0))
    dr = ImageDraw.Draw(im)
    # 1..6: floor, wall, mess, locked exit, open exit, health
    colors = [
        (88, 84, 102, 255),   # floor
        (40, 38, 50, 255),    # wall
        (32, 140, 80, 255),   # mess
        (120, 60, 100, 255),  # locked
        (220, 200, 120, 255),  # open exit
        (220, 70, 100, 255),  # health
    ]
    for i, c in enumerate(colors):
        x0, y0, x1, y1 = cell_rect(i, 0)
        dr.rectangle([x0, y0, x1, y1], fill=c, outline=(255, 255, 255, 80), width=2)
    return im


def make_player_atlas():
    w, h = PL_COLS * CELL, PL_ROWS * CELL
    im = Image.new("RGBA", (w, h), (50, 55, 80, 255))
    dr = ImageDraw.Draw(im)
    for row in range(PL_ROWS):
        for col in range(PL_COLS):
            x0, y0, x1, y1 = cell_rect(col, row)
            dr.ellipse([x0 + 6, y0 + 6, x1 - 6, y1 - 6], fill=(180, 160, 200, 255), outline=(255, 255, 255, 120), width=2)
    return im


def make_enemy_strip(name):
    w, h = ENEMY_COLS * CELL, ENEMY_ROWS * CELL
    base = {
        "slime": (60, 180, 100, 255),
        "ghost": (200, 210, 255, 180),
        "charger": (255, 160, 80, 255),
    }[name]
    im = Image.new("RGBA", (w, h), (0, 0, 0, 0))
    dr = ImageDraw.Draw(im)
    for i in range(ENEMY_COLS):
        x0, y0, x1, y1 = cell_rect(i, 0)
        c = (base[0] + i * 15, min(255, base[1] + i * 5), min(255, base[2] - i * 8), 255) if name != "ghost" else base
        dr.ellipse([x0 + 4, y0 + 4, x1 - 4, y1 - 4], fill=c, outline=(0, 0, 0, 100), width=2)
    return im


def main():
    os.makedirs(OUT, exist_ok=True)
    make_tileset().save(os.path.join(OUT, "tileset.png"), "PNG")
    make_player_atlas().save(os.path.join(OUT, "player.png"), "PNG")
    make_enemy_strip("slime").save(os.path.join(OUT, "slime.png"), "PNG")
    make_enemy_strip("ghost").save(os.path.join(OUT, "ghost.png"), "PNG")
    make_enemy_strip("charger").save(os.path.join(OUT, "charger.png"), "PNG")
    print("Wrote:", ", ".join(f for f in os.listdir(OUT) if f.endswith(".png")))


if __name__ == "__main__":
    main()
