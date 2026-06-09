#include "StageData2.h"
StageData2::StageData2():StageData("Data/Json/stage2.json")
{
	MyStage = std::make_unique<Stage>("Data/Model/Stage/tutorial_wall.mdl");
	MyStage->SetScale({1.0f,1.0f,1.0f });
	StageFloor = std::make_unique<Stage>("Data/Model/Stage/tutorial_floor.mdl");
}

StageData3::StageData3() :StageData("Data/Json/stage3.json")
{
	MyStage = std::make_unique<Stage>("Data/Model/Stage/main_stage_wall.mdl");
	MyStage->SetScale({ 1.0f,1.0f,1.0f });
	StageFloor = std::make_unique<Stage>("Data/Model/Stage/main_stage_floor.mdl");
}

StageData4::StageData4() :StageData("Data/Json/stage4.json")
{
	MyStage = std::make_unique<Stage>("Data/Model/Stage/main_stage_wall.mdl");
	MyStage->SetScale({ 1.0f,1.0f,1.0f });
	StageFloor = std::make_unique<Stage>("Data/Model/Stage/main_stage_floor.mdl");
}

StageData5::StageData5() :StageData("Data/Json/stage5.json")
{
	MyStage = std::make_unique<Stage>("Data/Model/Stage/main_stage_wall.mdl");
	MyStage->SetScale({ 1.0f,1.0f,1.0f });
	StageFloor = std::make_unique<Stage>("Data/Model/Stage/main_stage_floor.mdl");
}

StageData6::StageData6() :StageData("Data/Json/stage6.json")
{
	MyStage = std::make_unique<Stage>("Data/Model/Stage/main_stage_wall.mdl");
	MyStage->SetScale({ 1.0f,1.0f,1.0f });
	StageFloor = std::make_unique<Stage>("Data/Model/Stage/main_stage_floor.mdl");
}

StageData7::StageData7() :StageData("Data/Json/stage7.json")
{
	MyStage = std::make_unique<Stage>("Data/Model/Stage/main_stage_wall.mdl");
	MyStage->SetScale({ 1.0f,1.0f,1.0f });
	StageFloor = std::make_unique<Stage>("Data/Model/Stage/main_stage_floor.mdl");
}