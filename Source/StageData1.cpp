#include "StageData1.h"

StageData1::StageData1()
{
	//ステージオブジェクトの配置データを追加
	objects.push_back({ ObjectType::Laser,{ 5,1,0 },{ 0 ,0 ,0 },{ 1,1,1 },{1, 0, 1} });
	objects.push_back({ ObjectType::IrradiationDevice,{ 5,0,10 },{ 0 ,0 ,0 },{  0.5f,0.5f,0.5f } });

	//自分自身のオブジェクト（ステージ）
	MyStage = new Stage("Data/Model/Stage/ExampleStage.mdl");
}