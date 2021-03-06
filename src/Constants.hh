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

constexpr int kTileSize = 16;

constexpr int kScreenWidthPixels = 320;
constexpr int kScreenHeightPixels = 200;

constexpr int kAtlasWidthTiles = 16;

constexpr int kScreenWidthTiles = kScreenWidthPixels / kTileSize;
constexpr int kScreenHeightTiles = kScreenHeightPixels / kTileSize;

#endif // CONSTANTS_HH
