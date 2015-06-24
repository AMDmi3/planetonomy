/*
 * Copyright (C) 2015 Dmitry Marakasov
 *
 * This file is part of planetonomy.
 *
 * planetonomy is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * planetonomy is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with planetonomy.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CONSTANTS_HH
#define CONSTANTS_HH

#include <SDL2pp/Rect.hh>

enum GameConstants {
	TILE_SIZE = 16,
	SCREEN_WIDTH_PIXELS = 320,
	SCREEN_HEIGHT_PIXELS = 200,

	SCREEN_WIDTH_TILES = SCREEN_WIDTH_PIXELS/TILE_SIZE,
	SCREEN_HEIGHT_TILES = SCREEN_HEIGHT_PIXELS/TILE_SIZE,
};

enum SpriteNames {
	SPRITE_GROUND,
	SPRITE_PLAYER,
	SPRITE_FIXME,

	NUM_SPRITES
};

static const float GForce = 100.0f;

static const float JumpImpulse = 58.0f;

static const float WalkAccel = 300.0f;
static const float WalkDecel = WalkAccel*2;
static const float WalkMaxSpeed = 40.0f;

static const float AirControlRate = 0.2f;

static const SDL2pp::Rect SpriteData[NUM_SPRITES] = {
	{ 19, 1, 16, 16 },  // GROUND
	{ 1, 237, 8, 18 },  // PLAYER
	{ 37, 1, 16, 16 },  // FIXME
};

#endif // CONSTANTS_HH
