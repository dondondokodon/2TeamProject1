#include "AudioManager.h"
#include "System/Audio.h"

AudioManager::AudioManager()
{
    audios.push_back(std::unique_ptr<AudioSource>(
        Audio::Instance().LoadAudioSource("Data/Sounds/Stage_BGM.mp3", Flag::eventName::StageBGM, true)
    ));

    audios.push_back(std::unique_ptr<AudioSource>(
        Audio::Instance().LoadAudioSource("Data/Sounds/result.mp3", Flag::eventName::ResultBGM, true)
    ));

    audios.push_back(std::unique_ptr<AudioSource>(
        Audio::Instance().LoadAudioSource("Data/Sounds/Roll.mp3", Flag::eventName::MirrorRotateSE, true)
    ));

    audios.push_back(std::unique_ptr<AudioSource>(
        Audio::Instance().LoadAudioSource("Data/Sounds/Goal.mp3", Flag::eventName::GoalSE, true)
    ));

    audios.push_back(std::unique_ptr<AudioSource>(
        Audio::Instance().LoadAudioSource("Data/Sounds/Goal_effect.mp3", Flag::eventName::GoalOpenSE, true)
    ));
}

//初期化
void AudioManager::Initialize()
{
	for(auto& audio : audios)
		audio->Initialize();
}

//更新
void AudioManager::Update(float elapsedTime)
{
	for(auto& audio : audios)
		audio->Update(elapsedTime);
}

//クリア
void AudioManager::Clear()
{
    audios.clear();
}