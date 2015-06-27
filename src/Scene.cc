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

#include "Scene.hh"

SDL2pp::SDL& Scene::GetSDL() const { return application_.sdl_; }
SDL2pp::Window& Scene::GetWindow() const { return application_.window_; }
SDL2pp::Renderer& Scene::GetRenderer() const { return application_.renderer_; }

void Scene::SetExit(bool exit) {
	application_.must_exit_ = exit;
}

Scene::Scene(Application& application) : application_(application) {
}

Scene::~Scene() {
}

void Scene::ProcessEvent(const SDL_Event& event) {
	if (event.type == SDL_QUIT) {
		SetExit(true);
		return;
	} else if (event.type == SDL_KEYDOWN) {
		switch (event.key.keysym.sym) {
		case SDLK_ESCAPE: case SDLK_q:
			SetExit(true);
			return;
		}
	}
}

void Scene::Update() {
}

void Scene::Render() {
	GetRenderer().SetDrawColor(0, 0, 0);
	GetRenderer().Clear();
}
