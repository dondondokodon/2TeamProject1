#include "Misc.h"
#include "System/AudioSource.h"

// コンストラクタ
AudioSource::AudioSource(IXAudio2* xaudio, std::shared_ptr<AudioResource>& resource)
	: resource(resource)
{
	HRESULT hr;

	// ソースボイスを生成
	hr = xaudio->CreateSourceVoice(&sourceVoice, &resource->GetWaveFormat());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
}

AudioSource::AudioSource(IXAudio2* xaudio, std::shared_ptr<AudioResource>& resource, Flag::eventName name, bool loop)
	: resource(resource),name(name),loop(loop)
{
	HRESULT hr;

	// ソースボイスを生成
	hr = xaudio->CreateSourceVoice(&sourceVoice, &resource->GetWaveFormat());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
}


// デストラクタ
AudioSource::~AudioSource()
{
	if (sourceVoice != nullptr)
	{
		sourceVoice->DestroyVoice();
		sourceVoice = nullptr;
	}
}

// 再生
void AudioSource::Play(bool loop)
{
	Stop();

	// ソースボイスにデータを送信
	XAUDIO2_BUFFER buffer = { 0 };
	buffer.AudioBytes = resource->GetAudioBytes();
	buffer.pAudioData = resource->GetAudioData();
	buffer.LoopCount = loop ? XAUDIO2_LOOP_INFINITE : 0;
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	
	sourceVoice->SubmitSourceBuffer(&buffer);

	HRESULT hr = sourceVoice->Start();
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	sourceVoice->SetVolume(1.0f);
}

// 停止
void AudioSource::Stop()
{
	sourceVoice->FlushSourceBuffers();
	sourceVoice->Stop();
}

// 音量設定
void AudioSource::SetVolume(float volume)
{
	sourceVoice->SetVolume(volume);
}

//更新処理
void AudioSource::Update(float elapsedTime)
{
	bool flag = Flag::Instance().getFlag(name);
	if (flag&&!oldPlay)
	{
		Play(loop);
	}
	else if(flag)
		Stop();
	
	oldPlay = flag;
}

//初期化
void AudioSource::Initialize()
{
	oldPlay = false;
}