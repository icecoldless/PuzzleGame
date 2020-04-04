#pragma once
#include "Component.h"
#include "Math.h"

class MoveComponent : public Component
{
public:
	// Lower update order to update first
	MoveComponent(class Actor* owner, int updateOrder = 10);
	void Update(Vector2 pos);

	float GetSpeed() const { return mSpeed; }
	void SetSpeed(float speed) { mSpeed = speed; }
	Vector2 GetDst() const { return mDst; }
	void SetDst(Vector2 pos) { mDst = pos; }
private:
	float mSpeed;
	Vector2 mDst;
};
