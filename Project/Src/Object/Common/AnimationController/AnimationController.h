#pragma once
#include<map>
#include<string>


class AnimationController
{
public:

	// アニメーションデータ
	struct Animation
	{
		int model = -1;
		int attachNo = -1;
		int animIndex = 0;
		float speed = 0.0f;
		float totalTime = 0.0f;
		float step = 0.0f;
		bool loop = false;
	};

	// コンストラクタ
	AnimationController(int modelId);

	// デストラクタ
	~AnimationController(void);

	//外部FBXからアニメーションを追加
	void Add(int type, float speed, bool loop, const std::string path);

	// 同じFBX内のアニメーションを準備
	void AddInFbx(int type, float speed, bool loop, int animIndex);
	
	/// <summary>
	/// / アニメーション再生
	/// </summary>
	/// <param name="type">再生タイプ</param>
	/// <param name="loop">ループ再生フラグ。指定なし(-1)で登録された情報で再生</param>
	void Play(int type, signed char loop = -1);

	void Stop(void);
	void Update(void);
	void Release(void);

	bool IsEnd(void)const;
	int GetAnimePlayType(void)const { return playType; }
	bool IsAnimEnd(void)const { return playAnim.step >= playAnim.totalTime; }

	float GetAnimeRatio(void)const { return (playAnim.step / playAnim.totalTime); }
	float GetAnimeTotalTime(void)const { return playAnim.totalTime / playAnim.speed; }

	// 再生ステップを取得
	float GetAnimeStep(void)const { return playAnim.step; }
	// 再生ステップを設定
	void SetAnimeStep(float step);

private:
	// アニメーションするモデルのハンドルID
	int modelId;

	// 種類別のアニメーションデータ
	std::map<int, Animation> animations;

	// 再生中のアニメーション
	int playType;
	Animation playAnim;
	Animation prevAnim;
	bool loopflg;

	//アニメーションブレンドの割合
	float blendRate;

	//デタッチできているか
	int isDetach;

	void Add(int type, float speed, bool loop, Animation animation);
};