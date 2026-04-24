#include <filesystem>
#include <imgui.h>
#include "System/Graphics.h"
#include "ResourceManager.h"

// モデルリソース読み込み
std::shared_ptr<ModelResource> ResourceManager::LoadModelResource(const char* filename)
{
	//既に読み込まれていた場合は読み込み済みのリソースを返す
	auto it = models.find(filename);	//filenameのキーが見つかったらその場所が入り　なかったらendの値が入る
	if (it != models.end())				//見つかったか
	{
		if (!it->second.expired())		//まだ参照しているか
		{
			return it->second.lock();
		}
	}

	//新規モデルリソース作成＆読み込み
	std::shared_ptr<ModelResource> p = std::make_shared<ModelResource>();
	p->Load(Graphics::Instance().GetDevice(), filename);

	//読み込み管理用の変数に登録
	models[filename] = p;

	//作成したリソースを返す
	return p;
}

// デバッグGUI描画
void ResourceManager::DrawDebugGUI()
{
	if (ImGui::CollapsingHeader("Resource", ImGuiTreeNodeFlags_DefaultOpen))
	{
		for (auto it = models.begin(); it != models.end(); ++it)
		{
			std::filesystem::path filepath(it->first);

			int use_count = it->second.use_count();
			ImGui::Text("use_count = %5d : %s", use_count, filepath.filename().u8string().c_str());
		}
	}
}

