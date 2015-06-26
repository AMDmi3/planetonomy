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

#ifndef TILETYPE_HH
#define TILETYPE_HH

class TileType {
public:
	enum Type {
		EMPTY = 0,
		GROUND = 2,
		FIXME = 3,
	};

	enum FlipFlags {
		DFLIP = 0x2,
		VFLIP = 0x4,
		HFLIP = 0x8,
	};

private:
	int data_; // as stored by tiled

public:
	static TileType DefaultEmpty() {
		return TileType(EMPTY);
	}

	static TileType DefaultFull() {
		return TileType(GROUND);
	}

	TileType() : data_(EMPTY) {
	}

	TileType(unsigned int data) : data_(data) {
	}

	bool IsPassable() const {
		return GetType() == EMPTY;
	}

	Type GetType() const {
		return static_cast<Type>(data_ & 0xfffffff);
	}

	int GetFlipFlag() const {
		return data_ >> 28;
	}
};

#endif // TILETYPE_HH
