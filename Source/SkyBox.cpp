#include "SkyBox.h"

void SkyBox::Update(float elapsedTime)
{

}

//描画処理
void SkyBox::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	if (!renderer) return;
	if (!model)   return;

	//レンダラにモデルを描画してもらう
	renderer->Render(rc, transform, model.get(), ShaderId::Lambert);	//shaderIdは見た目の設定 Lambertは一般的な陰影表現 transformは3Dモデルを表示する位置や姿勢の情報
}

//行列更新処理
void SkyBox::UpdateTransform()
{
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMMATRIX W = S * R * T;
	DirectX::XMStoreFloat4x4(&transform, W);
}