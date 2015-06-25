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
	  game_map_(DATADIR "/maps/planetonomy.tmx"),
	  painter_(GetRenderer(), tiles_, SCREEN_WIDTH_PIXELS, SCREEN_HEIGHT_PIXELS),
	  prev_frame_time_(SDL_GetTicks()),
	  player_(SCREEN_WIDTH_PIXELS, SCREEN_HEIGHT_PIXELS + SCREEN_HEIGHT_PIXELS / 2.0f, SpriteData[SPRITE_PLAYER].w, SpriteData[SPRITE_PLAYER].h) {
	control_flags_ = 0;

	painter_.UpdateSize();
}

void GameScene::ProcessEvent(const SDL_Event& event) {
	if (event.type == SDL_QUIT) {
		SetExit(true);
		return;
	} else if (event.type == SDL_KEYDOWN && event.key.repeat == 0) {
		switch (event.key.keysym.sym) {
		case SDLK_ESCAPE: case SDLK_q:
			SetExit(true);
			return;
		case SDLK_LEFT:
			control_flags_ |= (int)ControlFlags::LEFT;
			break;
		case SDLK_RIGHT:
			control_flags_ |= (int)ControlFlags::RIGHT;
			break;
		case SDLK_UP:
			control_flags_ |= (int)ControlFlags::UP;
			break;
		}
	} else if (event.type == SDL_KEYUP) {
		switch (event.key.keysym.sym) {
		case SDLK_LEFT:
			control_flags_ &= ~(int)ControlFlags::LEFT;
			break;
		case SDLK_RIGHT:
			control_flags_ &= ~(int)ControlFlags::RIGHT;
			break;
		case SDLK_UP:
			control_flags_ &= ~(int)ControlFlags::UP;
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

	int moveresult = MoveWithCollision(player_, delta_time);

	// process player controls
	bool on_ground = (moveresult & (int)CollisionState::BOTTOM) && player_.yvel >= 0.0f;
	float control_rate = on_ground ? 1.0 : AirControlRate;

	// move left/right
	if (control_flags_ & (int)ControlFlags::LEFT && player_.xvel >= -WalkMaxSpeed) {
		player_.xvel = std::max(-WalkMaxSpeed, player_.xvel - control_rate * WalkAccel * delta_time);
	} else if (control_flags_ & (int)ControlFlags::RIGHT && player_.xvel <= WalkMaxSpeed) {
		player_.xvel = std::min(WalkMaxSpeed, player_.xvel + control_rate * WalkAccel * delta_time);
	} else if (on_ground) { // decelerate when on ground
		if (player_.xvel > 0)
			player_.xvel -= std::min(player_.xvel, WalkDecel * delta_time);
		if (player_.xvel < 0)
			player_.xvel += std::min(-player_.xvel, WalkDecel * delta_time);
	}

	// jump
	if (on_ground && control_flags_ & (int)ControlFlags::UP)
		player_.yvel -= JumpImpulse;

	// long fall
	if (moveresult & (int)CollisionState::SCREENBOTTOM)
		SetExit(true);
}

void GameScene::Render() {
	GetRenderer().SetDrawColor(16, 16, 16);
	GetRenderer().Clear();

	GetRenderer().SetDrawColor(0, 0, 0);
	painter_.Clear();

	RenderGround();
	RenderPlayer();
}

void GameScene::RenderGround() {
	int screen_x = (int)player_.x / (SCREEN_WIDTH_TILES * TILE_SIZE);
	int screen_y = (int)player_.y / (SCREEN_HEIGHT_TILES * TILE_SIZE);

	for (int y = 0; y < SCREEN_HEIGHT_TILES; y++) {
		for (int x = 0; x < SCREEN_WIDTH_TILES; x++) {
			switch (game_map_.GetTile(screen_x * SCREEN_WIDTH_TILES + x, screen_y * SCREEN_HEIGHT_TILES + y).GetType()) {
			case TileType::GROUND:
				painter_.Copy(SpriteData[SPRITE_GROUND], SDL2pp::Point(x * TILE_SIZE, y * TILE_SIZE));
				break;
			case TileType::FIXME:
				painter_.Copy(SpriteData[SPRITE_FIXME], SDL2pp::Point(x * TILE_SIZE, y * TILE_SIZE));
				break;
			default:
				break;
			}
		}
	}
}

void GameScene::RenderPlayer() {
	painter_.Copy(SpriteData[SPRITE_PLAYER], SDL2pp::Point((int)player_.x % (SCREEN_WIDTH_TILES * TILE_SIZE), (int)player_.y % (SCREEN_HEIGHT_TILES * TILE_SIZE)));
}

int GameScene::MoveWithCollision(GameScene::DynamicObject& object, float delta_time) {
	// move in 1 pixel steps, checking collisions on each step
	int num_steps = 1 + (int)(std::max(std::abs(object.xvel), std::abs(object.yvel)) * delta_time);

	int result = (int)CollisionState::NONE;
	for (int step = 0; step < num_steps; step++) {
		int int_x = (int)object.x;
		int int_y = (int)object.y;

		SDL2pp::Rect coll_rect(
				int_x - 1,
				int_y - 1,
				object.width + 2,
				object.height + 2
			);

		// this + std::max() in the loops below to avoid signed math
		// problems with negative tile coordinates
		if (coll_rect.GetX() < 0)
			result |= (int)CollisionState::LEFT;
		if (coll_rect.GetY() < 0)
			result |= (int)CollisionState::TOP;

		for (int y = std::max(coll_rect.y / TILE_SIZE, 0); y <= coll_rect.GetY2() / TILE_SIZE; y++) {
			for (int x = std::max(coll_rect.x / TILE_SIZE, 0); x <= coll_rect.GetX2() / TILE_SIZE; x++) {
				if (!game_map_.GetTile(x, y).IsPassable()) {
					SDL2pp::Rect ground_rect(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE);

					if (SDL2pp::Rect(int_x, int_y - 1, object.width, 1).Intersects(ground_rect))
						result |= (int)CollisionState::TOP;
					if (SDL2pp::Rect(int_x - 1, int_y, 1, object.height).Intersects(ground_rect))
						result |= (int)CollisionState::LEFT;
					if (SDL2pp::Rect(int_x, int_y + object.height, object.width, 1).Intersects(ground_rect))
						result |= (int)CollisionState::BOTTOM;
					if (SDL2pp::Rect(int_x + object.width, int_y, 1, object.height).Intersects(ground_rect))
						result |= (int)CollisionState::RIGHT;
				}
			}
		}

		if (result & (int)CollisionState::TOP && object.yvel < 0.0f)
			object.yvel = 0.0f;
		if (result & (int)CollisionState::BOTTOM && object.yvel > 0.0f)
			object.yvel = 0.0f;
		if (result & (int)CollisionState::LEFT && object.xvel < 0.0f)
			object.xvel = 0.0f;
		if (result & (int)CollisionState::RIGHT && object.xvel > 0.0f)
			object.xvel = 0.0f;

		object.x += object.xvel * delta_time / num_steps;
		object.y += object.yvel * delta_time / num_steps;
	}

	return result;
}
