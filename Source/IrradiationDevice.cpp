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
    //まずは今まで通りローカル座標での円柱判定
    DirectX::XMMATRIX invWorld = DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&transform));
    DirectX::XMVECTOR localHitPos = XMVector3TransformCoord(XMLoadFloat3(&hitPos), invWorld);
    DirectX::XMFLOAT3 localPos;
    DirectX::XMStoreFloat3(&localPos, localHitPos);

    float distSq = localPos.x * localPos.x + localPos.y * localPos.y;
    float radius = 0.5f;

    RayHitResult result{ false, this, type, hitPos }; // デフォルトはfalse

    //円柱の範囲内かチェック
    if (distSq < (radius * radius)) {
        // 向きの判定を追加
        // デバイスの正面方向（ワールドZ軸）を取得
        DirectX::XMMATRIX world = DirectX::XMLoadFloat4x4(&transform);
        DirectX::XMVECTOR deviceForward = world.r[2]; // 3行目がZ軸（Forward）

        // レーザーの進行方向
        DirectX::XMVECTOR laserDir = DirectX::XMLoadFloat3(&dir);

        // 内積を計算（向き合っていればマイナスになる）
        float dot;
        DirectX::XMStoreFloat(&dot, DirectX::XMVector3Dot(deviceForward, laserDir));

        // 内積がマイナス（向き合っている）かつ、Zが正面側にある場合のみヒット
        if (dot < 0.3f) {
            if (localPos.z < 0.0f) { 
                result.hit = true;
            }
        }
    }
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