#include "StageData.h"
#include "Laser.h"
#include "IrradiationDevice.h"
#include "StageGrid.h"
#include<memory>

StageObject* ObjectData::CreateStageObject()const
{
	StageObject* obj = nullptr;
	switch (type)
	{
	case ObjectType::Box:
		obj = new StageGrid();
		break;
	case ObjectType::Mirror:
		//ミラーオブジェクトの生成と初期化
		break;
	case ObjectType::Goal:
		//ゴールオブジェクトの生成と初期化
		break;
	case ObjectType::IrradiationDevice:
		//照射装置オブジェクトの生成と初期化
		obj = new IrradiationDevice();
		break;
	case ObjectType::Laser:
	{
		//レーザーオブジェクトの生成と初期化
		obj = new Laser();
		Laser* laser = dynamic_cast<Laser*>(obj);
		if (laser)
		{
			laser->Initialize(pos, direction, 30.0f);
		}
		break;
	}
	default:
		break;
	}
	obj->SetPosition(pos);
	obj->SetAngle(angle);
	obj->SetScale(scale);
	return obj;
}