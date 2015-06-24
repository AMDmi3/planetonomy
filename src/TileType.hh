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
	};

private:
	Type type_;

public:
	TileType() : type_(EMPTY) {
	}

	TileType(int type) : type_(static_cast<Type>(type)) {
	}

	bool IsPassable() const {
		return type_ == EMPTY;
	}

	bool operator==(Type type) const {
		return type_ == type;
	}
};

#endif // TILETYPE_HH
