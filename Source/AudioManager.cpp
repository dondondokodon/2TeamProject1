#include "AudioManager.h"

AudioManager::AudioManager()
{
	audios.push_back(std::make_unique<AudioSource>("Data/"));
}