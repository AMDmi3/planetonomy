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

#include "LowresPainter.hh"

LowresPainter::LowresPainter(SDL2pp::Renderer& renderer, SDL2pp::Texture& tiles, int width, int height)
	: renderer_(renderer),
	  tiles_(tiles),
	  screen_width_(width),
	  screen_height_(height) {
	UpdateSize();
}

void LowresPainter::UpdateSize() {
	int target_width = renderer_.GetOutputWidth();
	int target_height = renderer_.GetOutputHeight();

	scale_factor_ = std::min(target_width/screen_width_, target_height/screen_height_);
	if (scale_factor_ < 1)
		scale_factor_ = 1;

	offset_ = SDL2pp::Point(
			(target_width - screen_width_ * scale_factor_) / 2,
			(target_height - screen_height_ * scale_factor_) / 2
		);
}

void LowresPainter::Copy(const SDL2pp::Rect& src, const SDL2pp::Point& dst) {
	renderer_.Copy(tiles_, src, SDL2pp::Rect(offset_.x + dst.x * scale_factor_, offset_.y + dst.y * scale_factor_, src.w * scale_factor_, src.h * scale_factor_));
}

void LowresPainter::Clear() {
	renderer_.FillRect(SDL2pp::Rect(offset_.x, offset_.y, screen_width_ * scale_factor_, screen_height_ * scale_factor_));
}
