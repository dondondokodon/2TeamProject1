#include "StageData1.h"


StageData1::StageData1()
{
	//ステージオブジェクトの配置データを追加
	//オブジェクトの種類、位置、回転、大きさ、レーザーの向き（レーザー以外は無視される）
	objects.push_back({ ObjectType::Laser,{ 5,1,0 },{ 0 ,0 ,0 },{ 0.5f,0.5f,0.5f },{0, 0, 1} });
	objects.push_back({ ObjectType::IrradiationDevice,{ -4.5f,1,12 },{ 0,DirectX::XMConvertToRadians(180),0},{0.5f,0.5f,0.5f}});
	objects.push_back({ ObjectType::Mirror,{ 3.5f, 1.2f, 16.0f },{ 0,0,0 },{ 1,1,1 } });
	objects.push_back({ ObjectType::Box,{0.5f, 0.5f, -5.5f },{ 0,0,0 },{ 1,1,1 } });
	objects.push_back({ ObjectType::Goal,{5, 1.5, 25.5f },{ 0,0,0 },{ 0.01f,0.01f,0.01f } });

	//自分自身のオブジェクト（ステージ）
	//MyStage = new Stage("Data/Model/Stage/ExampleStage.mdl");//前のやつ
	MyStage = new Stage("Data/Model/Stage/tutorial_stage.mdl");
	MyStage->SetScale({ 0.5f,0.5f,0.5f });
}