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
	  player_(game_map_.GetMetaTileInfo("player")),
	  lander_(game_map_.GetMetaTileInfo("lander")) {

	player_.Place(game_map_.GetObject(GameMap::PLAYER_START).rect);
	lander_.Place(game_map_.GetObject(GameMap::LANDER).rect);

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

	UpdatePlayer(delta_time);
}

void GameScene::UpdatePlayer(float delta_time) {
	// Make gravity work
	player_.yvel += kGForce * delta_time;

	float original_yvel = player_.yvel;

	// Update player position
	int moveresult = MoveWithCollision(player_, delta_time);

	// Handle some death conditions
	if (moveresult & (int)CollisionState::DEADLY) {
		Death("you've touched something deadly");
		return;
	}
	if (moveresult & (int)CollisionState::BOTTOM && original_yvel >= kFatalSpeed) {
		Death("you fell to your death");
		return;
	}

	// Process player controls
	bool on_ground = (moveresult & (int)CollisionState::BOTTOM) && player_.yvel >= 0.0f;
	float control_rate = on_ground ? 1.0 : kAirControlRate;

	// Move left/right
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

	// Jump
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
		player_.GetAnchor().x / (kScreenWidthTiles * kTileSize) * (kScreenWidthTiles * kTileSize),
		player_.GetAnchor().y / (kScreenHeightTiles * kTileSize) * (kScreenHeightTiles * kTileSize)
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

			if (tt.IsFlipped()) {
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

				painter_.Copy(
						tt.GetSourceRect(),
						SDL2pp::Point(x * kTileSize, y * kTileSize),
						angle,
						SDL2pp::NullOpt,
						flipflag
					);
			} else {
				painter_.Copy(
						tt.GetSourceRect(),
						SDL2pp::Point(x * kTileSize, y * kTileSize)
					);
			}
		}
	}
}

void GameScene::RenderPlayer(const SDL2pp::Point& offset) {
	painter_.Copy(
			player_.GetSrcRect(),
			player_.GetPoint() - offset,
			0.0,
			SDL2pp::NullOpt,
			player_facing_right_ ? 0 : SDL_FLIP_HORIZONTAL
		);
}

void GameScene::RenderLander(const SDL2pp::Point& offset) {
	painter_.Copy(
			lander_.GetSrcRect(),
			lander_.GetPoint() - offset
		);
}

int GameScene::MoveWithCollision(GameScene::DynamicObject& object, float delta_time) const {
	// move in 1 pixel steps, checking collisions on each step
	int num_steps = 1 + (int)(std::max(std::abs(object.xvel), std::abs(object.yvel)) * delta_time);

	int result = (int)CollisionState::NONE;
	for (int step = 0; step < num_steps && (object.xvel != 0.0f || object.yvel != 0.0f); step++) {
		result = (int)CollisionState::NONE;

		// try normal collision
		object.ForeachCollisionRect([&result, this](const SDL2pp::Rect& rect){
				result |= CheckCollisionWithStatic(rect);
			});

		// if applicable, try autostep
		if (result & (int)CollisionState::BOTTOM &&
				((result & (int)CollisionState::LEFT && object.xvel < 0.0f) ||
				(result & (int)CollisionState::RIGHT && object.xvel > 0.0f))) {
			for (int autostep = 1; autostep <= kAutoStepAmount; autostep++) {
				int tryresult = (int)CollisionState::NONE;

				object.ForeachCollisionRect([&tryresult, autostep, this](const SDL2pp::Rect& rect){
						tryresult |= CheckCollisionWithStatic(rect - SDL2pp::Point(0, autostep));
					});

				if (tryresult == (int)CollisionState::NONE) {
					result = tryresult;
					object.y -= autostep;
					break;
				}
			}
		}

		if (result & (int)CollisionState::LEFT && object.xvel < 0.0f)
			object.xvel = 0.0f;
		if (result & (int)CollisionState::RIGHT && object.xvel > 0.0f)
			object.xvel = 0.0f;
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
			const auto& tile = game_map_.GetTile(x, y);
			for (auto& coll_rect : tile.GetCollisionMap()) {
				SDL2pp::Rect ground_rect = coll_rect + SDL2pp::Point(x * kTileSize, y * kTileSize);

				int tile_result = 0;
				if (top_rect.Intersects(ground_rect))
					tile_result |= (int)CollisionState::TOP;
				if (left_rect.Intersects(ground_rect))
					tile_result |= (int)CollisionState::LEFT;
				if (bottom_rect.Intersects(ground_rect))
					tile_result |= (int)CollisionState::BOTTOM;
				if (right_rect.Intersects(ground_rect))
					tile_result |= (int)CollisionState::RIGHT;

				if (tile_result && tile.IsDeadly())
					tile_result |= (int)CollisionState::DEADLY;

				result |= tile_result;
			}
		}
	}

	return result;
}

void GameScene::Death(const std::string& message) {
	std::cerr << "Game over (" << message << ")" << std::endl;
	SetExit(true);
}
