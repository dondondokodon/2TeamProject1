#include "StageData1.h"


StageData1::StageData1():StageData("Data/Json/stage1.json")
{
	//ステージオブジェクトの配置データを追加
	//オブジェクトの種類、位置、回転、大きさ、レーザーの向き（レーザー以外は無視される）

	//自分自身のオブジェクト（ステージ）
	//MyStage = new Stage("Data/Model/Stage/ExampleStage.mdl");//前のやつ
	MyStage =std::make_unique<Stage>("Data/Model/Stage/tutorial_stage.mdl");
	MyStage->SetScale({ 0.3f,0.3f,0.3f });
	//MyStage->SetScale({ 1.0f,1.0f,1.0f });
}
