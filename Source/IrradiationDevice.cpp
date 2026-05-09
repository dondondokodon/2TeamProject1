#include "IrradiationDevice.h"
#include<imgui.h>
#include"Flag.h"

void IrradiationDevice::Update(float elapsedTime)
{
    //フラグの更新
    Flag::Instance().SetFlag(Flag::Instance().openGoal,isHit);

	//オブジェクトの更新処理
	isHit = false;	//毎フレームリセットして、ヒット通知があったフレームだけtrueになるようにする

    UpdateTransform();
}

RayHitResult IrradiationDevice::ReallyHit(DirectX::XMFLOAT3 dir, DirectX::XMFLOAT3 hitPos, DirectX::XMFLOAT3 hitNormal)
{
    // ローカル座標に変換
    DirectX::XMMATRIX invWorld = DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&transform));    //逆行列
    DirectX::XMVECTOR localHitPos = XMVector3TransformCoord(XMLoadFloat3(&hitPos), invWorld);               //逆行列をかけ合わせる

    DirectX::XMFLOAT3 localPos;
    DirectX::XMStoreFloat3(&localPos, localHitPos);

    // 奥行き(Z)を無視し、XとYの平面距離だけで判定する
    // これにより、中心を通る「透明な筒」にレーザーが触れていればOKになる
    float distSq = localPos.x * localPos.x + localPos.y * localPos.y;

    RayHitResult result{ true,this,type,hitPos };

    float radius = 1.0f;
    if (distSq < (radius * radius)) {
        // 前後の制限
         if (abs(localPos.z) < 1.0f) 
        result.hit= true;
    }
    else
    result.hit = false;
    return result;
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