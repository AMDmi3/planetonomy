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

static const float kGForce = 100.0f;

static const float kJumpImpulse = 58.0f;

static const float kWalkAccel = 300.0f;
static const float kWalkDecel = kWalkAccel*2;
static const float kWalkMaxSpeed = 40.0f;

static const float kAirControlRate = 0.2f;

#endif // PHYSICS_HH
