#include "IrradiationDevice.h"
#include<imgui.h>

void IrradiationDevice::Update(float elapsedTime)
{
	//オブジェクトの更新処理
	isHit = false;	//毎フレームリセットして、ヒット通知があったフレームだけtrueになるようにする
}

void IrradiationDevice::DrawDebugGUI()
{
	if (ImGui::Begin("hitObject", nullptr, ImGuiWindowFlags_None))
	{
		//トランスフォーム
		if (ImGui::CollapsingHeader("isHit", ImGuiTreeNodeFlags_DefaultOpen))
		{
			//位置
			ImGui::CheckboxFlags("isHit", (unsigned int*)&isHit, true);
		}
	}
	ImGui::End();
}