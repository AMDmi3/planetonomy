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
#include <map>
#include <functional>

#include <SDL2pp/Rect.hh>

namespace pugi {
	class xml_node;
};

class GameMap {
public:
	typedef std::vector<SDL2pp::Rect> CollisionMap;

	struct TileInfo {
		CollisionMap collision_map;
		bool damaging_flag = false;

		TileInfo() {
		}
	};

	class Tile {
	private:
		unsigned int data_;
		const TileInfo& info_;

	public:
		Tile(unsigned int data, const GameMap& map)
			: data_(data),
			  info_(map.GetTileInfo(GetType())) {
		}

		unsigned int GetType() const {
			return data_ & 0x0fffffff;
		}

		bool IsHFlipped() const {
			// http://doc.mapeditor.org/reference/tmx-map-format/#tile-flipping
			return data_ & 0x80000000;
		}

		bool IsVFlipped() const {
			// http://doc.mapeditor.org/reference/tmx-map-format/#tile-flipping
			return data_ & 0x40000000;
		}

		bool IsDFlipped() const {
			// http://doc.mapeditor.org/reference/tmx-map-format/#tile-flipping
			return data_ & 0x20000000;
		}

		bool IsDamaging() const {
			return info_.damaging_flag;
		}

		const CollisionMap& GetCollisionMap() const {
			return info_.collision_map;
		}
	};

	enum ObjectTypes {
		LANDER,
		PLAYER_START,
		MOUTH_MONSTER,
	};

	struct Object {
		ObjectTypes type;
		SDL2pp::Rect rect;
	};

protected:
	std::map<unsigned int, TileInfo> tile_infos_;
	std::vector<unsigned int> map_data_;
	unsigned int width_;
	unsigned int height_;

	std::vector<Object> objects_;

public:
	GameMap(const std::string& tmxpath);

	void ParseTileset(const pugi::xml_node& map);
	void ParseMap(const pugi::xml_node& map);
	void ParseObjects(const pugi::xml_node& map);

	unsigned int GetWidth() const;
	unsigned int GetHeight() const;

	Tile GetTile(int x, int y) const;
	const TileInfo& GetTileInfo(unsigned int id) const;

	const Object& GetObject(ObjectTypes type) const;
	void ForeachObject(std::function<void(const Object&)> processor) const;
	void ForeachObject(ObjectTypes type, std::function<void(const Object&)> processor) const;
};

#endif // GAMEMAP_HH
