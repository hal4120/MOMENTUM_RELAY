#include "GoalArea.h"

#include "../../Utility/Utility.h"

#include "../Common/Collider/SphereCollider.h"

#include "../Common/Shader/GoalFieldShader.h"

GoalArea::GoalArea()
{
}

void GoalArea::Load(void)
{
#pragma region オブジェクト設定

	// 動的オブジェクトとしての処理を無効にする
	SetColliderMobility(COLLIDER_MOBILITY::Static);

	// 当たり判定による押し出しを無効にする
	SetContactResponseEnabled(false);

#pragma endregion

	// モデルを読み込み
	trans.LoadModel("GoalArea/GoalArea");

	// モデルのスケールを設定
	trans.scale = 2;

	// 座標の設定
	trans.pos = Vector3(-3400.0f, 1577.83f, 1100.0f);

	// 角度の設定
	trans.rotation = Quaternion::FromRotationY(Deg2Rad(90.0f));

	// 当たり判定の設定
	AddCollider(new SphereCollider(COLLIDER_TAG::Goal, 85.0f * trans.scale.MaxElementF()));
}

void GoalArea::DefaultCreateShader(void)
{
	CreateShader(new GoalFieldShader(SHADER_VERTEX_TYPE::Frame1));
}