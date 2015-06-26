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
#include "GameMap.hh"

#include "Constants.hh"

class GameScene : public Scene {
private:
	SDL2pp::Texture tiles_;
	GameMap game_map_;

	LowresPainter painter_;

private:
	struct DynamicObject {
		float x;
		float y;
		float xvel;
		float yvel;

		SDL2pp::Rect rect;

		DynamicObject(int x, int y, const SDL2pp::Rect& rect)
			: x(x), y(y),
			  xvel(0), yvel(0),
			  rect(rect) {
		}

		SDL2pp::Point GetPoint() const {
			return SDL2pp::Point((int)x, (int)y);
		}

		SDL2pp::Rect GetRect() const {
			return rect + GetPoint();
		}
	};

private:
	unsigned int prev_frame_time_;

	DynamicObject player_;

	enum class ControlFlags {
		LEFT = 0x01,
		RIGHT = 0x02,
		UP = 0x04,
	};

	enum class CollisionState {
		NONE = 0,
		LEFT = 0x01,
		RIGHT = 0x02,
		TOP = 0x04,
		BOTTOM = 0x08,

		SCREENLEFT = 0x10,
		SCREENRIGHT = 0x20,
		SCREENTOP = 0x40,
		SCREENBOTTOM = 0x80,
	};

	int control_flags_;

public:
	GameScene(Application& app);

	virtual void ProcessEvent(const SDL_Event& event) override;
	virtual void Update() override;
	virtual void Render() override;

	void RenderGround(const SDL2pp::Point& offset);
	void RenderPlayer(const SDL2pp::Point& offset);

	int MoveWithCollision(DynamicObject& object, float delta_time);
};

#endif // GAMESCENE_HH
