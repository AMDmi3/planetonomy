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

GameScene::GameScene(Application& app)
	: Scene(app),
	  tiles_(GetRenderer(), DATADIR "/images/tiles.png"),
	  painter_(GetRenderer(), tiles_, 320, 200) {
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
	for (int x = 0; x < 20; x++)
		for (int y = 0; y < 12; y++)
			painter_.Copy(SDL2pp::Rect(16, 0, 16, 16), SDL2pp::Point(x * 16, y * 16));
}
