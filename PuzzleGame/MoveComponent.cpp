// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "MoveComponent.h"
#include "Actor.h"
#include "SDL/SDL.h"

MoveComponent::MoveComponent(class Actor* owner, int updateOrder)
	:Component(owner, updateOrder)
	,mSpeed(10.0f)
	,mDst(owner->GetPosition())
{

}

void MoveComponent::Update(Vector2 pos)
{
	Vector2 nowpos = mOwner->GetPosition();
	if (pos.x > nowpos.x)
		nowpos.x += 10;
	else if (pos.x < nowpos.x)
		nowpos.x -= 10;
	else if (pos.y > nowpos.y)
		nowpos.y += 10;
	else if (pos.y < nowpos.y)
		nowpos.y -= 10;
	mOwner->SetPosition(nowpos);
}
