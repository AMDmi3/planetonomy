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

#include "GameScene.hh"

GameScene::GameScene(Application& app)
	: Scene(app),
	  tiles_(GetRenderer(), DATADIR "/images/tiles.png"),
	  painter_(GetRenderer(), tiles_, SCREEN_WIDTH_PIXELS, SCREEN_HEIGHT_PIXELS) {

	std::fill(ground_.begin(), ground_.end(), false);
	for (int x = 0; x < SCREEN_WIDTH_TILES; x++)
		ground_[x + SCREEN_WIDTH_TILES * (SCREEN_HEIGHT_TILES - 1)] = true;

	player_x_ = SCREEN_WIDTH_PIXELS / 2.0f;
	player_y_ = SCREEN_HEIGHT_PIXELS / 2.0f;

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
		}
	} else if (event.type == SDL_WINDOWEVENT) {
		painter_.UpdateSize();
	}
}

void GameScene::Update() {
}

void GameScene::Render() {
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
	painter_.Copy(SpriteData[SPRITE_PLAYER], SDL2pp::Point(player_x_, player_y_));
}
