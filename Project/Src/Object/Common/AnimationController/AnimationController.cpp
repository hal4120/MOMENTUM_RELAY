#include "AnimationController.h"

#include "../../../pch.h"

#include "../../../Manager/TimeScale/TimeScale.h"

AnimationController::AnimationController(int modelId) : 
	modelId(modelId),
	playType(-1),

	playAnim(),

	loopflg(false),

	blendRate(),
	isDetach()
{
}

AnimationController::~AnimationController(void)
{
}

void AnimationController::Add(int type, float speed, bool loop, const std::string path)
{
	Animation animation;
	animation.model = MV1LoadModel(path.c_str());
	animation.animIndex = -1;
	Add(type, speed, loop, animation);
}

void AnimationController::AddInFbx(int type, float speed, bool loop, int animIndex)
{
	Animation animation;
	animation.model = -1;
	animation.animIndex = animIndex;

	Add(type, speed, loop, animation);
}

void AnimationController::Play(int type, signed char loop)
{
	// 同じアニメーションが再生されている場合は何もしない
	if (playType == type) {

		// ただし、ループ再生が無効な場合は、アニメーションの再生位置をリセットする
		if ((loop == -1 && !playAnim.loop) || loop == (signed char)false) {
			playAnim.step = 0.0f;
		}

		return;
	}

	// アニメーションが存在しない場合は何もしない
	if (playType != -1) {
		if (isDetach == -1) { MV1DetachAnim(modelId, prevAnim.attachNo); }
		// モデルからアニメーションを外す
		MV1DetachAnim(modelId, playAnim.attachNo);
	}

	// アニメーション種別を変更
	prevAnim = playAnim;
	playType = type;
	playAnim = animations[type];

	if (prevAnim.speed != 0) {
		if (playAnim.model == -1) {
			// モデルと同じファイルからアニメーションをアタッチする
			prevAnim.attachNo = MV1AttachAnim(modelId, prevAnim.animIndex);
		}
		else {
			int animIndex = 0;
			prevAnim.attachNo = MV1AttachAnim(modelId, animIndex, prevAnim.model);
		}
		MV1SetAttachAnimTime(modelId, prevAnim.attachNo, prevAnim.step);
	}


	// 初期化
	loopflg = (loop == -1 && playAnim.loop) || loop == (signed char)true;
	playAnim.step = 0.0f;

	if (playAnim.model == -1) {
		// モデルと同じファイルからアニメーションをアタッチする
		playAnim.attachNo = MV1AttachAnim(modelId, playAnim.animIndex);
	}
	else {
		int animIndex = 0;
		playAnim.attachNo = MV1AttachAnim(modelId, animIndex, playAnim.model);
	}

	// アニメーション総時間の取得
	playAnim.totalTime = MV1GetAttachAnimTotalTime(modelId, playAnim.attachNo);

	// ブレンド用
	blendRate = 0.1f;
	isDetach = -1;

	if (prevAnim.speed != 0) {

		MV1SetAttachAnimBlendRate(modelId, prevAnim.attachNo, 1.0f - blendRate);
		MV1SetAttachAnimBlendRate(modelId, playAnim.attachNo, blendRate);
	}
}

void AnimationController::Stop(void)
{
	MV1DetachAnim(modelId, playAnim.attachNo);
	playType = -1;
}


void AnimationController::Update(void)
{
	// 再生
	playAnim.step += playAnim.speed * TimeScale::Get();

	if (prevAnim.speed != 0) {
		while (blendRate <= 1.0f) {
			if (blendRate >= 0.9f) {

				MV1SetAttachAnimBlendRate(modelId, playAnim.attachNo, 1.0f);
				isDetach = MV1DetachAnim(modelId, prevAnim.attachNo);

				break;
			}
			MV1SetAttachAnimBlendRate(modelId, prevAnim.attachNo, 1.0f - blendRate);
			MV1SetAttachAnimBlendRate(modelId, playAnim.attachNo, blendRate);

			break;
		}
		blendRate += 0.1f * TimeScale::Get();
	}
	if (loopflg) {
		if (playAnim.step >= playAnim.totalTime)playAnim.step = 0.0f;
	}
	else {
		if (playAnim.step >= playAnim.totalTime)playAnim.step = playAnim.totalTime;

	}

	// アニメーション設定
	MV1SetAttachAnimTime(modelId, playAnim.attachNo, playAnim.step);
}


void AnimationController::Release(void)
{
	MV1DetachAnim(modelId, playAnim.attachNo);

	//ロードした外部FBXのモデルのメモリを解放する
	for (auto& pair : animations) {
		if (pair.second.model == -1)continue;
		MV1DeleteModel(pair.second.model);
	}

	//可変長配列をクリア
	animations.clear();
}

bool AnimationController::IsEnd(void) const
{
	bool ret = false;

	if (loopflg)return ret;

	if (playAnim.step >= playAnim.totalTime)ret = true;


	return ret;
}

void AnimationController::SetAnimeStep(float step)
{
	playAnim.step = step;
	if (loopflg) {
		if (playAnim.step >= playAnim.totalTime)playAnim.step = 0.0f;
	}
	else {
		if (playAnim.step >= playAnim.totalTime)playAnim.step = playAnim.totalTime;

	}
	MV1SetAttachAnimTime(modelId, playAnim.attachNo, playAnim.step);
}

void AnimationController::Add(int type, float speed, bool loop, Animation animation)
{
	if (animations.count(type) == 0) {
		//追加
		animation.speed = speed;
		animation.loop = loop;
		animations.emplace(type, animation);
	}
}


