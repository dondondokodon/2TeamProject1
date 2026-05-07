#include "StageData1.h"

StageData1::StageData1()
{
	//ステージオブジェクトの配置データを追加
	objects.push_back({ ObjectType::Stage, { 0,0,0 },{ 0,0,0 },{ 1,1,1 } });
	objects.push_back({ ObjectType::Box, { 0.5f,0.5f,-5.5f },{ 0,0,0 },{ 1,1,1 } });
	objects.push_back({ ObjectType::Box, { -2.5f,0.5f,-8.5f },{ 0,DirectX::XM_PI / 4.0f,0 },{ 1,1,1 } });
	objects.push_back({ ObjectType::Box, { 2.5f,0.5f,-8.5f },{ 0,-DirectX::XM_PI / 4.0f,0 },{ 1,1,1 } });
	objects.push_back({ ObjectType::Mirror,{ -2.5f,0.5f,-12.5f },{ 0,-DirectX::XM_PI / 4.0f ,0 },{ 1,1,1 } });
	objects.push_back({ ObjectType::Mirror,{ 2.5f,0.5f,-12.5f },{ 0 ,DirectX::XM_PI / 4.0f ,0 },{ 1,1,1 } });
	objects.push_back({ ObjectType::Goal,{ 0,0.5f,-15.5f },{ 0 ,DirectX::XM_PI / 4.0f ,0 },{ 1,1,1 } });
}