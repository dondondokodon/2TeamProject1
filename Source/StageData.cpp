#include "StageData.h"
#include "Laser.h"
#include "IrradiationDevice.h"
#include "StageGrid.h"
#include "Mirror.h"
#include "GoalObject.h"
#include "Stairs.h"
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
		obj = new Mirror();
		break;
	case ObjectType::Goal:
		//ゴールオブジェクトの生成と初期化
		obj = new GoalObject();
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
			laser->Initialize(pos, direction, 150.0f);
		}
		break;
	
	}
	case ObjectType::Stairs:
		//階段オブジェクトの生成と初期化
	    obj = new Stairs();
	    break;
	default:
		break;
	}
	obj->SetPosition(pos);
	obj->SetAngle(angle);
	obj->SetScale(scale);
	return obj;
}

#include <fstream>
#include<string>
#include "nlohmann/json.hpp" // JSONライブラリ

using json = nlohmann::json;



StageData::StageData(const char* filename)
{
	// 1. ファイルを開く
	std::ifstream ifs(filename);
	if (!ifs.is_open()) {
		return; // ファイルが開けなければ終了
	}

	// 2. JSONとして直接解析 (取り出し)
	json data;
	ifs >> data; // ストリームから直接JSONオブジェクトへ流し込む

	// 3. データをループで取り出して objects に追加
	for (auto& obj : data)
	{
		ObjectType o;
		std::string type = obj.value("type", "");

		//タイプ
		if (type == "Laser")
			o = ObjectType::Laser;
		else if (type == "Device")
			o = ObjectType::IrradiationDevice;
		else if (type == "Mirror")
			o = ObjectType::Mirror;
		else if (type == "Box")
			o = ObjectType::Box;
		else if (type == "Goal")
			o = ObjectType::Goal;
		else
			continue;	//例外

		//ポジション
		DirectX::XMFLOAT3 position = {
			obj.value("PosX",0.0f),
			obj.value("PosY",0.0f),
			obj.value("PosZ",0.0f)
		};

		//アングル
		DirectX::XMFLOAT3 angle = {
			DirectX::XMConvertToRadians(obj.value("angleX",0.0f)),
			DirectX::XMConvertToRadians(obj.value("angleY",0.0f)),
			DirectX::XMConvertToRadians(obj.value("angleZ",0.0f)),
		};

		//スケール
		DirectX::XMFLOAT3 scale = {
			obj.value("ScaleX",1.0f),
			obj.value("ScaleY",1.0f),
			obj.value("ScaleZ",1.0f)
		};

		//向き
		DirectX::XMFLOAT3 direction = {
			obj.value("DirectionX",0.0f),
			obj.value("DirectionY",0.0f),
			obj.value("DirectionZ",0.0f)
		};

		objects.push_back(new ObjectData{ o,position,angle,scale,direction });

	}
}