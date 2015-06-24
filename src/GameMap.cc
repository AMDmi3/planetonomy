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

#include "GameMap.hh"

#include <pugixml.hpp>

GameMap::GameMap(const std::string& tmxpath) {
	pugi::xml_document tmxdoc;

	if (!tmxdoc.load_file(tmxpath.c_str()))
		throw std::runtime_error("cannot load map file");

	auto map = tmxdoc.child("map");
	width_ = map.attribute("width").as_int();
	height_ = map.attribute("height").as_int();
	std::string map_csv = map.child("layer").child_value("data");

	if (width_ == 0 || height_ == 0 || map_csv.empty())
		throw std::runtime_error("cannot process map file");

	map_data_.reserve(width_ * height_);

	int curint = 0;
	int curlen = 0;
	for (auto& ch : map_csv) {
		if (ch >= '0' && ch <= '9') {
			curint = curint * 10 + ch - '0';
			curlen++;
		} else {
			if (curlen)
				map_data_.push_back(curint);
			curint = 0;
			curlen = 0;
		}
	}

	if (map_data_.size() != width_ * height_)
		throw std::runtime_error("cannot process map file");
}

GameMap::ScreenTileData GameMap::GetScreenTileData(int x, int y) {
	GameMap::ScreenTileData result;

	int target_index = 0;
	for (int row = y * SCREEN_HEIGHT_TILES; row < y * SCREEN_HEIGHT_TILES + SCREEN_HEIGHT_TILES; row++)
		for (int column = x * SCREEN_WIDTH_TILES; column < x * SCREEN_WIDTH_TILES + SCREEN_WIDTH_TILES; column++)
			result[target_index++] = map_data_[row * width_ + column];

	return result;
}