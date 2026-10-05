#include "SoundManager.h"

#include "../../pch.h"

#include<sstream>
#include<fstream>
#include<iostream>

#include"../../Utility/Utility.h"

SoundManager* SoundManager::ins = nullptr;

SoundManager::SoundManager() :
	masterVolume(255),
	bgmVolume(1.0f),
	seVolume(1.0f)
{
}

void SoundManager::Init(void)
{
#pragma region サウンドテーブルを読み込む
	// CSVファイルを開く
	std::ifstream ifs = std::ifstream("Data/Sound/SoundTable.csv");

	// 例外処理：ファイルが開けなかった場合
	if (!ifs) { std::cerr << "サウンドデータの読み込みに失敗しました。" << '\n'; }

	// 1行の代入先
	std::string line;

	// 1行ずつカンマ区切りにされた状態で読み込む
	while (getline(ifs, line)) {
		// カンマごとに分けて文字列として配列に格納
		std::vector<std::string> table = Split(line, ',');

		// 一旦一時変数にデータを取りまとめる
		SoundTable data(
			table[TABLE_PATH],
			table[TABLE_SCENE],
			table[TABLE_TYPE],
			table[TABLE_VOLUME],
			table[TABLE_LOOP],
			table[TABLE_DUPLI]
		);

		// IDを添え字にしてデータを格納
		SOUND_TABLE[table[TABLE_ID]] = data;
	}
#pragma endregion

#pragma region 読み込みシーンAllのものは最初に読み込んでおく

	for (std::pair<const std::string, SoundTable>& data : SOUND_TABLE) {
		// 「All」以外はスキップ
		if (data.second.scene != "All") { continue; }

		// 音声を追加する
		SoundInfoAdd(data);
	}

#pragma endregion
}

void SoundManager::Release(void)
{
	// 現状抱えているデータを全て破棄する
	for (std::pair<const std::string, std::vector<SoundData>>& sound : sounds) {
		for (SoundData& s : sound.second) { DeleteSoundMem(s.id); }
		sound.second.clear();
	}
	sounds.clear();

	SOUND_TABLE.clear();
}

void SoundManager::Update(void)
{
	for (auto& info : soundInfos) {
		if (info.second.playInterval > 0) { info.second.playInterval--; }
		else if (info.second.playInterval < 0) { info.second.playInterval = 0; }
	}
}

void SoundManager::ChangeScene(const std::string& scene)
{
#pragma region All以外を破棄する
	// 削除した配列要素数の添え字を記憶しておくための一時変数
	std::vector<std::string> deleteSubscrs = {};
	deleteSubscrs.reserve(sounds.size());

	// まず現状抱えているサウンドデータのなかの「All」以外のもののリソースを解放する
	for (std::pair<const std::string, std::vector<SoundData>>& sound : sounds) {
		if (SOUND_TABLE.at(sound.first).scene == "All") { continue; }

		for (SoundData& info : sound.second) { DeleteSoundMem(info.id); info.id = -1; }

		sound.second.clear();

		// 何を消したか記憶する
		deleteSubscrs.emplace_back(sound.first);
	}
	// 消したものを完全に配列から削除する
	for (std::string& subscr : deleteSubscrs) {
		soundInfos.erase(subscr);
		sounds.erase(subscr);
	}

	// 一時変数をクリア
	deleteSubscrs.clear();
#pragma endregion

	AddScene(scene);
}

void SoundManager::AddScene(const std::string& scene)
{

#pragma region 指定されたシーンに対応するサウンドを読み込む
	// テーブルを探索する
	for (std::pair<const std::string, SoundTable>& data : SOUND_TABLE) {
		// 指定のもの以外はスキップ
		if (data.second.scene != scene) { continue; }

		// 読み込んでデータ追加
		SoundInfoAdd(data);
	}
#pragma endregion
}

void SoundManager::Play(const std::string& id)
{
	// テーブルにIDがなかったらエラー通知して終了
	if (!SOUND_TABLE.contains(id)) {
		std::cerr << "指定のIDが見つからない為、音声を再生できませんでした" << '\n';
		return;
	}
	// サウンドデータがあるか確認する
	if (!soundInfos.contains(id) || !sounds.contains(id)) {
		std::cerr << "指定のIDの音声が正常に読み込まれていなかった為、音声を再生できませんでした" << '\n';
		return;
	}

	// インターバル
	if (soundInfos.at(id).playInterval > 0) { return; }

	// 配列を探索する
	for (SoundData& info : sounds.at(id)) {
		// 再生中だったら次のを確認しに行く
		if (info.play()) { continue; }

		// 音量設定
		ChangeVolumeSoundMem(VolumeValue(id), info.id);

		// ループ再生するかどうか
		auto loop = (SOUND_TABLE.at(id).loop) ? DX_PLAYTYPE_LOOP : DX_PLAYTYPE_BACK;

		// 再生
		PlaySoundMem(info.id, loop, true);

		// 再生したのでインターバルをセット
		soundInfos.at(id).playInterval = PLAY_INTERVAL_TIME;

		break;
	}
}

void SoundManager::Stop(const std::string& id)
{
	// テーブルにIDがなかったらエラー通知して終了
	if (!SOUND_TABLE.contains(id)) {
		std::cerr << "指定のIDが見つからない為、音声を停止できませんでした" << '\n';
		return;
	}
	// サウンドデータがあるか確認する
	if (!soundInfos.contains(id) || !sounds.contains(id)) {
		std::cerr << "指定のIDの音声が正常に読み込まれていなかった為、音声を停止できませんでした" << '\n';
		return;
	}

	// 配列を探索する
	for (SoundData& info : sounds.at(id)) {
		// 再生中じゃなかったら次のを確認しに行く
		if (!info.play()) { continue; }

		// 停止
		StopSoundMem(info.id);

		info.paused = false;
	}
}

void SoundManager::AllStop(void)
{
	// 配列を探索する
	for (std::pair<const std::string, std::vector<SoundData>>& sound : sounds) {
		for (SoundData& info : sound.second) {
			// 再生中じゃなかったら次のを確認しに行く
			if (!info.play()) { continue; }

			// 停止
			StopSoundMem(info.id);

			info.paused = false;
		}
	}
}

void SoundManager::Pause(void)
{
	// 配列を探索する
	for (std::pair<const std::string, std::vector<SoundData>>& sound : sounds) {
		for (SoundData& info : sound.second) {
			// 再生中じゃなかったら次のを確認しに行く
			if (!info.play()) { continue; }

			// 停止
			StopSoundMem(info.id);

			info.paused = true;
		}
	}
}

void SoundManager::PausePlay(void)
{
	// 配列を探索する
	for (std::pair<const std::string, std::vector<SoundData>>& sound : sounds) {
		for (SoundData& info : sound.second) {
			if (!info.paused) { continue; }

			// ループ再生するかどうか
			auto loop = (SOUND_TABLE.at(sound.first).loop) ? DX_PLAYTYPE_LOOP : DX_PLAYTYPE_BACK;

			// 途中から再生
			PlaySoundMem(info.id, loop, false);
		}
	}
}

void SoundManager::SoundInfoAdd(const std::pair<const std::string, SoundTable>& data)
{
	// その場所にすでに要素数があれば何もしない
	if (soundInfos.contains(data.first) || sounds.contains(data.first)) { return; }

	// 明示的に配列を追加
	soundInfos[data.first] = {};
	sounds[data.first] = {};

	// 最大同時再生数の数だけ音声を読み込む と同時に初期化も行う
	for (unsigned char i = 0; i < data.second.maxDupli; i++) {

		// 一旦一時変数にデータを取りまとめる
		SoundData work = {};
		work.id = LoadSoundMem(data.second.path.c_str());
		work.paused = false;

		// 一時変数にまとめて生成した情報を配列に追加する
		sounds.at(data.first).emplace_back(work);
	}
}

int SoundManager::VolumeValue(std::string id)
{
	int ret = masterVolume;

	// BGM/SE どちらなのかテーブルから取ってくる
	std::string type = SOUND_TABLE.at(id).type;
	
	// タイプ別の音量の割合計算
	float volumeRatio = 1.0f;
	if (type == "BGM") { volumeRatio = bgmVolume; }
	else if (type == "SE") { volumeRatio = seVolume; }

	ret = (int)((float)ret * volumeRatio);

	// 個別の音量の割合計算
	volumeRatio = SOUND_TABLE.at(id).volume;

	ret = (int)((float)ret * volumeRatio);

	return ret;
}

bool SoundManager::SoundData::play(void) { return (CheckSoundMem(id) == 1); }