#pragma once

#include "Common/ActorBase/ActorBase.h"

class TestObject : public ActorBase
{
public:
	TestObject(const Vector3& initPos,const Vector3& MOVE_POWER);
	~TestObject()override = default;

	// ƒ[ƒh
	void Load(void)override;

	// ¨‚¢‚ğ’D‚í‚ê‚é
	const Vector3& VelGreed(void);

private:

	// ’Ç‰ÁXV
	void SubUpdate(void)override;
	// ’Ç‰Á•`‰æ
	void SubDraw(void)override;

	// ”h¶æ’Ç‰ÁÚ’n
	void SubOnGrounded(COLLIDER_TAG ownTag, const ColliderBase& other, const Vector3& pushVec)override;

	// ¨‚¢‚ğ’D‚í‚ê‚Ä‚¢‚éó‘Ô
	bool velocityGreed;

	// ˆÚ“®—Ê‰ÁZŠÔŠu
	const unsigned short MOVE_INTERVAL = 200;
	unsigned short intervalCounter;
	signed char reverse;

	// ˆÚ“®—Ê
	const Vector3 MOVE_POWER;

};