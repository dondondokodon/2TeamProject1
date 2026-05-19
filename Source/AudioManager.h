#pragma once
#include "System/AudioSource.h"
#include <vector>
#include <memory>

class AudioManager
{
	AudioManager();
	~AudioManager() { Clear(); };

public:
	static AudioManager& Instance()
	{
		static AudioManager instance;
		return instance;
	}

	//初期化
	void Initialize();

	//更新処理
	void Update(float elapsedTime);

	//ステージオブジェクト全削除
	void Clear();

private:
	std::vector<std::unique_ptr<AudioSource>> audios;
};

