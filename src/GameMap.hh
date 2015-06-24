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

#ifndef GAMEMAP_HH
#define GAMEMAP_HH

#include <string>
#include <vector>
#include <array>

#include "Constants.hh"

class GameMap {
protected:
	std::vector<int> map_data_;
	unsigned int width_;
	unsigned int height_;

public:
	typedef std::array<int, SCREEN_WIDTH_TILES * SCREEN_HEIGHT_TILES> ScreenTileData;

public:
	GameMap(const std::string& tmxpath);

	ScreenTileData GetScreenTileData(int x, int y);
};

#endif // GAMEMAP_HH
