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
	  player_(SCREEN_WIDTH_PIXELS,
			  SCREEN_HEIGHT_PIXELS + SCREEN_HEIGHT_PIXELS / 2.0f,
			  SDL2pp::Rect(-SpriteData[SPRITE_PLAYER].w / 2, -SpriteData[SPRITE_PLAYER].h + 1, SpriteData[SPRITE_PLAYER].w, SpriteData[SPRITE_PLAYER].h)
		  ) {
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
	// clear whole window to make actualy rendering area visible
	SDL2pp::Rect clip = GetRenderer().GetClipRect();
	GetRenderer().SetClipRect();
	GetRenderer().SetDrawColor(16, 16, 16);
	GetRenderer().Clear();
	GetRenderer().SetClipRect(clip);

	// low-res rendering starts
	GetRenderer().SetDrawColor(0, 0, 0);
	painter_.Clear();

	SDL2pp::Point screen_offset{
		player_.GetPoint().x / (SCREEN_WIDTH_TILES * TILE_SIZE) * (SCREEN_WIDTH_TILES * TILE_SIZE),
		player_.GetPoint().y / (SCREEN_HEIGHT_TILES * TILE_SIZE) * (SCREEN_HEIGHT_TILES * TILE_SIZE)
	};

	RenderGround(screen_offset);
	RenderPlayer(screen_offset);
}

void GameScene::RenderGround(const SDL2pp::Point& offset) {
	for (int y = 0; y < (SCREEN_HEIGHT_PIXELS + TILE_SIZE - 1) / TILE_SIZE; y++) {
		for (int x = 0; x < SCREEN_WIDTH_TILES; x++) {
			TileType tt = game_map_.GetTile(offset.x / TILE_SIZE + x, offset.y / TILE_SIZE + y);

			// handle tiled's flipping flags
			int flipflag = 0;
			double angle = 0.0;
			if (tt.IsDFlipped()) {
				flipflag = (tt.IsHFlipped() ? 0 : SDL_FLIP_VERTICAL) | (tt.IsVFlipped() ? SDL_FLIP_HORIZONTAL : 0);
				angle = 90.0;
			} else {
				flipflag = (tt.IsHFlipped() ? SDL_FLIP_HORIZONTAL : 0) | (tt.IsVFlipped() ? SDL_FLIP_VERTICAL : 0);
				angle = 0.0;
			}

			// select tile sprite to render
			int sprite_to_render = -1;
			switch (tt.GetType()) {
			case TileType::EMPTY:  break;
			case TileType::GROUND: sprite_to_render = SPRITE_GROUND; break;
			case TileType::FIXME:  sprite_to_render = SPRITE_FIXME; break;
			default:               sprite_to_render = SPRITE_FIXME; break;
			}

			if (sprite_to_render >= 0)
				painter_.Copy(SpriteData[sprite_to_render], SDL2pp::Point(x * TILE_SIZE, y * TILE_SIZE), angle, SDL2pp::NullOpt, flipflag);
		}
	}
}

void GameScene::RenderPlayer(const SDL2pp::Point& offset) {
	painter_.Copy(
			SpriteData[SPRITE_PLAYER],
			SDL2pp::Point(
				(int)player_.GetRect().x - offset.x,
				(int)player_.GetRect().y - offset.y
			)
		);
}

int GameScene::MoveWithCollision(GameScene::DynamicObject& object, float delta_time) {
	// move in 1 pixel steps, checking collisions on each step
	int num_steps = 1 + (int)(std::max(std::abs(object.xvel), std::abs(object.yvel)) * delta_time);

	int result = (int)CollisionState::NONE;
	for (int step = 0; step < num_steps; step++) {
		// collision rectangle
		SDL2pp::Rect coll_rect = { object.GetRect().x - 1, object.GetRect().y - 1, object.GetRect().w + 2, object.GetRect().h + 2 };

		// side collision GetRect()angles
		SDL2pp::Rect left_rect = { object.GetRect().x - 1, object.GetRect().y - 1, 1, object.GetRect().h };
		SDL2pp::Rect right_rect = { object.GetRect().x + object.GetRect().w, object.GetRect().y - 1, 1, object.GetRect().h };
		SDL2pp::Rect top_rect = { object.GetRect().x - 1, object.GetRect().y - 1, object.GetRect().w, 1 };
		SDL2pp::Rect bottom_rect = { object.GetRect().x - 1, object.GetRect().y + object.GetRect().h, object.GetRect().w, 1 };

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

					if (top_rect.Intersects(ground_rect))
						result |= (int)CollisionState::TOP;
					if (left_rect.Intersects(ground_rect))
						result |= (int)CollisionState::LEFT;
					if (bottom_rect.Intersects(ground_rect))
						result |= (int)CollisionState::BOTTOM;
					if (right_rect.Intersects(ground_rect))
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
