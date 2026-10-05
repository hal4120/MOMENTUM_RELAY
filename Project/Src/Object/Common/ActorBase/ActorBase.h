#pragma once

#include <string>
#include <vector>
#include <map>

#include "../Transform/Transform.h"

#include "../Collider/ColliderTagDefine.h"

#include "ActorDrawTypeDefine.h"

class ColliderBase;
struct CollisionResult;

class ShaderBase;

class ParameterLoad;

struct Vector2;
struct Vector2I;

class ActorBase
{
public:
	
	// デフォルトコンストラクタ
	ActorBase();
	// パラメーターを外部から読み込む場合に使うコンストラクタ
	ActorBase(const std::string& parameterPath);

	// デストラクタ
	virtual ~ActorBase() = default;

	// 読み込み処理
	virtual void Load(void) = 0;
	// 初期化処理
	virtual void Init(void);
	// 更新処理
	virtual void Update(void);
	// 描画処理
	virtual void Draw(void);
	// 解放処理
	virtual void Release(void);

	// モデルを複製する
	void ModelDuplicate(int model) { trans.Duplicate(model); }

	// モデル制御情報構造体のゲット関数
	const Transform& GetTrans(void)const { return trans; }

	// 当たり判定の通知
	virtual void OnCollision(COLLIDER_TAG ownTag, const ColliderBase& other, const CollisionResult& result) {}

	// 接地判定の通知
	virtual void OnGrounded(COLLIDER_TAG ownTag, const ColliderBase& other) {

		// 接地判定
		isGroundMaster = true;

		// 挙動
		SubOnGrounded(ownTag, other);
	}

	// コライダーすべてを取得
	virtual std::vector<ColliderBase*> GetColliders(void)const;

	// コライダーのデバッグ描画
	void DrawColliderDebug(void)const;

	// 描画タイプを取得
	const ACTOR_DRAW_TYPE& GetDrawType(void)const { return drawType; }

	// 子クラスを取得する
	const std::vector<ActorBase*>& GetChildActors(void)const { return childActors; }

private:

	// 当たり判定情報
	std::vector<ColliderBase*> colliders;
	
	// 動的オブジェクトかどうか（true = 動的、false = 静的）
	bool dynamicFlg;

	// 重力を適用するかどうか（true = する、false = しない）
	bool gravityFlg;

	// 衝突時押し出しを行うかどうか（true = 押し出す、false = 押し出さず通り抜ける）
	bool pushFlg;

	// 押し出しを行う際の重さ（0 ～ 100 で設定）
	unsigned char pushWeight;

	// 加速度の更新
	void VelocityUpdate(bool deceleration);

	// 重力更新
	void Gravity(void);

	// 重力加速度
	static constexpr float GRAVITY = -0.9f;
	// 重力の最大値
	static constexpr float GRAVITY_MAX = -100.0f;

	// 接地判定 管理用(派生先で変更不可で参照渡し)
	bool isGroundMaster;

	// 描画判定 （true =「描画する」、false =「描画しない」）
	bool isDraw;

	// 描画タイプ（通常描画、アルファ描画、UI描画など）
	ACTOR_DRAW_TYPE drawType;

	// シェーダーデータ
	ShaderBase* shader;

	// パラメーター外部ファイル管理クラス
	ParameterLoad* parameter;

	// 子クラス
	std::vector<ActorBase*> childActors;

protected:

	// モデル制御情報構造体
	Transform trans;

	// 移動量
	Vector3 velocity;

	// 横軸加速度の1フレームごとの加速量
	float ACCEL_RATE = 3.0f;
	// 横軸加速度の1フレームごとの減速量
	float DECEL_RATE = 3.0f;
	// 横軸加速度の最大値
	float ACCEL_MAX = 30.0f;

	// 接地判定(派生先で参照用)
	const bool& isGround = isGroundMaster;

#pragma region 初期設定

	// コライダー追加
	void AddCollider(ColliderBase* newCollider);

	/// <summary>
	/// 移動するかを切り替える
	/// </summary>
	/// <param name="flg">true = 「移動する」に切り替える、false = 「移動しない」に切り替える</param>
	void SetDynamicFlg(bool flg) { dynamicFlg = trans.dynamicFlg = flg; }

	/// <summary>
	/// 重力を適用するかを切り替える
	/// </summary>
	/// <param name="flg">true = 「する」に切り替える、false = 「しない」に切り替える</param>
	void SetGravityFlg(bool flg) { gravityFlg = flg; }

	/// <summary>
	/// 衝突時押し出しを行うかを設定する
	/// </summary>
	/// <param name="flg">true = 押し出す、false = 押し出さず通り抜ける</param>
	void SetPushFlg(bool flg) { pushFlg = flg; }

	/// <summary>
	/// 押し出しを行う際の重さ
	/// </summary>
	/// <param name="weight">0 ～ 100 で設定（数値が大きいほど重い）</param>
	void SetPushWeight(unsigned char weight) { pushWeight = weight; }

	// シェーダーを設定
	void CreateShader(ShaderBase* newShader);
	// シェーダーを消去
	void ResetShader();

	// 子クラスを追加する
	void AddChildActor(ActorBase* child) {
		child->Load();
		childActors.push_back(child); 
	}
#pragma endregion


#pragma region パラメーター外部ファイル管理に関する関数

	bool IsParameterExist(const std::string& fileName, const std::string& parameterName)const;

	/// <summary>
	/// パラメーター外部ファイル管理クラスから指定のパラメーターの指定の配列番号の値だけを取得する
	/// </summary>
	/// <param name="parameterName">パラメーターのID</param>
	/// <param name="index">配列番号（指定なしで0）</param>
	/// <returns></returns>
	float GetParameter(const std::string& fileName, const std::string& parameterName, int index = 0)const;

	/// <summary>
	/// パラメーター外部ファイル管理クラスから指定のパラメーターを配列ごと取得する
	/// </summary>
	/// <param name="parameterName">パラメーターのID</param>
	/// <returns></returns>
	const std::vector<float>& GetParameterArray(const std::string& fileName, const std::string& parameterName)const;

	/// <summary>
	/// パラメーター外部ファイル管理クラスから指定のパラメーターの指定の配列番号の値だけをint型にキャストして取得する
	/// </summary>
	/// <param name="parameterName">パラメーターのID</param>
	/// <param name="index">配列番号（指定なしで0）</param>
	/// <returns></returns>
	int GetParameterToInt(const std::string& fileName, const std::string& parameterName, int index = 0)const;

	/// <summary>
	/// パラメーター外部ファイル管理クラスから指定のパラメーターをVector3構造体にして取得する
	/// </summary>
	/// <param name="parameterName">パラメーターのID</param>
	/// <returns></returns>
	Vector3 GetParameterToVector3(const std::string& fileName, const std::string& parameterName);

	/// <summary>
	/// パラメーター外部ファイル管理クラスから指定のパラメーターをVector2構造体にして取得する
	/// </summary>
	/// <param name="parameterName">パラメーターのID</param>
	/// <returns></returns>
	Vector2 GetParameterToVector2(const std::string& fileName, const std::string& parameterName);

	/// <summary>
	/// パラメーター外部ファイル管理クラスから指定のパラメーターをVector2I構造体にして取得する
	/// </summary>
	/// <param name="parameterName">パラメーターのID</param>
	/// <returns></returns>
	Vector2I GetParameterToVector2I(const std::string& fileName, const std::string& parameterName);

#pragma endregion


	/// <summary>
	/// 当たり判定フラグの取得
	/// </summary>
	/// <returns>所有するコライダーどれか1つでも「判定する」状態ならtrue</returns>
	bool GetJudgeFlg(void);
	// 当たり判定の設定（true = 「判定する」、false = 「判定しない」）
	void SetJudgeFlg(bool flg);

	// 移動するオブジェクトかの設定を取得
	bool GetDynamicFlg(void)const { return dynamicFlg; }
	// 重力の設定の状態を取得
	bool GetGravityFlg(void)const { return gravityFlg; }

	// 描画判定の取得（true = 「描画する」、false = 「描画しない」）
	bool GetIsDraw(void)const { return isDraw; }
	// 描画判定の設定（true = 「描画する」、false = 「描画しない」）
	void SetIsDraw(bool flg) { isDraw = flg; }
	// 描画判定の設定（引数省略で現在の逆にスイッチ）
	void SetIsDraw(void) { isDraw = !isDraw; }

	// 描画タイプの設定
	void SetDrawType(const ACTOR_DRAW_TYPE& type) { drawType = type; }

	// 指定の方向に向かって加速度を用いて移動する
	void MoveAccel(const Vector3& vec);

	// 派生先追加初期化
	virtual void SubInit(void) {}
	// 派生先追加更新
	virtual void SubUpdate(void) {}
	// 派生先追加描画
	virtual void SubDraw(void) {}
	// 派生先追加解放
	virtual void SubRelease(void) {}

	// 派生先追加接地判定（デフォルトはY軸加速度の打ち消し）
	virtual void SubOnGrounded(COLLIDER_TAG ownTag, const ColliderBase& other) {

		// 動的オブジェクトの場合のみ、下方向のY軸加速度のみを打ち消す
		if (dynamicFlg) { velocity.y = (velocity.y < 0.0f) ? 0.0f : velocity.y; }
	}

private:

	// 中間基底クラス用追加処理
	virtual void BaseInit(void) {}
	virtual void BaseUpdate(void) {}
	virtual void BaseDraw(void) {}
	virtual void BaseRelease(void) {}
};