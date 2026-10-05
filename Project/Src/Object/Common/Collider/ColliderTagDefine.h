#pragma once

// 形状列挙型定義
enum class COLLIDER_SHAPE
{
	None = -1,

	Line,
	Sphere,
	Capsule,
	Box,
	Mesh,
	XzCircle,
};

// コライダータグ列挙型定義
enum class COLLIDER_TAG
{
	None = -1,

	Player,
	PlayerKickDownAttack,

	Enemy,

	Stage,

	DebugObject,
};