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

#ifndef GAMESCENE_HH
#define GAMESCENE_HH

#include <array>

#include <SDL2pp/Texture.hh>

#include "Scene.hh"
#include "LowresPainter.hh"

#include "Constants.hh"

class GameScene : public Scene {
private:
	SDL2pp::Texture tiles_;

	LowresPainter painter_;

private:
	struct DynamicObject {
		float x;
		float y;
		float xvel;
		float yvel;

		int width;
		int height;

		DynamicObject(int x, int y, int width, int height)
			: x(x), y(y),
			  xvel(0), yvel(0),
			  width(width), height(height) {
		}
	};

private:
	unsigned int prev_frame_time_;

	std::array<bool, SCREEN_WIDTH_TILES * SCREEN_HEIGHT_TILES> ground_;

	DynamicObject player_;

	enum ControlFlags {
		LEFT = 0x01,
		RIGHT = 0x02,
		UP = 0x04,
	};

	int control_flags_;

public:
	GameScene(Application& app);

	virtual void ProcessEvent(const SDL_Event& event) override;
	virtual void Update() override;
	virtual void Render() override;

	void RenderGround();
	void RenderPlayer();

	bool MoveWithCollision(DynamicObject& object, float delta_time);
};

#endif // GAMESCENE_HH
