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

#include <algorithm>
#include <cmath>
#include <cassert>

#include "GameScene.hh"

GameScene::GameScene(Application& app)
	: Scene(app),
	  tiles_(GetRenderer(), DATADIR "/images/tiles.png"),
	  painter_(GetRenderer(), tiles_, SCREEN_WIDTH_PIXELS, SCREEN_HEIGHT_PIXELS),
	  prev_frame_time_(SDL_GetTicks()),
	  player_(SCREEN_WIDTH_PIXELS / 2.0f, SCREEN_HEIGHT_PIXELS / 2.0f, SpriteData[SPRITE_PLAYER].w, SpriteData[SPRITE_PLAYER].h) {
	std::fill(ground_.begin(), ground_.end(), false);
	for (int x = 0; x < SCREEN_WIDTH_TILES; x++)
		ground_[x + SCREEN_WIDTH_TILES * (SCREEN_HEIGHT_TILES - 1)] = true;

	control_flags_ = 0;

	painter_.UpdateSize();
}

void GameScene::ProcessEvent(const SDL_Event& event) {
	if (event.type == SDL_QUIT) {
		SetExit(true);
		return;
	} else if (event.type == SDL_KEYDOWN) {
		switch (event.key.keysym.sym) {
		case SDLK_ESCAPE: case SDLK_q:
			SetExit(true);
			return;
		case SDLK_LEFT:
			control_flags_ |= LEFT;
			break;
		case SDLK_RIGHT:
			control_flags_ |= RIGHT;
			break;
		case SDLK_UP:
			control_flags_ |= UP;
			break;
		}
	} else if (event.type == SDL_KEYUP) {
		switch (event.key.keysym.sym) {
		case SDLK_LEFT:
			control_flags_ &= ~LEFT;
			break;
		case SDLK_RIGHT:
			control_flags_ &= ~RIGHT;
			break;
		case SDLK_UP:
			control_flags_ &= ~UP;
			break;
		}
	} else if (event.type == SDL_WINDOWEVENT) {
		painter_.UpdateSize();
	}
}

void GameScene::Update() {
	// update time
	unsigned int frame_time = SDL_GetTicks();
	float delta_time = (frame_time - prev_frame_time_) / 1000.0f; // seconds
	prev_frame_time_ = frame_time;

	// update player velocity and position
	player_.yvel += GForce * delta_time;

	MoveWithCollision(player_, delta_time);
}

void GameScene::Render() {
	GetRenderer().SetDrawColor(0, 0, 0);
	GetRenderer().Clear();

	RenderGround();
	RenderPlayer();
}

void GameScene::RenderGround() {
	for (int y = 0; y < SCREEN_HEIGHT_TILES; y++)
		for (int x = 0; x < SCREEN_WIDTH_TILES; x++)
			if (ground_[x + y * SCREEN_WIDTH_TILES])
				painter_.Copy(SpriteData[SPRITE_GROUND], SDL2pp::Point(x * TILE_SIZE, y * TILE_SIZE));
}

void GameScene::RenderPlayer() {
	painter_.Copy(SpriteData[SPRITE_PLAYER], SDL2pp::Point(player_.x, player_.y));
}

bool GameScene::MoveWithCollision(GameScene::DynamicObject& object, float delta_time) {
	// move by 1 pixel steps, checking collisions each step
	int num_steps = 1 + (int)(std::max(object.xvel, object.yvel) * delta_time);

	for (int step = 1; step <= num_steps; step++) {
		float new_x = object.x + object.xvel * delta_time * step / num_steps;
		float new_y = object.y + object.yvel * delta_time * step / num_steps;

		SDL2pp::Rect new_rect(
				std::round(new_x),
				std::round(new_y),
				object.width,
				object.height
			);

		for (int y = new_rect.y / TILE_SIZE; y <= std::min(new_rect.GetY2() / TILE_SIZE, SCREEN_HEIGHT_TILES - 1); y++) {
			for (int x = new_rect.x / TILE_SIZE; x <= std::min(new_rect.GetX2() / TILE_SIZE, SCREEN_WIDTH_TILES - 1); x++) {
				if (!ground_[x + y * SCREEN_WIDTH_TILES])
					continue;
				SDL2pp::Rect ground_rect(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE);
				if (new_rect.Intersects(ground_rect)) {
					object.x += object.xvel * delta_time * (step - 1) / num_steps;
					object.y += object.yvel * delta_time * (step - 1) / num_steps;
					return true;
				}
			}
		}
	}

	object.x += object.xvel * delta_time;
	object.y += object.yvel * delta_time;

	return false;
}
