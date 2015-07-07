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

#include "GameScene.hh"

#include <algorithm>
#include <cmath>
#include <cassert>

#include "Constants.hh"
#include "Physics.hh"
#include "Sprites.hh"

GameScene::GameScene(Application& app)
	: Scene(app),
	  tiles_(GetRenderer(), DATADIR "/images/tiles.png"),
	  game_map_(DATADIR "/maps/planetonomy.tmx"),
	  painter_(GetRenderer(), tiles_, kScreenWidthPixels, kScreenHeightPixels),
	  prev_frame_time_(SDL_GetTicks()),
	  player_(0.0f, 0.0f,
			  SDL2pp::Rect(-SpriteData[(int)SpriteNames::PLAYER].w / 2, -SpriteData[(int)SpriteNames::PLAYER].h + 1, SpriteData[(int)SpriteNames::PLAYER].w, SpriteData[(int)SpriteNames::PLAYER].h)
		  ),
	  lander_(0.0f, 0.0f, SDL2pp::Rect(SpriteData[(int)SpriteNames::LANDER]).SetX(0).SetY(0)) {
	player_.collision_rect.y += 1; // XXX: unhardcode
	player_.collision_rect.h -= 1;

	player_.x = game_map_.GetObject(GameMap::PLAYER_START).rect.x + game_map_.GetObject(GameMap::PLAYER_START).rect.w / 2;
	player_.y = game_map_.GetObject(GameMap::PLAYER_START).rect.GetY2();

	lander_.x = game_map_.GetObject(GameMap::LANDER).rect.x;
	lander_.y = game_map_.GetObject(GameMap::LANDER).rect.y;

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
			player_facing_right_ = false;
			break;
		case SDLK_RIGHT:
			control_flags_ |= (int)ControlFlags::RIGHT;
			player_facing_right_ = true;
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
	player_.yvel += kGForce * delta_time;

	int moveresult = MoveWithCollision(player_, delta_time);

	// process player controls
	bool on_ground = (moveresult & (int)CollisionState::BOTTOM) && player_.yvel >= 0.0f;
	float control_rate = on_ground ? 1.0 : kAirControlRate;

	// move left/right
	if (control_flags_ & (int)ControlFlags::LEFT && player_.xvel >= -kWalkMaxSpeed) {
		player_.xvel = std::max(-kWalkMaxSpeed, player_.xvel - control_rate * kWalkAccel * delta_time);
	} else if (control_flags_ & (int)ControlFlags::RIGHT && player_.xvel <= kWalkMaxSpeed) {
		player_.xvel = std::min(kWalkMaxSpeed, player_.xvel + control_rate * kWalkAccel * delta_time);
	} else if (on_ground) { // decelerate when on ground
		if (player_.xvel > 0)
			player_.xvel -= std::min(player_.xvel, kWalkDecel * delta_time);
		if (player_.xvel < 0)
			player_.xvel += std::min(-player_.xvel, kWalkDecel * delta_time);
	}

	// jump
	if (on_ground && control_flags_ & (int)ControlFlags::UP)
		player_.yvel -= kJumpImpulse;
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
		player_.GetPoint().x / (kScreenWidthTiles * kTileSize) * (kScreenWidthTiles * kTileSize),
		player_.GetPoint().y / (kScreenHeightTiles * kTileSize) * (kScreenHeightTiles * kTileSize)
	};

	RenderGround(screen_offset);
	RenderLander(screen_offset);
	RenderPlayer(screen_offset);
}

void GameScene::RenderGround(const SDL2pp::Point& offset) {
	for (int y = 0; y < (kScreenHeightPixels + kTileSize - 1) / kTileSize; y++) {
		for (int x = 0; x < kScreenWidthTiles; x++) {
			GameMap::Tile tt = game_map_.GetTile(offset.x / kTileSize + x, offset.y / kTileSize + y);

			if (tt.GetType() == 0)
				continue;

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

			int nsrctile = tt.GetType() - 1;
			int srcx = nsrctile % kAtlasWidthTiles * kTileSize;
			int srcy = nsrctile / kAtlasWidthTiles * kTileSize;

			painter_.Copy(
					SDL2pp::Rect(srcx, srcy, kTileSize, kTileSize),
					SDL2pp::Point(x * kTileSize, y * kTileSize),
					angle,
					SDL2pp::NullOpt,
					flipflag
				);
		}
	}
}

void GameScene::RenderPlayer(const SDL2pp::Point& offset) {
	painter_.Copy(
			SpriteData[(int)SpriteNames::PLAYER],
			SDL2pp::Point(
				(int)player_.GetRect().x - offset.x,
				(int)player_.GetRect().y - offset.y
			),
			0.0,
			SDL2pp::NullOpt,
			player_facing_right_ ? 0 : SDL_FLIP_HORIZONTAL
		);
}

void GameScene::RenderLander(const SDL2pp::Point& offset) {
	painter_.Copy(
			SpriteData[(int)SpriteNames::LANDER],
			SDL2pp::Point(
				(int)lander_.GetRect().x - offset.x,
				(int)lander_.GetRect().y - offset.y
			)
		);
}

int GameScene::MoveWithCollision(GameScene::DynamicObject& object, float delta_time) const {
	// move in 1 pixel steps, checking collisions on each step
	int num_steps = 1 + (int)(std::max(std::abs(object.xvel), std::abs(object.yvel)) * delta_time);

	int result = (int)CollisionState::NONE;
	for (int step = 0; step < num_steps; step++) {
		result |= CheckCollisionWithStatic(object.GetCollisionRect());

		// autostep
		if (result & (int)CollisionState::LEFT && object.xvel < 0.0f) {
			int newres;
			for (int i = 0; i < kAutoStepAmount; i++) {
				if (result & (int)CollisionState::BOTTOM && (newres = CheckCollisionWithStatic(object.GetCollisionRect() - SDL2pp::Point(0, i))) == (int)CollisionState::NONE) {
					result = newres;
					object.y -= i;
					break;
				}
			}

			if (result & (int)CollisionState::LEFT)
				object.xvel = 0.0f;
		}
		if (result & (int)CollisionState::RIGHT && object.xvel > 0.0f) {
			int newres;
			for (int i = 0; i < kAutoStepAmount; i++) {
				if (result & (int)CollisionState::BOTTOM && (newres = CheckCollisionWithStatic(object.GetCollisionRect() - SDL2pp::Point(0, i))) == (int)CollisionState::NONE) {
					result = newres;
					object.y -= i;
					break;
				}
			}

			if (result & (int)CollisionState::RIGHT)
				object.xvel = 0.0f;
		}

		if (result & (int)CollisionState::TOP && object.yvel < 0.0f)
			object.yvel = 0.0f;
		if (result & (int)CollisionState::BOTTOM && object.yvel > 0.0f)
			object.yvel = 0.0f;

		object.x += object.xvel * delta_time / num_steps;
		object.y += object.yvel * delta_time / num_steps;
	}

	return result;
}

int GameScene::CheckCollisionWithStatic(const SDL2pp::Rect& rect) const {
	// collision rectangle
	const SDL2pp::Rect coll_rect { rect.x - 1, rect.y - 1, rect.w + 2, rect.h + 2 };

	// side collision rectangles
	const SDL2pp::Rect left_rect { rect.x - 1, rect.y, 1, rect.h };
	const SDL2pp::Rect right_rect { rect.x + rect.w, rect.y, 1, rect.h };
	const SDL2pp::Rect top_rect { rect.x, rect.y - 1, rect.w, 1 };
	const SDL2pp::Rect bottom_rect { rect.x, rect.y + rect.h, rect.w, 1 };

	int result = 0;

	// this + std::max() in the loops below to avoid signed math
	// problems with negative tile coordinates
	if (coll_rect.GetX() < 0)
		result |= (int)CollisionState::LEFT;
	if (coll_rect.GetY() < 0)
		result |= (int)CollisionState::TOP;

	for (int y = std::max(coll_rect.y / kTileSize, 0); y <= coll_rect.GetY2() / kTileSize; y++) {
		for (int x = std::max(coll_rect.x / kTileSize, 0); x <= coll_rect.GetX2() / kTileSize; x++) {
			for (auto& coll_rect : game_map_.GetTile(x, y).GetCollisionMap()) {
				SDL2pp::Rect ground_rect = coll_rect + SDL2pp::Point(x * kTileSize, y * kTileSize);

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

	return result;
}
