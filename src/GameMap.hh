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

#include "Constants.hh"
#include "TileType.hh"

class GameMap {
protected:
	std::vector<TileType> map_data_;
	unsigned int width_;
	unsigned int height_;

public:
	GameMap(const std::string& tmxpath);

	TileType GetTile(int x, int y) const;

	unsigned int GetWidth() const;
	unsigned int GetHeight() const;
};

#endif // GAMEMAP_HH
