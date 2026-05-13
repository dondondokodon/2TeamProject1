#include "StageData2.h"
StageData2::StageData2():StageData("Data/Json/stage2.json")
{
	MyStage = std::make_unique<Stage>("Data/Model/Stage/tutorial_stage.mdl");
	MyStage->SetScale({ 0.3f,0.3f,0.3f });
}