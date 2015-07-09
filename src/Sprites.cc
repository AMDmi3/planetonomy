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

#include "Sprites.hh"

const SDL2pp::Rect SpriteData[(int)SpriteNames::NUM_SPRITES] = {
	{ 37, 48, 7, 16 },  // PLAYER
	{ 0, 48, 32, 32 },  // LANDER
	{ 0, 80, 32, 16 },  // MOUTH_MONSTER_0
	{ 0, 96, 32, 16 },  // MOUTH_MONSTER_1
	{ 0, 112, 32, 16 },  // MOUTH_MONSTER_2
};
