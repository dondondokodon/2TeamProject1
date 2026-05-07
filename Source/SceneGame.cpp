#include "System/Graphics.h"
#include "SceneGame.h"
#include"Camera.h"
#include"EnemyManager.h"
#include"EnemySlime.h"
#include"Player.h"
#include"Laser.h"
#include"Mirror.h"
#include "EffectManager.h"
#include"StageObjectManager.h"
#include"LaserManager.h"
#include"System/Input.h"
#include"IrradiationDevice.h"

#include"StageData1.h"

//
void SceneGame::Initialize()
{

	//ステージ初期化
	//stage = new Stage();

	//ステージグリッド初期化
	stageGrid = new StageGrid();





	//プレイヤー初期化
	players[0] = new Player();
	players[0]->Initialize("Data/Model/Player/Player.mdl");
	players[0]->SetPosition({ -5.0f, 0.0f, -3.0f });
	players[0]->SetScale({ 0.5f,0.5f,0.5f });

	players[1] = new Player();
	players[1]->Initialize("Data/Model/Jammo/Jammo.mdl");
	players[1]->SetPosition({ 5.0f, 0.0f, -3.0f });

	controlPlayerIndex = 0;
	//カメラコントローラー初期化
	cameraController = new CameraController();

	//カメラ初期設定
	Graphics& graphics = Graphics::Instance();
	Camera& camera = Camera::Instance();
	camera.SetLookAt(
		DirectX::XMFLOAT3(0, 10, -10),//視点
		DirectX::XMFLOAT3(0, 0, 0),	//注視点
		DirectX::XMFLOAT3(0, 1, 0)	//上方向
	);
	camera.SetPerspectiveFov(
		DirectX::XMConvertToRadians(45),	//視野角
		graphics.GetScreenWidth() / graphics.GetScreenHeight(),//画面アスペクト比
		0.1f,	//クリップ距離(近)
		1000.0f //クリップ距離(遠)
	);

	//エネミー初期化
	/*EnemyManager& enemyManager=EnemyManager::Instance();
	for (int i = 0;i < 2;i++)
	{
		EnemySlime* slime = new EnemySlime();
		slime->SetPosition(DirectX::XMFLOAT3(i * 2.0f, 0, 5));
		slime->SetTerritory(slime->GetPosition(), 10.0f);
		enemyManager.Register(slime);
	}*/

	EffectManager::Instance().Initialize();

	
	//ステージ初期化
	std::unique_ptr<StageData> stageData = std::make_unique<StageData1>(StageData1());
	StageObjectManager& mng = StageObjectManager::Instance();
	mng.setLaserManager(new LaserManager());
	mng.LoadStageData(stageData.get());
}

// 終了化
void SceneGame::Finalize()
{
	//ステージ終了化
	/*if (stage != nullptr)
	{
		delete stage;
		stage = nullptr;
	}*/

	//ステージグリッド終了化
	if (stageGrid != nullptr)
	{
		delete stageGrid;
		stageGrid = nullptr;
	}


	//プレイヤー終了化
	for (int i = 0; i < 2; ++i)
	{
		if (players[i] != nullptr)
		{
			players[i]->Finalize();
			delete players[i];
			players[i] = nullptr;
		}
	}

	//カメラコントローラー終了化
	if (cameraController!=nullptr)
	{
		delete cameraController;
		cameraController = nullptr;
	}

	//エネミー終了化
	//EnemyManager::Instance().Clear();



	EffectManager::Instance().Finalize();

}

// 更新処理
void SceneGame::Update(float elapsedTime)
{
	//stage->Update(elapsedTime);

	// ★ 毎フレームリセット
	stageGrid->isTouchingPlayer = false;

	// 木箱との当たり判定（isTouchingPlayer が true になる）
	stageGrid->CollisionVsPlayer(*players[controlPlayerIndex]);

	// 木箱の更新処理（isTouchingPlayer を使う）
	stageGrid->Update(elapsedTime, * players[controlPlayerIndex]);
	

	// カメラ更新
	InputChangePlayer();

	Player* controlPlayer = GetControlPlayer();

	DirectX::XMFLOAT3 target = controlPlayer->GetPosition();
	target.y += 0.5f;
	cameraController->SetTarget(target);
	cameraController->Update(elapsedTime);

	for (int i = 0; i < 2; ++i)
	{
		if (players[i] != nullptr)
		{
			bool canControl = (i == controlPlayerIndex);
			players[i]->Update(elapsedTime, canControl);
		}
	}

	if (players[0] != nullptr && players[1] != nullptr)
	{
		// プレイヤー同士の衝突判定
		if (!players[0]->IsRiding())
		{
			int otherIndex = 1 - controlPlayerIndex;										
			bool canRide = (controlPlayerIndex == 0 && otherIndex == 1);					
			players[controlPlayerIndex]->CollisionVsPlayer(*players[otherIndex], canRide);	
		}
	}


	//プレイヤー更新処理
	// Player::Instance().Update(elapsedTime);

	//エネミー更新処理
	//EnemyManager::Instance().Update(elapsedTime);

	//ステージオブジェクト更新処理
	StageObjectManager::Instance().Update(elapsedTime);

	// エフェクト更新
	EffectManager::Instance().Update(elapsedTime);
}

// 描画処理
void SceneGame::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ShapeRenderer* shapeRenderer = graphics.GetShapeRenderer();
	ModelRenderer* modelRenderer = graphics.GetModelRenderer();

	//描画処理
	//RenderContext rc;

	// 描画準備
	RenderContext rc;
	rc.deviceContext = dc;
	rc.lightDirection = { 0.0f, -1.0f, 0.0f };	// ライト方向（下方向）
	rc.renderState = graphics.GetRenderState();

	//カメラパラメータ設定
	Camera& camera = Camera::Instance();
	rc.view = camera.GetView();
	rc.projection = camera.GetProjection();

	//// ビュー行列
	//{
	//	DirectX::XMFLOAT3 eye = { 0, 10, -10 };	// カメラの視点（位置）
	//	DirectX::XMFLOAT3 focus = { 0, 0, 0 };	// カメラの注視点（ターゲット）
	//	DirectX::XMFLOAT3 up = { 0, 1, 0 };		// カメラの上方向

	//	DirectX::XMVECTOR Eye = DirectX::XMLoadFloat3(&eye);
	//	DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&focus);
	//	DirectX::XMVECTOR Up = DirectX::XMLoadFloat3(&up);
	//	DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH(Eye, Focus, Up);
	//	DirectX::XMStoreFloat4x4(&rc.view, View);
	//}
	//// プロジェクション行列
	//{
	//	float fovY = DirectX::XMConvertToRadians(45);	// 視野角
	//	float aspectRatio = graphics.GetScreenWidth() / graphics.GetScreenHeight();	// 画面縦横比率
	//	float nearZ = 0.1f;	// カメラが映し出すの最近距離
	//	float farZ = 1000.0f;	// カメラが映し出すの最遠距離
	//	DirectX::XMMATRIX Projection = DirectX::XMMatrixPerspectiveFovLH(fovY, aspectRatio, nearZ, farZ);
	//	DirectX::XMStoreFloat4x4(&rc.projection, Projection);
	//}

	// 3Dモデル描画
	{
		//ステージ描画
		//stage->Render(rc, modelRenderer);

		//ステージグリッド(今は木箱を出す用)描画
		stageGrid->Render(rc, modelRenderer);

		//鏡描画


		//プレイヤー描画
		for (int i = 0; i < 2; ++i)
		{
			if (players[i] != nullptr)
			{
				players[i]->Render(rc, modelRenderer);
			}
		}

		//エネミー描画
		//EnemyManager::Instance().Render(rc, modelRenderer);

		//ステージオブジェクト描画
		StageObjectManager::Instance().Render(rc, modelRenderer);

		//エフェクト描画
		EffectManager::Instance().Render(rc.view, rc.projection);
	}

	// 3Dデバッグ描画
	{
		//プレイヤーデバッグプリミティブ描画
		for (int i = 0; i < 2; ++i)
		{
			if (players[i] != nullptr)
			{
				players[i]->RenderDebugPrimitive(rc, shapeRenderer);
			}
		}

		//エネミーデバッグプリミティブ描画
		//EnemyManager::Instance().RenderDebugPrimitive(rc,shapeRenderer);

		//ステージオブジェクトデバッグプリミティブ描画
		StageObjectManager::Instance().RenderDebugPrimitive(rc, shapeRenderer);

		//木箱用デバッグプリミティブ描画
		stageGrid->RenderDebugPrimitive(rc, shapeRenderer);

		

	}

	// 2Dスプライト描画
	{

	}
}

// GUI描画
void SceneGame::DrawGUI()
{
	//プレイヤーデバッグ描画
	Player::Instance().DrawDebugGUI();

	//ステージオブジェクトマネージャー
	StageObjectManager::Instance().DrawDebugGUI();



	//Player::Instance().DrawDebugGUI();
	Player* controlPlayer = GetControlPlayer();

	if (controlPlayer != nullptr)
	{
		controlPlayer->DrawDebugGUI();
	}

}

void SceneGame::InputChangePlayer()
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	if (gamePad.GetButtonDown() & GamePad::BTN_B)
	{
		if (!players[controlPlayerIndex]->IsRiding())
		{
			players[controlPlayerIndex]->StopControl();
		}

		controlPlayerIndex = 1 - controlPlayerIndex;
	}
}

Player* SceneGame::GetControlPlayer()
{
	return players[controlPlayerIndex];
}
