#pragma once
// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Actor.h"
#include "MoveComponent.h"
class Puzzle :public Actor
{
public:
	Puzzle(class Game*,Vector2,const string&);
	~Puzzle();

	void Move(Vector2);
private:
	MoveComponent* mc;
};
