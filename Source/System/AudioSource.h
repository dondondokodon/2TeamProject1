#pragma once

#include <memory>
#include <xaudio2.h>
#include "System/AudioResource.h"

#include "Flag.h"
// オーディオソース
class AudioSource
{
public:
	AudioSource(IXAudio2* xaudio, std::shared_ptr<AudioResource>& resource);
	AudioSource(IXAudio2* xaudio, std::shared_ptr<AudioResource>& resource,Flag::eventName name,bool loop);
	~AudioSource();

	// 再生
	void Play(bool loop);

	// 停止
	void Stop();

	// 音量設定
	void SetVolume(float volume);

	//更新処理
	void Update(float elapsedTime);

	//セットフラグ
	void SetFlag(Flag::eventName name) { this->name = name; }

	//ループするかどうか
	void SetLoop(bool loop) { this->loop = loop; }

	//イニシャライズ
	void Initialize();

private:
	IXAudio2SourceVoice*			sourceVoice = nullptr;
	std::shared_ptr<AudioResource>	resource;

	Flag::eventName name;
	bool loop = false;
	bool oldPlay=false;
};
