#include "AnimationController.h"
#include "System/Graphics.h"


AnimationController::AnimationController()
{ 

}


// アニメーション再生
void AnimationController::PlayAnimation(int index, bool loop)
{
	if (!model) return;

	// 同じアニメーションを再生中なら、最初から再生し直さない
	if (animationPlaying && animationIndex == index)
	{
		return;
	}

	// 切り替え前の現在姿勢を保存する
	animationBlendStartNodeKeys.clear();

	for (const Model::Node& node : model->GetNodes())
	{
		ModelResource::NodeKeyData key;
		key.scale = node.scale;
		key.rotate = node.rotate;
		key.translate = node.translate;
		animationBlendStartNodeKeys.push_back(key);
	}

	animationBlending = !animationBlendStartNodeKeys.empty();
	animationBlendSeconds = 0.0f;

	animationPlaying = true;
	animationLoop = loop;
	animationIndex = index;
	animationSeconds = 0.0f;
}

void AnimationController::PlayAnimation(const char* name, bool loop)
{
	int index = 0;
	const std::vector<ModelResource::Animation>& animations = model->GetResource()->GetAnimations();
	for (const ModelResource::Animation& animation : animations)
	{
		if (animation.name == name)
		{
			PlayAnimation(index, loop);
			return;
		}
		++index;
	}
}


// アニメーション更新処理
void AnimationController::UpdateAnimation(float elapsedTime)
{
	if (animationPlaying)
	{
		std::vector<Model::Node>& nodes = model->GetNodes();

		//指定のアニメーションデータを取得
		const std::vector<ModelResource::Animation>& animations = model->GetResource()->GetAnimations();
		const ModelResource::Animation& animation = animations.at(animationIndex);

		//時間経過
		animationSeconds += elapsedTime;

		//アニメーションブレンド中なら、ブレンド時間を加算
		if (animationBlending)
		{
			animationBlendSeconds += elapsedTime;

			if (animationBlendSeconds >= animationBlendSecondsLength)
			{
				animationBlending = false;
			}
		}

		//再生時間が終端時間を超えたら
		if (animationSeconds >= animation.secondsLength)
		{
			if (animationLoop)
			{
				animationSeconds = 0.0f;
			}
			else
			{
				animationPlaying = false;
				animationSeconds = animation.secondsLength;
			}
		}

		//アニメーション切り替え時のブレンド率を計算
		float blendRate = 1.0f;
		if (animationSeconds < animationBlendSecondsLength)
		{
			blendRate = animationSeconds / animationBlendSecondsLength;
			if (blendRate > 1.0f) blendRate = 1.0f;
		}

		//アニメーションデータからキーフレームデータリストを取得
		const std::vector<ModelResource::Keyframe>& keyframes = animation.keyframes;
		int keyCount = static_cast<int>(keyframes.size());
		for (int keyIndex = 0;keyIndex < keyCount - 1;++keyIndex)
		{
			//現在の時間がどのキーフレームの間にいるか判定する
			const ModelResource::Keyframe& keyframe0 = keyframes.at(keyIndex);
			const ModelResource::Keyframe& keyframe1 = keyframes.at(keyIndex + 1);
			if (animationSeconds >= keyframe0.seconds && animationSeconds < keyframe1.seconds)
			{
				//再生時間とキーフレームの時間から補完率を算出
				float rate = (animationSeconds - keyframe0.seconds) / (keyframe1.seconds - keyframe0.seconds);

				//全てのノードの姿勢を計算する
				int nodeCount = static_cast<int>(nodes.size());
				for (int nodeIndex = 0;nodeIndex < nodeCount;++nodeIndex)
				{
					//キーフレームデータ取得
					const ModelResource::NodeKeyData& key0 = keyframe0.nodeKeys.at(nodeIndex);
					const ModelResource::NodeKeyData& key1 = keyframe1.nodeKeys.at(nodeIndex);

					//ノード取得
					Model::Node& node = nodes[nodeIndex];

					// 現在のアニメーション時間に対応する姿勢を計算する
					DirectX::XMVECTOR S0 = DirectX::XMLoadFloat3(&key0.scale);
					DirectX::XMVECTOR S1 = DirectX::XMLoadFloat3(&key1.scale);
					DirectX::XMVECTOR R0 = DirectX::XMLoadFloat4(&key0.rotate);
					DirectX::XMVECTOR R1 = DirectX::XMLoadFloat4(&key1.rotate);
					DirectX::XMVECTOR T0 = DirectX::XMLoadFloat3(&key0.translate);
					DirectX::XMVECTOR T1 = DirectX::XMLoadFloat3(&key1.translate);

					DirectX::XMVECTOR S = DirectX::XMVectorLerp(S0, S1, rate);
					DirectX::XMVECTOR R = DirectX::XMQuaternionSlerp(R0, R1, rate);
					DirectX::XMVECTOR T = DirectX::XMVectorLerp(T0, T1, rate);

					// アニメーション切り替え直後だけ、前の姿勢から新しいアニメ姿勢へ補間する
					if (animationBlending && nodeIndex < animationBlendStartNodeKeys.size())
					{
						float blendRate = animationBlendSeconds / animationBlendSecondsLength;
						if (blendRate > 1.0f) blendRate = 1.0f;

						const ModelResource::NodeKeyData& blendStart = animationBlendStartNodeKeys[nodeIndex];

						DirectX::XMVECTOR BS = DirectX::XMLoadFloat3(&blendStart.scale);
						DirectX::XMVECTOR BR = DirectX::XMLoadFloat4(&blendStart.rotate);
						DirectX::XMVECTOR BT = DirectX::XMLoadFloat3(&blendStart.translate);

						S = DirectX::XMVectorLerp(BS, S, blendRate);
						R = DirectX::XMQuaternionSlerp(BR, R, blendRate);
						T = DirectX::XMVectorLerp(BT, T, blendRate);
					}

					// 計算結果をボーンに格納
					DirectX::XMStoreFloat3(&node.scale, S);
					DirectX::XMStoreFloat4(&node.rotate, R);
					DirectX::XMStoreFloat3(&node.translate, T);
				}
				break;
			}
		}
	}
	//モデル行列更新
	model->UpdateTransform();
}

