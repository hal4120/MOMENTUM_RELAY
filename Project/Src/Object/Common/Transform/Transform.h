#pragma once

#include "../../../Common/Vector3.h"
#include "../../../Common/Quaternion.h"

struct Transform
{
	// モデル
	int model;

	// 座標
	Vector3 pos;

	// 1フレーム前の座標
	Vector3 prevPos;

	// 描画する際の座標のズレを補正するための変数
	// モデルの中心を軸に描画するために使用
	Vector3 centerDiff;

	// 回転
	Quaternion rotation;

	// 描画する際の回転のズレを補正するための回転
	Quaternion localRotation;

	// スケール
	Vector3 scale;

	// 動的オブジェクトかどうか
	// true  = 動的オブジェクト
	// false = 静的オブジェクト
	bool dynamicFlg;


	/// <summary>
	/// 生成
	/// </summary>
	Transform(void) :
		model(-1),

		pos(),
		prevPos(),

		centerDiff(),

		rotation(),
		localRotation(),

		scale(1),

		dynamicFlg(true)
	{
	}


	// 現在の移動量
	Vector3 Velocity(void) const;


	/// <summary>
	/// 自身の回転を行列として取得する
	/// </summary>
	MATRIX RotationMat(void) const;

	/// <summary>
	/// ローカル補正を含めた最終回転を取得する
	/// </summary>
	Quaternion FinalRotation(void) const;

	/// <summary>
	/// ローカル補正を含めた最終回転を行列として取得する
	/// </summary>
	MATRIX FinalRotationMat(void) const;


	/// <summary>
	/// Vector3を自身の回転情報で回転させる
	/// </summary>
	Vector3 VTrans(const Vector3& v) const;

	/// <summary>
	/// VECTORを自身の回転情報で回転させる
	/// </summary>
	Vector3 VTrans(const VECTOR& v) const;


#pragma region 回転設定

	/// <summary>
	/// 回転をQuaternionで直接設定する
	/// </summary>
	void SetRotation(const Quaternion& rotation);

	/// <summary>
	/// XZY順のEuler角をラジアンで指定して回転を設定する
	/// </summary>
	void SetAngle(const Vector3& angle);

	/// <summary>
	/// XZY順のEuler角を度数法で指定して回転を設定する
	/// </summary>
	void SetAngleDeg(const Vector3& deg);

	void SetAngleXDeg(float deg);
	void SetAngleYDeg(float deg);
	void SetAngleZDeg(float deg);


	/// <summary>
	/// Euler角による回転を追加する
	/// </summary>
	void AddAngle(const Vector3& angle);

	void AddAngleDeg(const Vector3& deg);

	void AddAngleXDeg(float deg);
	void AddAngleYDeg(float deg);
	void AddAngleZDeg(float deg);


	/// <summary>
	/// ローカル補正回転をQuaternionで直接設定する
	/// </summary>
	void SetLocalRotation(const Quaternion& rotation);

	/// <summary>
	/// XZY順のEuler角をラジアンで指定してローカル補正回転を設定する
	/// </summary>
	void SetLocalAngle(const Vector3& angle);

	/// <summary>
	/// XZY順のEuler角を度数法で指定してローカル補正回転を設定する
	/// </summary>
	void SetLocalAngleDeg(const Vector3& deg);

#pragma endregion


	/// <summary>
	/// モデルをロード
	/// </summary>
	/// <param name="path">モデルのパス（Data/Model/～.mv1）</param>
	void LoadModel(std::string path);

	/// <summary>
	/// モデルを複製
	/// </summary>
	void Duplicate(int model);

	/// <summary>
	/// エフェクトをロード
	/// </summary>
	/// <param name="path">エフェクトのパス（Data/Effect/～.efk）</param>
	void LoadEffect(std::string path);


	// Transform情報をモデルに適用
	void Attach(void);

	// モデルを描画
	void Draw(void);

	// モデルを解放
	void Release(void);
};