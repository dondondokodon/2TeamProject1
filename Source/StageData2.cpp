#include "StageData2.h"
StageData2::StageData2():StageData("Data/Json/stage2.json")
{
	MyStage =std::make_unique<Stage>("Data/Model/Stage/stage_tutorial.mdl");
	MyStage->SetScale({1.0f,1.0f,1.0f });
}

StageData3::StageData3() :StageData("Data/Json/stage3.json")
{
	MyStage = std::make_unique<Stage>("Data/Model/Stage/main_stage.mdl");
	MyStage->SetScale({ 1.0f,1.0f,1.0f });
}

StageData4::StageData4() :StageData("Data/Json/stage4.json")
{
	MyStage = std::make_unique<Stage>("Data/Model/Stage/main_stage.mdl");
	MyStage->SetScale({ 1.0f,1.0f,1.0f });
}