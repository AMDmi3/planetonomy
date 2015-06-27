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

#ifndef SCENE_HH
#define SCENE_HH

#include <SDL2/SDL.h>

#include <SDL2pp/SDL.hh>
#include <SDL2pp/Window.hh>
#include <SDL2pp/Renderer.hh>

#include "Application.hh"

class Scene {
protected:
	Application& application_;

protected:
	SDL2pp::SDL& GetSDL() const;
	SDL2pp::Window& GetWindow() const;
	SDL2pp::Renderer& GetRenderer() const;

	template<class NewScene, class... Args>
	void SetNextScene(Args&&... args) {
		application_.next_scene_.reset(new NewScene(application_, std::forward<Args>(args)...));
	}

	void SetExit(bool exit = true);

public:
	Scene(Application& application);
	virtual ~Scene();

	virtual void ProcessEvent(const SDL_Event& event);
	virtual void Update();
	virtual void Render();
};

#endif // SCENE_HH
