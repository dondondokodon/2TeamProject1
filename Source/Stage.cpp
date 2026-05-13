#include"Stage.h"

//コンストラクタ
Stage::Stage(const char* filename)
{
	//ステージモデルを読み込み
	model = std::make_unique<Model>(filename);
}

Stage::~Stage()
{
}

void Stage::Update(float elapsedTime)
{
	//今は特にやることはない
}

//描画処理
//void Stage::Render(const RenderContext& rc, ModelRenderer* renderer)
//{
//	DirectX::XMFLOAT4X4 transform;
//	DirectX::XMStoreFloat4x4(&transform, DirectX::XMMatrixIdentity());
//
//	//レンダラにモデルを描画してもらう
//	renderer->Render(rc, transform, model.get(), ShaderId::Lambert);	//shaderIdは見た目の設定 Lambertは一般的な陰影表現 transformは3Dモデルを表示する位置や姿勢の情報
//}