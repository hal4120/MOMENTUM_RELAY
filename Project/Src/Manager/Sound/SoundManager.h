#pragma once

#include<vector>
#include<map>
#include<string>
#include<algorithm>

class SoundManager
{
private:

	// コンストラクタ/デストラクタ
	SoundManager();
	~SoundManager() = default;

	// インスタンス
	static SoundManager* ins;

	// コピー/ムーブ 操作を禁止
	SoundManager(const SoundManager&) = delete;
	SoundManager& operator=(const SoundManager&) = delete;
	SoundManager(SoundManager&&) = delete;
	SoundManager& operator=(SoundManager&&) = delete;

public:

#pragma region シングルトン定義
	// 生成/初期化処理
	static void CreateIns(void) { if (ins == nullptr) { ins = new SoundManager(); ins->Init(); } }
	// 取得
	static SoundManager& GetIns(void) { return *ins; }
	// 終了処理/削除
	static void DeleteIns(void) { if (ins != nullptr) { ins->Release(); delete ins; ins = nullptr; } }
#pragma endregion

	// 更新
	void Update(void);

	/// <summary>
	/// 現在の音声情報を破棄して指定されたシーンの音声情報を読み込む
	/// </summary>
	/// <param name="scene">シーンの名前</param>
	void ChangeScene(const std::string& scene);

	/// <summary>
	/// 現在の音声情報を破棄せず指定されたシーンの音声情報を読み込む
	/// </summary>
	/// <param name="scene">シーンの名前</param>
	void AddScene(const std::string& scene);

	/// <summary>
	/// 音声を再生する
	/// </summary>
	/// <param name="id">音声ID</param>
	void Play(const std::string& id);

	/// <summary>
	/// 指定したサウンドを停止
	/// </summary>
	/// <param name="id">音声ID</param>
	void Stop(const std::string& id);

	/// <summary>
	/// 再生中のサウンドをすべて停止
	/// </summary>
	void AllStop(void);

	/// <summary>
	/// 再生中のサウンドをすべて一時停止。「PausePlay()」で再生再開
	/// </summary>
	void Pause(void);

	/// <summary>
	/// 「Pause()」で一時停止したサウンドをすべて再生再開
	/// </summary>
	void PausePlay(void);

#pragma region ボリューム取得
	// マスターボリューム（0～255）
	unsigned char MasterVolume(void)const { return masterVolume; }
	// マスターボリューム割合（0～1）
	float MasterVolumeRatio(void)const { return ((float)masterVolume / (float)MASTER_VOLUME_MAX); }
	// BGMボリューム割合（0～1）
	float BgmVolume(void)const { return bgmVolume; }
	// SEボリューム割合（0～1）
	float SeVolume(void)const { return seVolume; }
#pragma endregion

#pragma region ボリューム変更
	// マスターボリュームを代入（0～255）
	void SetMasterVolume(unsigned char volume) { masterVolume = std::clamp(volume, MASTER_VOLUME_MIN, MASTER_VOLUME_MAX); }
	// マスターボリュームを加算
	void AddMasterVolume(signed char volume) {
		short newVolume = masterVolume + volume;
		masterVolume = (unsigned char)std::clamp(newVolume, (short)MASTER_VOLUME_MIN, (short)MASTER_VOLUME_MAX);
	}

	// BGMボリューム割合を代入（0～1）
	void SetBgmVolume(float volume) { bgmVolume = std::clamp(volume, BGM_VOLUME_MIN, BGM_VOLUME_MAX); }
	// BGMボリューム割合を加算
	void AddBgmVolume(float volume) { bgmVolume = std::clamp(bgmVolume + volume, BGM_VOLUME_MIN, BGM_VOLUME_MAX); }

	// SEボリューム割合を代入（0～1）
	void SetSeVolume(float volume) { seVolume = std::clamp(volume, SE_VOLUME_MIN, SE_VOLUME_MAX); }
	// SEボリューム割合を加算
	void AddSeVolume(float volume) { seVolume = std::clamp(seVolume + volume, SE_VOLUME_MIN, SE_VOLUME_MAX); }
#pragma endregion

private:

#pragma region 定数定義
	// テーブルの参照番号
	enum TABLE_SUBSCR {
		TABLE_ID,		// ID
		TABLE_PATH,		// パス
		TABLE_SCENE,	// 読み込むシーン
		TABLE_TYPE,		// タイプ
		TABLE_VOLUME,	// ボリューム
		TABLE_LOOP,		// ループ
		TABLE_DUPLI,	// 最大同時再生数
	};

	// 1度その音声を再生してから次に再生するまで最低でも必要なインターバル
	const int PLAY_INTERVAL_TIME = 5;
#pragma endregion

	// 初期化処理
	void Init(void);
	// 終了処理
	void Release(void);

#pragma region 構造体定義
	// 音声テーブル読み込み用構造体
	struct SoundTable
	{
		// パス
		std::string path = {};

		// 読み込みシーン
		std::string scene = {};

		// タイプ（BGM/SE）
		std::string type = {};

		// 個別の音量の割合
		float volume = 1.0f;

		// ループ再生できるか
		bool loop = false;

		// 最大同時再生数
		unsigned char maxDupli = 1;

		// 生成
		SoundTable(void) {}

		SoundTable(
			const std::string& path,
			const std::string& scene,
			const std::string& type,
			const std::string& volume,
			const std::string& loop,
			const std::string& maxDupli
		) :
			path(path),
			scene(scene),
			type(type),
			volume(std::stof(volume)),
			loop((loop == "TRUE") ? true : false),
			maxDupli((unsigned char)std::stoi(maxDupli))
		{
		}
	};

	// 音声データの種類ごとに１つ抱える情報の構造体
	struct SoundInfo
	{
		// 連続再生に制限をかけるためのインターバルカウンター
		int playInterval = 0;


	};

	// 音声データ構造体
	struct SoundData
	{
		// ハンドル番号
		int id = -1;

		// 再生中か
		bool play(void);

		// 一時停止中か
		bool paused = false;
	};
#pragma endregion

	// 外部ファイルからサウンドテーブルをそのまま読み込む際の格納先
	std::map<std::string, SoundTable>SOUND_TABLE;

	// 今読み込んでいる音声データの種類ごとに抱えるデータ
	std::map<std::string, SoundInfo>soundInfos;

	// 実際に今読み込んで使う音声データ
	std::map<std::string, std::vector<SoundData>>sounds;

#pragma region 音量

	// ベースの音量
	unsigned char masterVolume;
	// ベースの最大音量
	const unsigned char MASTER_VOLUME_MAX = 255;
	// ベースの最小音量
	const unsigned char MASTER_VOLUME_MIN = 0;

	// ベースの音量に対しての「BGM」の音量の割合
	float bgmVolume;
	// ベースの音量に対しての「BGM」の最大割合
	const float BGM_VOLUME_MAX = 1.0f;
	// ベースの音量に対しての「BGM」の最小割合
	const float BGM_VOLUME_MIN = 0.0f;

	// ベースの音量に対しての「SE」の音量の割合
	float seVolume;
	// ベースの音量に対しての「SE」の最大割合
	const float SE_VOLUME_MAX = 1.0f;
	// ベースの音量に対しての「SE」の最小割合
	const float SE_VOLUME_MIN = 0.0f;

#pragma endregion

#pragma region ユーティリティ
	// 読み込み関数
	void SoundInfoAdd(const std::pair<const std::string, SoundTable>& data);

	// ベースの音量とテーブルを参照して最終的な音量を返す
	int VolumeValue(std::string id);
#pragma endregion
};

using Snd = SoundManager;