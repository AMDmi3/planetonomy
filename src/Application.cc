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

#include "Application.hh"

#include <SDL2/SDL.h>

#include "Scene.hh"

bool Application::CheckFlags() {
	if (must_exit_)
		return false;

	if (next_scene_.get()) {
		current_scene_.swap(next_scene_);
		next_scene_.reset(nullptr);
	}

	return true;
}

void Application::MainLoop() {
	while (1) {
		// Process events
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			current_scene_->ProcessEvent(event);
			if (!CheckFlags())
				return;
		}

		// Update scene logic
		current_scene_->Update();
		if (!CheckFlags())
			return;

		// Render
		current_scene_->Render();
		renderer_.Present();

		// Frame limiter
		SDL_Delay(1);
	}
}

Application::Application(const std::string& title) :
	sdl_(SDL_INIT_VIDEO),
	window_(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_RESIZABLE),
	renderer_(window_, -1, SDL_RENDERER_ACCELERATED),
	must_exit_(false) {
}
