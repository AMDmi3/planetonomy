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

#include "GameMap.hh"
#include "LowresPainter.hh"
#include "Scene.hh"

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
		SDL2pp::Rect collision_rect;

		DynamicObject(int x, int y, const SDL2pp::Rect& rect)
			: x(x), y(y),
			  xvel(0), yvel(0),
			  rect(rect),
			  collision_rect(rect) {
		}

		SDL2pp::Point GetPoint() const {
			return SDL2pp::Point((int)x, (int)y);
		}

		SDL2pp::Rect GetRect() const {
			return rect + GetPoint();
		}

		SDL2pp::Rect GetCollisionRect() const {
			return collision_rect + GetPoint();
		}
	};

private:
	unsigned int prev_frame_time_;

	DynamicObject player_;
	bool player_facing_right_ = true;

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

		DEADLY = 0x100,
	};

	int control_flags_ = 0;

	// misc. objects
	DynamicObject lander_;

public:
	GameScene(Application& app);

	virtual void ProcessEvent(const SDL_Event& event) override;
	virtual void Update() override;
	virtual void Render() override;

	void UpdatePlayer(float delta_time);

	void RenderGround(const SDL2pp::Point& offset);
	void RenderPlayer(const SDL2pp::Point& offset);
	void RenderLander(const SDL2pp::Point& offset);

	int MoveWithCollision(DynamicObject& object, float delta_time) const;

	int CheckCollisionWithStatic(const SDL2pp::Rect& rect) const;

	void Death(const std::string& message);
};

#endif // GAMESCENE_HH
