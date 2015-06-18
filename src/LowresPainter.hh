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

#ifndef LOWRESPAINTER_HH
#define LOWRESPAINTER_HH

#include <SDL2pp/Renderer.hh>
#include <SDL2pp/Texture.hh>
#include <SDL2pp/Rect.hh>

class LowresPainter {
private:
	SDL2pp::Renderer& renderer_;
	SDL2pp::Texture& tiles_;

	const int screen_width_;
	const int screen_height_;

	SDL2pp::Point offset_;
	int scale_factor_;

public:
	LowresPainter(SDL2pp::Renderer& renderer, SDL2pp::Texture& tiles, int width, int height);

	void UpdateSize();
	void Copy(const SDL2pp::Rect& src, const SDL2pp::Point& dst);
};

#endif // LOWRESPAINTER_HH
