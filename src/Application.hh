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

#ifndef APPLICATION_HH
#define APPLICATION_HH

#include <string>

#include <SDL2pp/SDL.hh>
#include <SDL2pp/Window.hh>
#include <SDL2pp/Renderer.hh>
#include <SDL2pp/Texture.hh>

class Scene;

class Application {
	friend class Scene;
private:
	SDL2pp::SDL sdl_;
	SDL2pp::Window window_;
	SDL2pp::Renderer renderer_;

	std::unique_ptr<Scene> current_scene_;
	std::unique_ptr<Scene> next_scene_;
	bool must_exit_;

private:
	bool CheckFlags();
	void MainLoop();

public:
	Application(const std::string& title);

	template<class NewScene, class... Args>
	void Run(Args&&... args) {
		current_scene_.reset(new NewScene(*this, std::forward<Args>(args)...));
		MainLoop();
	}
};

#endif // APPLICATION_HH
