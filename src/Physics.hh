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

#ifndef PHYSICS_HH
#define PHYSICS_HH

// Gravity
constexpr float kGForce = 100.0f;

// Amount of velocity to add on jumping
constexpr float kJumpImpulse = 58.0f;

constexpr float kWalkAccel = 300.0f;
constexpr float kWalkDecel = kWalkAccel * 2;
constexpr float kWalkMaxSpeed = 40.0f;

constexpr float kAirControlRate = 0.2f;

// This is set based on the following idea:
// Falling down one screen should be OK (by map design, it'll be common
// along the game). Top speed for falling one screen down after jumping
// is 204.3.
// Longer falls (such as into the `nose') should be fatal. Top speed for
// shortest possible fall into the nose is 226.2
// Fatal set should be in-between. Maybe reconsidered later for bowel
// and lungs.
constexpr float kFatalSpeed = 210.0f;

// If player moves on the ground and encounters a step of this
// pixels high, he is automatically moved up this step. This
// makes it possible to travel up gentle slopes without need to
// jump; too high amount hehaves ugly though
constexpr int kAutoStepAmount = 2;

#endif // PHYSICS_HH
