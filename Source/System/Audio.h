#pragma once

#include <xaudio2.h>
#include "System/AudioSource.h"

#include "Flag.h"

// オーディオ
class Audio
{
private:
	Audio() = default;
	~Audio() = default;

public:
	// インスタンス取得
	static Audio& Instance()
	{
		static Audio instance;
		return instance;
	}

	// 初期化
	void Initialize();

	// 終了化
	void Finalize();

	// オーディオソース読み込み
	AudioSource* LoadAudioSource(const char* filename);
	AudioSource* LoadAudioSource(const char* filename,Flag::eventName name,bool loop);

private:
	IXAudio2*				xaudio = nullptr;
	IXAudio2MasteringVoice* masteringVoice = nullptr;
};
