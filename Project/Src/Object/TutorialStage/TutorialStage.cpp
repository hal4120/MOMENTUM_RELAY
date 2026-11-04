#include "TutorialStage.h"

#include "../Common/Collider/MeshCollider.h"

TutorialStage::TutorialStage() :
	ActorBase()
{
}

void TutorialStage::Load(void)
{
	// モデル
	trans.LoadModel("TutorialStage/Base/Base");

	// スケール
	trans.scale = 15;

	// コライダー
	AddCollider(new MeshCollider(COLLIDER_TAG::Stage));
}
