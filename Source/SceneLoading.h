#pragma once
#include <thread>
#include "System/Sprite.h"
#include "Scene.h"
#include"LoadSprite.h"
#include"fade.h"
class SceneLoading :
    public Scene
{
public:
    SceneLoading(Scene* nextScene):nextScene(nextScene) {}
    ~SceneLoading()override {}

    //初期化
    void Initialize()override;

    //終了化
    void Finalize()override;

    //更新処理
    void Update(float elapsedTime)override;

    //描画処理
    void Render()override;

    //GUI描画
    void DrawGUI()override;

private:
    //ローディングスレッド
    static void LoadingThread(SceneLoading* scene);

private:
    //Sprite* sprite      = nullptr;
    Sprite2D sprite;
    LoadSprite loadSprites[2];
    /*float angle         = 0.0f;*/
    Scene* nextScene    = nullptr;
    std::thread* thread = nullptr;
    Fade fade;
    bool isFading = false;
};

