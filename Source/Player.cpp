#include"Player.h"
#include"System/Input.h"
#include"System/Audio.h"
#include<imgui.h>
#include"Camera.h"
#include"Collision.h"
#include"StageObjectManager.h"
#include"LaserManager.h"
#include"RideState.h"
#include"StageGrid.h"
#include"GoalObject.h"
#include "Stairs.h"
#include <algorithm>

// コンストラクタ
void Player::Initialize(const char* modelPath)
{
	//model = new Model("Data/Model/Mr.Incredible/Mr.Incredible.mdl");
	model = std::make_unique<Model>(modelPath);

	//モデルが大きいのでスケーリング
	scale.x = scale.y = scale.z = 0.01f;

	//ヒットエフェクト読み込み
	//hitEffect = new Effect("Data/Effect/Hit.efk");

	//ヒットSE読み込み
	//hitSE = Audio::Instance().LoadAudioSource("Data/Sound/Hit.wav");

	//アニメーションコントローラーにモデルをセット
	animation.setModel(model.get());

	const auto& animations = model->GetResource()->GetAnimations();
	//for (const auto& anim : animations)
	//{
	//	OutputDebugStringA(("Animation: " + anim.name + "\n").c_str());
	//}


	//初期ステートをIdleStateに設定
	state = std::make_unique<IdleState>();
	state->Initialize(*this);

	//コライダーのセット
	//bodyCollider.SetCenter({ position.x, position.y, position.z });
	//bodyCollider.SetSize(scale);

	UpdateCollider();
}

//デストラクタ
void Player::Finalize()
{
	//delete hitSE;
	//delete hitEffect;
	//delete model;
}

//ステート変更処理
void Player::ChangeState(std::unique_ptr<PlayerState> newState)
{
	//現在のステートの終了処理
	if (state)
	{
		state->Finalize(*this);
	}
	//新しいステートに変更
	state = std::move(newState);
	//新しいステートの初期化処理
	if (state)
	{
		state->Initialize(*this);
	}
}

//更新処理
void Player::Update(float elapsedTime, bool canControl)
{
	if (canControl)
	{
		isWaitingByLaserRotation = false;
	}

// 肩車タイマー更新
if (rideTimer > 0.0f)
{
	rideTimer -= elapsedTime;

	if (rideTimer <= 0.0f && !isRiding)
	{
		ridingTarget = nullptr;
	}
}

// ステート更新（操作中 または 肩車中）
//if ((canControl || isRiding) && state)
if (!isBoxPushing && (canControl || isRiding) && state) //箱を押しているときはステート更新しない・固定用

{
	state->Update(*this, elapsedTime, canControl);
}

// 肩車完了判定
bool rideReady = IsRideReady();

// 肩車位置まで上がるまでは通常更新しない
if (isRiding && !rideReady)
{
	return;
}
// 速度更新
UpdateVelocity(elapsedTime);

// 肩車中の位置固定処理
if (isRiding && ridingTarget != nullptr && rideReady)
{
	if (!canControl)
	{
		// 操作していない肩車中は、速度更新後にもう一度位置を合わせる
		// これをしないと待機中でも重力や残った速度で少しずつ滑る
		UpdateRiding(elapsedTime);
	}
	else
	{
		DirectX::XMFLOAT3 targetPosition = ridingTarget->GetPosition();

		float dx = position.x - targetPosition.x;
		float dz = position.z - targetPosition.z;
		float distanceSq = dx * dx + dz * dz;

		float standRadius = radius + ridingTarget->GetRadius();

		if (distanceSq < standRadius * standRadius)
		{
			float targetY = targetPosition.y + ridingTarget->GetHeight() - rideOffsetY;

			position.y = targetY;
			velocity.y = 0.0f;
			isGround = true;
		}
		else
		{
			isRiding = false;
			ridingTarget = nullptr;
			isGround = false;
		}
	}
}

// コライダー更新
//bodyCollider.SetCenter({ position.x, position.y - 0.1f, position.z });
//bodyCollider.SetSize({ 0.5f, 0.1f, 0.5f });

// 箱押しアニメ中の挙動切り替え
// 
// true  : Pushアニメ中はプレイヤーを止める
// false : Pushアニメ中も通常通り動ける
//
//アニメーション更新処理
animation.UpdateAnimation(elapsedTime);

// 箱を押すアニメーションが終わったら通常のアニメーションに戻す
if (isPlayingPushAnimation && !animation.IsPlaying())
{
	isPlayingPushAnimation = false;

	DirectX::XMFLOAT3 moveVec = GetMoveVec();

	if ((fabsf(moveVec.x) > 0.01f || fabsf(moveVec.z) > 0.01f))
	{
		animation.PlayAnimation("Run", true);
	}
	else
	{
		animation.PlayAnimation("Idle", true);
	}
}
	//無敵時間更新
	UpdateInvincibleTimer(elapsedTime);

	UpdateCollider();

	//プレイヤーとステージオブジェクトの衝突処理
	CollisionPlayerVsStage();

	//オブジェクト行列を更新
	UpdateTransform();

	//モデル行列更新
	model->UpdateTransform();
}

//コライダー更新処理
void Player::UpdateCollider()
{
	//コライダーのセット
	//bodyCollider.SetCenter({ position.x + bodyColliderOffset.x, position.y + bodyColliderOffset.y, position.z + bodyColliderOffset.z });
	//bodyCollider.SetSize({ 0.5f,0.5f,0.5f });

	//円柱
	bodyCylinderCollider.SetCenter({ position.x, position.y+0.5f, position.z });
	bodyCylinderCollider.SetRadius(0.5f);
	bodyCylinderCollider.SetHeight(0.7f);
}

//描画処理
void Player::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	renderer->Render(rc, transform, model.get(), ShaderId::Lambert);
}

//デバッグプリミティブ描画
void Player::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
	//基底クラスの関数の呼び出し
	//Character::RenderDebugPrimitive(rc, renderer);

	//BoxColliderのデバッグプリミティブ描画
	//renderer->RenderBox(rc, bodyCollider.GetCenter(), { 0,0,0 }, bodyCollider.GetSize(), DirectX::XMFLOAT4(0, 1, 0, 1));

	//円柱
	DirectX::XMFLOAT3 position = bodyCylinderCollider.GetCenter();
	position.y -= bodyCylinderCollider.GetHeight() * 0.5f;
	renderer->RenderCylinder(rc, position, bodyCylinderCollider.GetRadius(), bodyCylinderCollider.GetHeight()*2, DirectX::XMFLOAT4(0, 1, 1, 1));
}

//デバッグ用GUI描画
void Player::DrawDebugGUI()
{
	ImVec2 pos = ImGui::GetMainViewport()->GetWorkPos();
	ImGui::SetNextWindowPos(ImVec2(pos.x + 10, pos.y + 10), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None))
	{
		//トランスフォーム
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			//位置
			ImGui::InputFloat3("Position", &position.x);
			//回転
			DirectX::XMFLOAT3 a;
			a.x = DirectX::XMConvertToDegrees(angle.x);
			a.y = DirectX::XMConvertToDegrees(angle.y);
			a.z = DirectX::XMConvertToDegrees(angle.z);
			ImGui::InputFloat3("Angle", &a.x);
			angle.x = DirectX::XMConvertToRadians(a.x);
			angle.y = DirectX::XMConvertToRadians(a.y);
			angle.z = DirectX::XMConvertToRadians(a.z);
			//スケール	
			ImGui::InputFloat3("Scale", &scale.x);
		}
	}
	ImGui::End();
}

DirectX::XMFLOAT3 Player::GetMoveVec()const
{
	//入力情報を取得
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ax = gamePad.GetAxisLX();
	float ay = gamePad.GetAxisLY();

	//カメラ方向とスティックの入力値によって進行方向を計算する
	Camera& camera = Camera::Instance();
	const DirectX::XMFLOAT3& cameraRight = camera.GetRight();
	const DirectX::XMFLOAT3& cameraFront = camera.GetFront();

	//移動ベクトルはXZ平面に水平なベクトルになるようにする

	//カメラ右方向ベクトルをXZ単位ベクトルに変換
	float cameraRightX = cameraRight.x;
	float cameraRightZ = cameraRight.z;
	float cameraRightLength = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);
	if (cameraRightLength > 0.0f)
	{
		//単位ベクトル化
		cameraRightX /= cameraRightLength;
		cameraRightZ /= cameraRightLength;
	}

	//カメラ前方向ベクトルをXZ単位ベクトルに変換
	float cameraFrontX = cameraFront.x;
	float cameraFrontZ = cameraFront.z;
	float cameraFrontLength = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);
	if (cameraFrontLength > 0.0f)
	{
		//単位ベクトル化
		cameraFrontX /= cameraFrontLength;
		cameraFrontZ /= cameraFrontLength;
	}

	//スティックの水平入力値をカメラ右方向に反映し、
	//スティックの垂直入力値をカメラ前方向に反映し、
	//進行ベクトルを計算する
	DirectX::XMFLOAT3 vec;
	/*vec.x = cameraRightX * ax+cameraRightZ*ay;
	vec.z = cameraFrontX * ax+cameraFrontZ*ay;*/

	vec.x = cameraRightX * ax + cameraFrontX * ay;
	vec.z = cameraRightZ * ax + cameraFrontZ * ay;
	//Y軸方向には移動しない
	vec.y = 0.0f;

	return vec;
}

//向き取得
DirectX::XMFLOAT3 Player::GetForward() const
{
	DirectX::XMFLOAT3 forward;
	forward.x = sinf(angle.y);
	forward.y = 0.0f;
	forward.z = cosf(angle.y);
	return forward;
}

bool Player::RayCastLaserMirror(
	const DirectX::XMFLOAT3& start,
	const DirectX::XMFLOAT3& end,
	DirectX::XMFLOAT3& hitPos,
	DirectX::XMFLOAT3& hitNormal) const
{
	// ステージ1ではロボット鏡判定を無効
	if (IsStage1RobotDisabled()) return false;

	// ロボット以外は鏡を持っていない
	if (!isRobot) return false;

	// ロボットの正面方向を、鏡面の法線として使う。ロボットの正面に鏡が付いている扱い
	DirectX::XMFLOAT3 normal = GetForward();

	// 鏡面の中心位置
	// プレイヤーの足元 position から少し前方・少し上に鏡がある想定
	const float mirrorHeightOffset = 0.6f;
	DirectX::XMFLOAT3 planeCenter = {
		position.x + normal.x * 0.25f,
		position.y + 0.55f + mirrorHeightOffset,
		position.z + normal.z * 0.25f
	};

	// レーザーの始点から終点へのベクトルをつｋる
	DirectX::XMFLOAT3 ray = {
		end.x - start.x,
		end.y - start.y,
		end.z - start.z
	};

	// レーザー線分の長さを求める
	float rayLength = sqrtf(ray.x * ray.x + ray.y * ray.y + ray.z * ray.z);

	// 長さがない線分は判定できない
	if (rayLength <= 0.0001f) return false;

	// レーザー方向を正規化する
	ray.x /= rayLength;
	ray.y /= rayLength;
	ray.z /= rayLength;

	// レーザー方向と鏡の法線の内積
	// 0に近いほど鏡面と平行で、交差位置を安定して求められない
	float denom =
		ray.x * normal.x +
		ray.y * normal.y +
		ray.z * normal.z;

	// ロボットの正面側から入ってきたレーザーだけ反射対象にする
	if (denom >= -0.1f) return false;	// denom が 0 に近い場合や、背面側から来た場合は反射しない

	// レーザー線分と鏡面の交点までの距離を求める
	float t =
		((planeCenter.x - start.x) * normal.x +
			(planeCenter.y - start.y) * normal.y +
			(planeCenter.z - start.z) * normal.z) / denom;

	// 交点がレーザー線分の範囲外なら当たっていない
	if (t < 0.0f || t > rayLength) return false;

	// 実際の交点を求める
	hitPos = {
		start.x + ray.x * t,
		start.y + ray.y * t,
		start.z + ray.z * t
	};

	// 鏡面の横方向ベクトル。normal をXZ平面で90度回して、鏡の幅方向として使う
	DirectX::XMFLOAT3 right = {
		normal.z,
		0.0f,
		-normal.x
	};

	// 交点が鏡の中心から横にどれだけ離れているか
	float localX =
		(hitPos.x - planeCenter.x) * right.x +
		(hitPos.z - planeCenter.z) * right.z;

	// 交点が鏡の中心から上下にどれだけ離れているか
	float localY = hitPos.y - planeCenter.y;

	// 鏡判定の半分の幅と高さ
	// 値を大きくすると、ロボットの鏡判定が広くなる
	const float mirrorHalfWidth = 0.9f;
	const float mirrorHalfHeight = 1.0f;

	// 交点が鏡の範囲外なら当たっていない
	if (fabsf(localX) > mirrorHalfWidth) return false;
	if (fabsf(localY) > mirrorHalfHeight) return false;

	// 反射に使う法線を返す
	hitNormal = normal;

	return true;
}


bool Player::IsStage1RobotDisabled() const
{
	return isRobot &&
		StageObjectManager::Instance().GetStageIndex() <= 2;
}

//着地したときの処理
void Player::OnLanding()
{
	JumpCount = 0;
}

//移動入力処理
void Player::InputMove(float elapsedTime)
{
	//進行ベクトル取得
	DirectX::XMFLOAT3 moveVec = GetMoveVec();

	//移動処理
	Move(elapsedTime, moveVec.x, moveVec.z, moveSpeed);

	//ロボットは45度回転で向きを変えるので、移動方向へ自動旋回しない
	if (!isRobot)
	{
		Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);
	}
}

//箱を押したときanimation再生
void Player::PlayPushAnimation()
{
	if (isRobot) return;

	isPlayingPushAnimation = true;

	animation.PlayAnimation("Push", false);
}

//木箱を押すときに固定化する用・いらないなら消す
void Player::StartBoxPush()
{
	if (isRobot) return;

	isBoxPushing = true;

	ResetMove();

	animation.PlayAnimation("Push", false);
}

void Player::StopBoxPush()
{
	isBoxPushing = false;

	ResetMove();

	//移動入力があるときはRunアニメーション、ないときはIdleアニメーションにする
	DirectX::XMFLOAT3 moveVec = GetMoveVec();
	if ((fabsf(moveVec.x) > 0.01f || fabsf(moveVec.z) > 0.01f))
	{
		animation.PlayAnimation("Run", true);
	}
	else
	{
		animation.PlayAnimation("Idle", true);
	}
}
//ここまで

//ステージとの衝突処理
void Player::CollisionPlayerVsStage()
{

	StageObjectManager& stageObjectManager = StageObjectManager::Instance();

	//レイを飛ばす位置調整用　元々0.5だったが階段が高いので多めにした
	float playerRayOffsetY = 0.85f;

	float playerWallRayOffsetY = 1.05f;

	// 接地判定（真下レイキャスト）
	{
		bool wasGround = isGround;

		DirectX::XMFLOAT3 start = { position.x, position.y + playerRayOffsetY, position.z };
		DirectX::XMFLOAT3 end = { position.x, position.y - 0.1f, position.z };
		if (isGround) end.y -= 0.2f;

		DirectX::XMFLOAT3 hitPos, hitNormal;

		RayHitResult result = StageObjectManager::Instance().RayCastFloor(start, end);
		if (result.hit)
		{
			//地面に当たった位置に高さを合わせる
			position.y = result.hitPos.y;
			velocity.y = 0.0f;
			isGround = true;

			if (!wasGround) OnLanding();
		}
		else
		{
			isGround = false;
		}

		//result = stageObjectManager.RayCast(start, end, hitPos, hitNormal);

		//if (result.hit)
		//{
		//	//地面に当たった位置に高さを合わせる
		//	position.y = hitPos.y;
		//	velocity.y = 0.0f;
		//	isGround = true;

		//	if (!wasGround) OnLanding();
		//}
		//else
		//{
		//	isGround = false;
		//}
		//
		
	}

	// 壁判定（3本レイ）
	{
		float vx = velocity.x;
		float vz = velocity.z;

		float speedSq = vx * vx + vz * vz;

		// 移動していないときは壁判定をしない
		if (speedSq > 0.0001f)
		{
			// プレイヤーの半径
			const float playerRadius = 0.5f;

			// 壁にぴったり張り付かないようにする余白
			const float skin = 0.02f;

			// 半径分 + 余白だけレイを飛ばす
			const float rayLength = playerRadius + skin;

			// 速度ベクトルを正規化して移動方向を求める
			float speed = sqrtf(speedSq);
			DirectX::XMFLOAT3 rayDir = {
				vx / speed,
				0.0f,
				vz / speed
			};

			// 移動方向に対して垂直な横方向ベクトル（左右レイのオフセットに使う）
			// XZ平面で90度回転させるだけなので (-Z, 0, X) になる
			DirectX::XMFLOAT3 sideDir = {
				-rayDir.z,
				0.0f,
				rayDir.x
			};

			// 横レイのずらし量（半径より少し小さめにして角が引っかかりにくくする）
			const float sideOffset = playerRadius * 0.8f;

			// 3本のレイの発射起点
			// 腰あたりの高さ(+0.5f)から飛ばす
			DirectX::XMFLOAT3 origins[3] = {
				// 中央
				{ 
				position.x, 
				position.y + playerWallRayOffsetY,
				position.z
				},
				// 左
				{ 
				position.x + sideDir.x * sideOffset, 
				position.y + playerWallRayOffsetY,
				position.z + sideDir.z * sideOffset 
				},
				// 右
				{
				position.x - sideDir.x * sideOffset, 
				position.y + playerWallRayOffsetY,
				position.z - sideDir.z * sideOffset 
				},
			};

			// 3本それぞれ判定する
			for (const auto& origin : origins)
			{
				// 移動方向へレイを伸ばす
				DirectX::XMFLOAT3 end = {
					origin.x + rayDir.x * rayLength,
					origin.y,
					origin.z + rayDir.z * rayLength
				};

				DirectX::XMFLOAT3 hitPos, hitNormal;
				RayHitResult result = stageObjectManager.RayCast(origin, end, hitPos, hitNormal);

				if (result.hit)
				{
					// 壁の法線方向への速度成分を求める
					float dot = velocity.x * hitNormal.x + velocity.z * hitNormal.z;

					// 壁に向かう成分だけ消して、壁沿いに動けるようにする
					if (dot < 0.0f)
					{
						velocity.x -= hitNormal.x * dot;
						velocity.z -= hitNormal.z * dot;
					}

					// プレイヤー中心から壁ヒット地点までのXZ距離を求める
					float dx = hitPos.x - position.x;
					float dz = hitPos.z - position.z;
					float distToWall = sqrtf(dx * dx + dz * dz);

					// 壁との距離が半径より近い（めり込んでいる）場合は押し戻す
					if (distToWall < playerRadius)
					{
						float pushDist = playerRadius - distToWall + skin;
						position.x += hitNormal.x * pushDist;
						position.z += hitNormal.z * pushDist;
					}
				}
			}
		}
	}

	LaserManager* laserManager = StageObjectManager::Instance().GetLaserManager();

	for (int i = 0; i < laserManager->GetLaserCount(); ++i)
	{
		Laser* laser = laserManager->GetLaser(i);
		if (!laser || !laser->IsActive()) continue;

		if (IsStage1RobotDisabled())
		{
			continue;
		}

		bool isHit = false;
		int loopCount = 0;
		const float skinWidth = 0.002f;

		LaserHit hit; // ← 外で宣言

		do {
			loopCount++;

			hit = laser->GetBeam().CheckHitCylinder(bodyCylinderCollider);

			if (!hit.hit)
			{
				isHit = false;
				break;
			}

			isHit = true;

			// Player2はレーザーを止める側なので、レーザーで押し戻さない
			// レーザーの反射・停止は LaserBeam 側で処理する
			if (isRobot)
			{
				break;
			}

			// 上に乗る処理
			if (hit.normal.y > 0.7f && velocity.y <= 0)
			{
				velocity.y = 0.0f;

				//position.y = 1.34;

				position.y = hit.point.y - 0.15f;	//コライダーが0.15飛び出てるので

				isGround = true;
				OnLanding();
			}
			else
			{
				UpdateCollider();

				// 押し出し量
				float push = hit.penetration + 0.001f;

				// 正しい高さ（腰の高さ）で先読み用のスタートとエンドを設定する
				DirectX::XMFLOAT3 rayStart = { position.x, position.y + playerRayOffsetY, position.z };
				DirectX::XMFLOAT3 rayEnd = {
					position.x + hit.normal.x * (push + 0.5f), // 押し出し量 ＋ プレイヤーの半径(0.5f) 分まで先読み
					position.y + playerRayOffsetY,
					position.z + hit.normal.z * (push + 0.5f)
				};

				RayHitResult ray = { false, nullptr, RayHitType::None, {0,0,0}, {0,0,0}, {0,0,0} };
				ray = StageObjectManager::Instance().RayCast(rayStart, rayEnd, ray.hitPos, ray.hitNormal);

				if (ray.hit)
				{
					//何もしない
				}
				else
				{
					position.x += hit.normal.x * push;
					position.z += hit.normal.z * push;
					// ? めり込み防止（速度殺し）
					float dot =
						velocity.x * hit.normal.x +
						velocity.z * hit.normal.z;

					if (dot < 0.0f)
					{
						velocity.x -= hit.normal.x * dot;
						velocity.z -= hit.normal.z * dot;
					}
				}

			}

			UpdateCollider();

		} while (isHit && loopCount < 10);
	}


	//ステージグリッドとゴールオブジェクト
	// 全てのステージオブジェクトに対してループ
	for (int i = 0; i < stageObjectManager.GetStageObjectSize(); ++i)
	{
		StageObject* obj = stageObjectManager.GetStageObject(i);
		if (!obj) continue;


		if (StageGrid* grid = dynamic_cast<StageGrid*>(obj))
		{
			grid->CollisionVsPlayer(*this);
		}

		else if (GoalObject* goal = dynamic_cast<GoalObject*>(obj))
		{
			goal->CollisionVsPlayer(*this);
		}
	}


	// ステージ外に出ないように、プレイヤーの位置を範囲内に制限する
	position.x = std::clamp(
		position.x,
		stageObjectManager.GetStageMinX(),
		stageObjectManager.GetStageMaxX()
	);

	position.z = std::clamp(
		position.z,
		stageObjectManager.GetStageMinZ(),
		stageObjectManager.GetStageMaxZ()
	);

	UpdateCollider();

}

// プレイヤー同士の衝突処理
// 肩車対応あり
void Player::CollisionVsPlayer(Player& other, bool canRide)
{
	if (IsStage1RobotDisabled()) return;
	if (other.IsStage1RobotDisabled()) return;

	DirectX::XMFLOAT3 otherPosition = other.GetPosition();
		
	float dx = position.x - otherPosition.x;
	float dz = position.z - otherPosition.z;

	//XZ平面で2人の距離を計算する
	float distanceSq = dx * dx + dz * dz;
	float minDistance = radius + other.GetRadius();

// 距離が離れていれば何もしない
if (distanceSq >= minDistance * minDistance)
{
	return;
}

// 肩車可能なら肩車処理
if (canRide)
{
	float otherTop = otherPosition.y + other.GetHeight();

	//値を大きくするほど、肩車判定で吸い付きにくくなる
	bool isAboveOther = position.y >= otherTop - 1.0f;

	if (!isAboveOther && !isRiding && rideTimer <= 0.0f)
	{
		StartRiding(other);
	}

	return;
}

// 完全に同じ位置にいる場合の押し出し（mainの安全処理）
if (distanceSq <= 0.0001f)
{
	position.x += minDistance;
	UpdateTransform();
	return;
}

	//半径の合計より近い場合は、重ならない位置まで押し戻す
	if (distanceSq < minDistance * minDistance)
	{
		float distance = sqrtf(distanceSq);
		float push = minDistance - distance;

		dx /= distance;
		dz /= distance;
	  // 自分だけ押し戻す
		position.x += dx * push;
		position.z += dz * push;

		UpdateTransform();
	}
}


//ジャンプ入力処理
bool Player::InputJump()
{
	// ジャンプ機能は使わないので、入力が来ても何もしない。
	// 落下や着地処理は別で使うため、関数だけ残して呼び出し元への影響を小さくする。
	return false;
}

//操作対象から外れたときに移動を止め、待機状態に戻す
void Player::StopControl()
{
	velocity.x = 0.0f;
	velocity.z = 0.0f;
	isControlling = false;
	ChangeState(std::make_unique<IdleState>());
}

// 肩車開始処理
void Player::StartRiding(Player& target)
{
	// 肩車中にする
	isRiding = true;

	// 乗る相手を保存
	ridingTarget = &target;

	// 移動や速度をリセット
	ResetMove();

	// 肩車状態へ切り替える
	ChangeState(std::make_unique<RideState>());
}

// 肩車状態の更新処理
void Player::UpdateRiding(float elapsedTime)
{
	if (ridingTarget == nullptr)
	{
		isRiding = false;
		ChangeState(std::make_unique<IdleState>());
		return;
	}

	DirectX::XMFLOAT3 targetPosition = ridingTarget->GetPosition();

	// 相手プレイヤーの位置に合わせる
	position.x = targetPosition.x;
	position.z = targetPosition.z;

	// 肩車する高さを計算
	float targetY = targetPosition.y + ridingTarget->GetHeight() - rideOffsetY;

	// 肩車位置まで上がる速度
	float riseSpeed = 5.0f;


	if (position.y < targetY)
	{
		position.y += riseSpeed * elapsedTime;

		if (position.y > targetY)
		{
			position.y = targetY;
		}
	}
	else
	{
		position.y = targetY;
	}

	ResetMove();

	isGround = true;

	//bodyCollider.SetCenter({ position.x, position.y - 0.1f, position.z });
	//bodyCollider.SetSize({ 0.5f, 0.1f, 0.5f });
	UpdateCollider();
	UpdateTransform();
	model->UpdateTransform();
}

//肩車解除処理
void Player::StopRiding()
{
	isRiding = false;

	velocity.x = 0.0f;
	velocity.y = 0.0f;
	velocity.z = 0.0f;

	moveVecX = 0.0f;
	moveVecZ = 0.0f;
	maxMoveSpeed = 0.0f;

	isGround = true;
	OnLanding();

	rideTimer = 3.0f;

	ChangeState(std::make_unique<IdleState>());
	UpdateCollider();
	UpdateTransform();
	model->UpdateTransform();
}

void Player::ResetMove()
{
	velocity.x = 0.0f;
	velocity.y = 0.0f;
	velocity.z = 0.0f;

	moveVecX = 0.0f;
	moveVecZ = 0.0f;
	maxMoveSpeed = 0.0f;
}

void Player::WaitIdleForLaserRotation()
{
	// レーザー回転中は操作できないので横移動だけ止める。
	// Y速度まで消すと、空中やレーザー上から降りている時に重力を受けず固まってしまう。
	float verticalVelocity = velocity.y;
	ResetMove();
	velocity.y = verticalVelocity;

	// 肩車中にStateをIdleへ変えると肩車処理が切れるので、通常時だけ状態も戻す
	if (!isRiding && !isWaitingByLaserRotation)
	{
		ChangeState(std::make_unique<IdleState>());
		isWaitingByLaserRotation = true;
	}

	// 直前にRunだった場合でも、見た目は待機にする
	animation.PlayAnimation("Idle", true);
}


bool Player::IsRideReady() const
{
	if (ridingTarget == nullptr)
	{
		return false;
	}

	float targetY = ridingTarget->GetPosition().y + ridingTarget->GetHeight() - rideOffsetY;

	return position.y >= targetY - 0.01f;
}

//操作中のプレイヤーだけ、かつロボット以外だけ木箱を押せる
void Player::InputRotate()
{

	if (IsStage1RobotDisabled()) return;

	if (!isRobot) return;

	GamePad& gamePad = Input::Instance().GetGamePad();

	//LB/RBで45度ずつ回転
	const float step = DirectX::XMConvertToRadians(45.0f);

	if (gamePad.GetButtonDown() & GamePad::BTN_LEFT)
	{
		angle.y -= step;
	}
	if (gamePad.GetButtonDown() & GamePad::BTN_RIGHT)
	{
		angle.y += step;
	}
}
