#include "StageData2.h"
#include <fstream>
#include<string>
#include "nlohmann/json.hpp" // JSONライブラリ

using json = nlohmann::json;

StageData2::StageData2()
{
	//ステージオブジェクトの配置データを追加
	//オブジェクトの種類、位置、回転、大きさ、レーザーの向き（レーザー以外は無視される）
	/*objects.push_back(std::make_unique<ObjectData>(ObjectData{ ObjectType::Laser,{   7.5f, 1.0f,  25.0f },{ 0.0f , 0.0f , 0.0f },{ 0.5f,0.5f,0.5f },{ 0.0f, 0.0f, -1.0f} }));
	objects.push_back(std::make_unique<ObjectData>(ObjectData{ ObjectType::Laser,{  -7.5f, 1.0f,  25.0f },{ 0.0f , 0.0f , 0.0f },{ 0.5f,0.5f,0.5f },{ 0.0f, 0.0f, -1.0f} }));
	objects.push_back(std::make_unique<ObjectData>(ObjectData{ ObjectType::Laser,{  25.0f, 1.0f,   6.0f },{ 0.0f , 0.0f , 0.0f },{ 0.5f,0.5f,0.5f },{-1.0f, 0.0f,  0.0f} }));
	objects.push_back(std::make_unique<ObjectData>(ObjectData{ ObjectType::Laser,{ -25.0f, 1.0f,  -7.0f },{ 0.0f , 0.0f , 0.0f },{ 0.5f,0.5f,0.5f },{ 1.0f, 0.0f,  0.0f} }));
	objects.push_back(std::make_unique<ObjectData>(ObjectData{ ObjectType::IrradiationDevice,{ -7.5f,1.0f,7.0f },{ 0.0f,DirectX::XMConvertToRadians(180.0f),0.0f},{0.5f,0.5f,0.5f} }));
	objects.push_back(std::make_unique<ObjectData>(ObjectData{ ObjectType::Box,{  -12.5f, 0.5f, 1.5f },{ 0.0f, 0.0f, 0.0f},{ 1.0f, 1.0f, 1.0f } }));
	objects.push_back(std::make_unique<ObjectData>(ObjectData{ ObjectType::Box,{  -11.5f, 0.5f, 11.5f },{ 0.0f, 0.0f, 0.0f},{ 1.0f, 1.0f, 1.0f } }));
	objects.push_back(std::make_unique<ObjectData>(ObjectData{ ObjectType::Box,{  -3.5f, 0.5f, -2.5f },{ 0.0f, 0.0f, 0.0f},{ 1.0f, 1.0f, 1.0f } }));
	objects.push_back(std::make_unique<ObjectData>(ObjectData{ ObjectType::Box,{  10.5f, 0.5f, 6.5f },{ 0.0f, 0.0f, 0.0f},{ 1.0f, 1.0f, 1.0f } }));
	objects.push_back(std::make_unique<ObjectData>(ObjectData{ ObjectType::Box,{  12.5f, 0.5f, 0.5f },{ 0.0f, 0.0f, 0.0f},{ 1.0f, 1.0f, 1.0f } }));
	objects.push_back(std::make_unique<ObjectData>(ObjectData{ ObjectType::Box,{  11.5f, 0.5f, -6.5f },{ 0.0f, 0.0f, 0.0f},{ 1.0f, 1.0f, 1.0f } }));
	objects.push_back(std::make_unique<ObjectData>(ObjectData{ ObjectType::Mirror,{  -2.5f, 1.2f, 3.0f },{ 0.0f, 0.0f, 0.0f},{ 0.5f, 0.5f, 0.5f } }));
	objects.push_back(std::make_unique<ObjectData>(ObjectData{ ObjectType::Mirror,{  4.5f, 1.2f, 0.5f },{ 0.0f, 0.0f, 0.0f},{ 0.5f, 0.5f, 0.5f } }));
	objects.push_back(std::make_unique<ObjectData>(ObjectData{ ObjectType::Goal,{-1.0f, 3.0f,  17.0f },{ 0.0f, 0.0f, 0.0f},{ 0.01f, 0.01f, 0.01f } }));*/

	int a;
	////自分自身のオブジェクト（ステージ）
	//MyStage = std::make_shared<Stage>("Data/Model/Stage/tutorial_stage.mdl");
	//MyStage->SetScale({ 0.3f,0.3f,0.3f });
}

StageData2::StageData2(const char* fileName)
{
    // 1. ファイルを開く
    std::ifstream ifs(fileName);
    if (!ifs.is_open()) {
        return; // ファイルが開けなければ終了
    }

	MyStage = std::make_unique<Stage>("Data/Model/Stage/tutorial_stage.mdl");
	MyStage->SetScale({ 0.3f,0.3f,0.3f });

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
		else if(type=="Mirror")
			o = ObjectType::Mirror;
		else if (type == "Box")
			o = ObjectType::Box;
		else if(type=="Goal")
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

		objects.push_back(std::make_unique<ObjectData>(ObjectData{ o,position,angle,scale,direction }));
		
	}
}