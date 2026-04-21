#pragma once
#pragma once

#include "System/Model.h"

class AnimationController
{
public:
	AnimationController();
	~AnimationController() = default;

	// アニメーション再生
	void PlayAnimation(int index, bool loop);
	void PlayAnimation(const char* name, bool loop);

	// アニメーション更新処理
	void UpdateAnimation(float elapsedTime);

	//modelのセット
	void setModel(Model* model) { this->model = model; }

private:
	Model*				model;
	int									animationIndex = -1;
	float								animationSeconds = 0.0f;
	bool								animationLoop = false;
	bool								animationPlaying = false;
	float								animationBlendSecondsLength = 0.2f;

	//enum class State
	//{
	//	Idle,
	//	Run,
	//	Jump,
	//};
	//State								state = State::Idle;
};
