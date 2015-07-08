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

#include <cstring>
#include <iostream>

#include <pugixml.hpp>

GameMap::GameMap(const std::string& tmxpath) {
	// prase .tmx xml
	pugi::xml_document tmxdoc;

	if (!tmxdoc.load_file(tmxpath.c_str()))
		throw std::runtime_error("cannot load map file");

	auto map = tmxdoc.child("map");

	ParseTileset(map);
	ParseMap(map);
	ParseObjects(map);
}

void GameMap::ParseTileset(const pugi::xml_node& map) {
	auto tileset = map.child("tileset");
	int firstgid = tileset.attribute("firstgid").as_int();

	for (auto tile = tileset.child("tile"); tile; tile = tile.next_sibling("tile")) {
		int id = tile.attribute("id").as_int() + firstgid;
		if (id < 1)
			throw std::runtime_error("cannot process map file: unexpected tile id");

		TileInfo ti;

		// properties
		for (auto property = tile.child("properties").child("property"); property; property = property.next_sibling("property")) {
			if (std::strcmp(property.attribute("name").as_string(), "deadly") == 0)
				ti.deadly_flag = true;
		}

		// collision
		for (auto rect = tile.child("objectgroup").child("object"); rect; rect = rect.next_sibling("object")) {
			ti.collision_map.emplace_back(
					rect.attribute("x").as_int(),
					rect.attribute("y").as_int(),
					rect.attribute("width").as_int(),
					rect.attribute("height").as_int()
				);
		}

		tile_infos_.insert(std::make_pair(id, ti));
	}

	tile_infos_.insert(std::make_pair(0, TileInfo())); // add empty tile
}

void GameMap::ParseMap(const pugi::xml_node& map) {
	width_ = map.attribute("width").as_int();
	height_ = map.attribute("height").as_int();
	std::string map_csv = map.child("layer").child_value("data");

	if (width_ == 0 || height_ == 0 || map_csv.empty())
		throw std::runtime_error("cannot parse map file: cannot get map dimensions or contents");

	map_data_.reserve(width_ * height_);

	// parse tile array
	unsigned int curint = 0;
	unsigned int curlen = 0;
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
		throw std::runtime_error("cannot parse map file: unexpected number of tiles in the map");
}

void GameMap::ParseObjects(const pugi::xml_node& map) {
	pugi::xml_node objects_group;
	for (auto objectgroup = map.child("objectgroup"); objectgroup; objectgroup = objectgroup.next_sibling("objectgroup"))
		if (std::strcmp(objectgroup.attribute("name").as_string(), "Objects") == 0)
			objects_group = objectgroup;

	if (!objects_group)
		throw std::runtime_error("cannot parse map file: objects layer not found");

	for (auto object = objects_group.child("object"); object; object = object.next_sibling("object")) {
		ObjectTypes type;

		const auto objtypename = object.attribute("type").as_string();
		if (std::strcmp(objtypename, "lander") == 0) {
			type = LANDER;
		} else if (std::strcmp(objtypename, "player_start") == 0) {
			type = PLAYER_START;
		} else if (std::strcmp(objtypename, "mouth_monster") == 0) {
			type = MOUTH_MONSTER;
		} else {
			std::cerr << "WARNING: Unknown object type: " << objtypename << std::endl;
			continue;
		}

		objects_.emplace_back(Object{
				type,
				{
					object.attribute("x").as_int(),
					object.attribute("y").as_int(),
					object.attribute("width").as_int(),
					object.attribute("height").as_int()
				}
			});
	}
}

unsigned int GameMap::GetWidth() const {
	return width_;
}

unsigned int GameMap::GetHeight() const {
	return height_;
}

GameMap::Tile GameMap::GetTile(int x, int y) const {
	static const int default_tile_id = 2; // XXX: unhardcode

	unsigned int tile_id;

	// for out-of-bounds data return full tile to prevent player
	// from leaving the map
	if (x < 0 || y < 0 || (unsigned int)x >= width_ || (unsigned int)y >= height_)
		tile_id = default_tile_id;
	else
		tile_id = map_data_[y * width_ + x];

	return Tile(tile_id, *this);
}

const GameMap::TileInfo& GameMap::GetTileInfo(unsigned int id) const {
	auto tileinfo_it = tile_infos_.find(id);
	if (tileinfo_it == tile_infos_.end())
		tileinfo_it = tile_infos_.find(0);

	return tileinfo_it->second;
}

const GameMap::Object& GameMap::GetObject(GameMap::ObjectTypes type) const {
	for (auto& obj : objects_)
		if (obj.type == type)
			return obj;

	throw std::runtime_error("required object not found");
}

void GameMap::ForeachObject(std::function<void(const GameMap::Object&)> processor) const {
	for (auto& obj : objects_)
		processor(obj);
}

void GameMap::ForeachObject(GameMap::ObjectTypes type, std::function<void(const GameMap::Object&)> processor) const {
	for (auto& obj : objects_)
		if (obj.type == type)
			processor(obj);
}
