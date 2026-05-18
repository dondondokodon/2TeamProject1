#include"SceneManager.h"

//更新処理
void SceneManager::Update(float elapsedTime)
{
	if (nextScene != nullptr)
	{
		//古いシーンを終了処理
		Clear();

		//新しいシーンを設定
		currentScene = nextScene;
		nextScene = nullptr;
		
		//シーン初期化処理
		if (!currentScene->IsReady())
		{
			currentScene->Initialize();
		}
	}

	if (currentScene != nullptr)
	{
		currentScene->Update(elapsedTime);
	}
}

//描画処理
void SceneManager::Render()
{
	if (currentScene != nullptr)
	{
		currentScene->Render();
	}
}

//GUI描画
void SceneManager::DrawGUI()
{
	if (currentScene != nullptr)
	{
		currentScene->DrawGUI();
	}
}

//シーンクリア
void SceneManager::Clear()
{
	if (currentScene != nullptr)
	{
		Scene* old = currentScene;
		currentScene = nullptr; // Finalize 中の参照事故を防ぐ
		old->Finalize();
		delete old;
	}
}


//シーン切り替え
void SceneManager::ChangeScene(Scene* scene)
{
	// すでに nextScene があれば破棄（リーク防止）
	if (nextScene != nullptr)
	{
		delete nextScene;
		nextScene = nullptr;
	}

	nextScene = scene; // 所有権を受け取る
}
