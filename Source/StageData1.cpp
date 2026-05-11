#include "StageData1.h"


StageData1::StageData1()
{
	//ステージオブジェクトの配置データを追加
	//オブジェクトの種類、位置、回転、大きさ、レーザーの向き（レーザー以外は無視される）
	objects.push_back({ ObjectType::Laser,{   7.5f, 1.0f,  25.0f },{ 0.0f , 0.0f , 0.0f },{ 0.5f,0.5f,0.5f },{ 0.0f, 0.0f, -1.0f} });
	objects.push_back({ ObjectType::Laser,{  -7.5f, 1.0f,  25.0f },{ 0.0f , 0.0f , 0.0f },{ 0.5f,0.5f,0.5f },{ 0.0f, 0.0f, -1.0f} });
	objects.push_back({ ObjectType::Laser,{  25.0f, 1.0f,   7.0f },{ 0.0f , 0.0f , 0.0f },{ 0.5f,0.5f,0.5f },{-1.0f, 0.0f,  0.0f} });
	objects.push_back({ ObjectType::Laser,{ -25.0f, 1.0f,  -7.0f },{ 0.0f , 0.0f , 0.0f },{ 0.5f,0.5f,0.5f },{ 1.0f, 0.0f,  0.0f} });
	objects.push_back({ ObjectType::IrradiationDevice,{ -5.0f,1.0f,-7.0f },{ 0.0f,DirectX::XMConvertToRadians(90.0f),0.0f},{0.5f,0.5f,0.5f}});
	objects.push_back({ ObjectType::Mirror,{  0.0f, 1.2f, -7.0f },{ 0.0f,0.0f,0.0f },{ 0.5f,0.5f,0.5f } });
	objects.push_back({ ObjectType::Mirror,{ -2.0f, 1.2f,  2.0f },{ 0.0f,0.0f,0.0f },{ 0.5f,0.5f,0.5f } });
	objects.push_back({ ObjectType::Box,{  3.5f, 0.5f, -10.5f },{ 0.0f, 0.0f, 0.0f},{ 1.0f, 1.0f, 1.0f } });
	objects.push_back({ ObjectType::Box,{ 11.5f, 0.5f, -10.5f },{ 0.0f, 0.0f, 0.0f},{ 1.0f, 1.0f, 1.0f } });
	objects.push_back({ ObjectType::Box,{ 10.5f, 0.5f,   7.5f },{ 0.0f, 0.0f, 0.0f},{ 1.0f, 1.0f, 1.0f } });
	objects.push_back({ ObjectType::Box,{  1.5f, 0.5f,  10.5f },{ 0.0f, 0.0f, 0.0f},{ 1.0f, 1.0f, 1.0f } });
	objects.push_back({ ObjectType::Box,{-10.5f, 0.5f,  10.5f },{ 0.0f, 0.0f, 0.0f},{ 1.0f, 1.0f, 1.0f } });
	objects.push_back({ ObjectType::Box,{-13.5f, 0.5f,   3.5f },{ 0.0f, 0.0f, 0.0f},{ 1.0f, 1.0f, 1.0f } });
	objects.push_back({ ObjectType::Goal,{-1.0f, 3.0f,  17.0f },{ 0.0f, 0.0f, 0.0f},{ 0.01f, 0.01f, 0.01f } });

	//デバッグ用
	objects.push_back({ ObjectType::Box,{0.5f, 0.5f, -5.5f },{ 0,0,0 },{ 1,1,1 } });
	objects.push_back({ ObjectType::Laser,{ 5,1,0 },{ 0 ,0 ,0 },{ 0.5f,0.5f,0.5f },{0, 0, 1} });

	//自分自身のオブジェクト（ステージ）
	//MyStage = new Stage("Data/Model/Stage/ExampleStage.mdl");//前のやつ
	MyStage = new Stage("Data/Model/Stage/tutorial_stage.mdl");
	MyStage->SetScale({ 0.3f,0.3f,0.3f });
	//MyStage->SetScale({ 1.0f,1.0f,1.0f });
}
