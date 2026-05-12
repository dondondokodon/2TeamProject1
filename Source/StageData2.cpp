#include "StageData2.h"

StageData2::StageData2()
{
	//ステージオブジェクトの配置データを追加
	//オブジェクトの種類、位置、回転、大きさ、レーザーの向き（レーザー以外は無視される）
	objects.push_back({ ObjectType::Laser,{   7.5f, 1.0f,  25.0f },{ 0.0f , 0.0f , 0.0f },{ 0.5f,0.5f,0.5f },{ 0.0f, 0.0f, -1.0f} });
	objects.push_back({ ObjectType::Laser,{  -7.5f, 1.0f,  25.0f },{ 0.0f , 0.0f , 0.0f },{ 0.5f,0.5f,0.5f },{ 0.0f, 0.0f, -1.0f} });
	objects.push_back({ ObjectType::Laser,{  25.0f, 1.0f,   6.0f },{ 0.0f , 0.0f , 0.0f },{ 0.5f,0.5f,0.5f },{-1.0f, 0.0f,  0.0f} });
	objects.push_back({ ObjectType::Laser,{ -25.0f, 1.0f,  -7.0f },{ 0.0f , 0.0f , 0.0f },{ 0.5f,0.5f,0.5f },{ 1.0f, 0.0f,  0.0f} });
	objects.push_back({ ObjectType::IrradiationDevice,{ -7.5f,1.0f,7.0f },{ 0.0f,DirectX::XMConvertToRadians(180.0f),0.0f},{0.5f,0.5f,0.5f} });
	objects.push_back({ ObjectType::Box,{  -12.5f, 0.5f, 1.5f },{ 0.0f, 0.0f, 0.0f},{ 1.0f, 1.0f, 1.0f } });
	objects.push_back({ ObjectType::Box,{  -11.5f, 0.5f, 11.5f },{ 0.0f, 0.0f, 0.0f},{ 1.0f, 1.0f, 1.0f } });
	objects.push_back({ ObjectType::Box,{  -3.5f, 0.5f, -2.5f },{ 0.0f, 0.0f, 0.0f},{ 1.0f, 1.0f, 1.0f } });
	objects.push_back({ ObjectType::Box,{  10.5f, 0.5f, 6.5f },{ 0.0f, 0.0f, 0.0f},{ 1.0f, 1.0f, 1.0f } });
	objects.push_back({ ObjectType::Box,{  12.5f, 0.5f, 0.5f },{ 0.0f, 0.0f, 0.0f},{ 1.0f, 1.0f, 1.0f } });
	objects.push_back({ ObjectType::Box,{  11.5f, 0.5f, -6.5f },{ 0.0f, 0.0f, 0.0f},{ 1.0f, 1.0f, 1.0f } });
	objects.push_back({ ObjectType::Mirror,{  -2.5f, 1.2f, 3.0f },{ 0.0f, 0.0f, 0.0f},{ 0.5f, 0.5f, 0.5f } });
	objects.push_back({ ObjectType::Mirror,{  4.5f, 1.2f, 0.5f },{ 0.0f, 0.0f, 0.0f},{ 0.5f, 0.5f, 0.5f } });
	objects.push_back({ ObjectType::Goal,{-1.0f, 3.0f,  17.0f },{ 0.0f, 0.0f, 0.0f},{ 0.01f, 0.01f, 0.01f } });


	//自分自身のオブジェクト（ステージ）
	MyStage = new Stage("Data/Model/Stage/tutorial_stage.mdl");
	MyStage->SetScale({ 0.3f,0.3f,0.3f });
}