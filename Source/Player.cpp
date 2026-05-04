#include"Player.h"
#include"System/Input.h"
#include"System/Audio.h"
#include<imgui.h>
#include"Camera.h"
#include"EnemyManager.h"
#include"Collision.h"
#include"ProjectileStraight.h"
#include"ProjectileHoming.h"
#include"StageObjectManager.h"
#include"LaserManager.h"
#include"RideState.h"

// コンストラクタ
void Player::Initialize(const char* modelPath)
{
	//model = new Model("Data/Model/Mr.Incredible/Mr.Incredible.mdl");
	model = std::make_unique<Model>(modelPath);

	//モデルが大きいのでスケーリング
	scale.x = scale.y = scale.z = 0.01f;

	//ヒットエフェクト読み込み
	hitEffect = new Effect("Data/Effect/Hit.efk");

	//ヒットSE読み込み
	hitSE = Audio::Instance().LoadAudioSource("Data/Sound/Hit.wav");

	//アニメーションコントローラーにモデルをセット
	animation.setModel(model.get());

	//初期ステートをIdleStateに設定
	state = std::make_unique<IdleState>();
	state->Initialize(*this);

	//コライダーのセット
	bodyCollider.SetCenter({ position.x, position.y, position.z });
	bodyCollider.SetSize(scale);
}

//デストラクタ
void Player::Finalize()
{
	delete hitSE;
	delete hitEffect;
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
	//肩車解除後の再肩車禁止時間を更新する
	if (rideTimer > 0.0f)
	{
		rideTimer -= elapsedTime;

		//肩車していない状態で禁止時間が終わったら、乗っていた相手を外す
		if (rideTimer <= 0.0f && !isRiding)
		{
			ridingTarget = nullptr;
		}
	}

	//肩車中のPlayer1も更新するため、操作中または肩車中ならStateを更新する
	if ((canControl || isRiding) && state)
	{
		state->Update(*this, elapsedTime, canControl);
	}

	//重力で判定がずれないように、State更新後の肩車完了判定を保存する
	bool rideReady = IsRideReady();

	//肩車位置まで上がりきっていない間は、通常の移動更新を行わない
	if (isRiding && !rideReady)
	{
		return;
	}

	//肩車中かつ操作中なら、Player2の上にいる間だけ足場扱いにする
	if (isRiding && ridingTarget != nullptr && canControl && rideReady)
	{
		return;
	}

	//重力や移動速度を反映する
	UpdateVelocity(elapsedTime);

	//肩車中で操作している場合は、上がりきった後だけ足場扱いにする
	if (isRiding && ridingTarget != nullptr && canControl && rideReady)
	{
		DirectX::XMFLOAT3 targetPosition = ridingTarget->GetPosition();

		float dx = position.x - targetPosition.x;
		float dz = position.z - targetPosition.z;
		float distanceSq = dx * dx + dz * dz;

		float standRadius = radius + ridingTarget->GetRadius();

		//Player2の上にいる間だけ、高さを固定する
		if (distanceSq < standRadius * standRadius)
		{
			float targetY = targetPosition.y + ridingTarget->GetHeight();

			position.y = targetY;
			velocity.y = 0.0f;
			isGround = true;
		}
		//Player2の上から外れたら肩車を解除する
		else
		{
			isRiding = false;
			ridingTarget = nullptr;
			isGround = false;
		}
	}


	//コライダーを現在位置に合わせる
	bodyCollider.SetCenter({ position.x, position.y - 0.1f, position.z });
	bodyCollider.SetSize({ 0.5f, 0.1f, 0.5f });

	//弾丸入力処理
	//一応、操作中のプレイヤーだけ弾をてつ
	if (canControl)
	{
		InputProjectile();
	}

	//アニメーション更新処理
	animation.UpdateAnimation(elapsedTime);

	//弾丸更新処理
	projectileManager.Update(elapsedTime);

	//無敵時間更新
	UpdateInvincibleTimer(elapsedTime);

	//死んだら回復
	if (health <= 0)	{ health = 100;}

	//プレイヤーと敵との衝突処理
	CollisionPlayerVsEnemies();

	//弾丸と敵の衝突処理
	CollisionProjectilesVsEnemies();
	
	//プレイヤーとステージオブジェクトの衝突処理
	CollisionPlayerVsStage();

	//オブジェクト行列を更新
	UpdateTransform();

	//モデル行列更新
	model->UpdateTransform();
}

//描画処理
void Player::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	renderer->Render(rc, transform, model.get(), ShaderId::Lambert);

	//弾丸描画処理
	projectileManager.Render(rc, renderer);
}

//デバッグプリミティブ描画
void Player::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
	//基底クラスの関数の呼び出し
	Character::RenderDebugPrimitive(rc, renderer);

	//弾丸デバッグプリミティブ描画
	projectileManager.RenderDebugPrimitive(rc, renderer);

	//BoxColliderのデバッグプリミティブ描画
	renderer->RenderBox(rc, bodyCollider.GetCenter(), { 0,0,0 }, bodyCollider.GetSize(), DirectX::XMFLOAT4(0, 1, 0, 1));
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



//���n�����Ƃ��ɌĂ΂��
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

	//旋回処理
	Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);
}


//プレイヤーとエネミーとの衝突処理
void Player::CollisionPlayerVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();

	//全ての敵と総当たりで衝突処理
	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0;i < enemyCount;++i)
	{
		Enemy* enemy = enemyManager.GetEnemy(i);

		//衝突処理
		DirectX::XMFLOAT3 outPosition;
		//球と球のほう
		//if (Collision::IntersectSphereVsSphere(position, radius,
		//	enemy->GetPosition(), enemy->GetRadius(), 
		//	outPosition))
		//{
		//	//押し出し後の位置設定
		//	outPosition.x += enemy->GetPosition().x;
		//	outPosition.y += enemy->GetPosition().y;
		//	outPosition.z += enemy->GetPosition().z;
		//	enemy->SetPosition(outPosition);
		//}
		if (Collision::IntersectCylinderVsCylinder(
			position, radius, height, enemy->GetPosition(),
			enemy->GetRadius(), enemy->GetHeight(), outPosition
		))
		{
			//前で先生がやってるやつ
			DirectX::HXMVECTOR P = DirectX::XMLoadFloat3(&position);
			DirectX::HXMVECTOR E = DirectX::XMLoadFloat3(&enemy->GetPosition());
			DirectX::HXMVECTOR V = DirectX::XMVectorSubtract(P, E);
			DirectX::HXMVECTOR N = DirectX::XMVector3Normalize(V);
			DirectX::XMFLOAT3 normal;
			DirectX::XMStoreFloat3(&normal, N);
			float enemyTop = enemy->GetPosition().y + enemy->GetHeight();
			//たぶんベクトルでやる判定　ｘｚ平面のベクトルでうまいことやりそう
			/*if (enemyTop <= position.y - velocity.y)
			{
				自分でやったやつ
				velocity.y = JumpSpeed*0.5f;
			}*/
			//上から踏んだ時ジャンプ（プレイヤーから敵へのベクトルでやってる？）
			if (normal.y > 0.8f)
			{
				//ダメージを与える
				enemy->ApplyDamage(1, 0.5f);

				Jump(JumpSpeed * 0.5f);
			}
			//押し出し後の位置設定
			else
				enemy->SetPosition(outPosition);
		}
	}
}

//弾丸と敵の衝突処理
void Player::CollisionProjectilesVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();

	//全ての弾丸とすべての敵を総当たりで衝突処理
	int projectileCount = projectileManager.GetProjectileCount();
	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0;i < projectileCount;i++)
	{
		Projectile* projectile = projectileManager.GetProjectile(i);
		for (int j = 0;j < enemyCount;++j)
		{
			Enemy* enemy = enemyManager.GetEnemy(j);

			//衝突処理
			DirectX::XMFLOAT3 outPosition;
			if (Collision::IntersectSphereVsCylinder(
				projectile->GetPosition(),
				projectile->GetRadius(),
				enemy->GetPosition(),
				enemy->GetRadius(),
				enemy->GetHeight(),
				outPosition))
			{
				//ダメージを与える
				if (enemy->ApplyDamage(1, 0.5f))
				{
					//吹き飛ばす
					{
						DirectX::XMFLOAT3 impulse;
						DirectX::XMVECTOR pPos = DirectX::XMLoadFloat3(&projectile->GetPosition());
						DirectX::XMVECTOR ePos = DirectX::XMLoadFloat3(&enemy->GetPosition());

						DirectX::XMVECTOR dir = DirectX::XMVectorSubtract(ePos, pPos);//向き
						dir = DirectX::XMVector3Normalize(dir);//正規化

						float power = 10.0f;
						dir = DirectX::XMVectorScale(dir, power);

						DirectX::XMStoreFloat3(&impulse, dir);

						//上には少しだけはねる
						impulse.y = power * 0.5f;


						enemy->AddImpulse(impulse);
					}

					//ヒットエフェクト再生
					{
						DirectX::XMFLOAT3 e = enemy->GetPosition();
						e.y += enemy->GetHeight() * 0.5f;
						hitEffect->Play(e);
					}

					//ヒットSE再生
					{
						hitSE->Play(false);
					}

					//弾丸破棄
					projectile->Destroy();
				}
			}
		}
	}
}

//ステージとの衝突処理
void Player::CollisionPlayerVsStage()
{
	StageObjectManager& stageObjectManager = StageObjectManager::Instance();

	//とりあえずレーザーだけ
	LaserManager* laserManager = stageObjectManager.GetLaserManager();	

	for (int i = 0;i < laserManager->GetLaserCount();++i)
	{
		Laser* laser = laserManager->GetLaser(i);
		if (!laser)	continue;
		//レーザーの当たり判定をプレイヤーのコライダーと衝突判定

		CollisionResult result = bodyCollider.Intersect(laser->GetTopCollider());

		if (result.hit&&velocity.y<=0)
		{
			// 上から乗った場合
			if (result.normal.y > 0.5f)
			{
				velocity.y = 0.0f;
				position.y += result.pushOut.y;
				isGround = true;
				OnLanding();
			}
		}

		// 側面
		result = bodyCollider.Intersect(laser->GetSideCollider());

		if (result.hit)
		{
			position.x += result.pushOut.x;
			position.z += result.pushOut.z;
		}
	}
}

//プレイヤー同士の衝突処理
//操作中のプレイヤーだけを押し戻し、待機中のプレイヤーは動かさない
void Player::CollisionVsPlayer(Player& other, bool canRide)
{
	DirectX::XMFLOAT3 otherPosition = other.GetPosition();
		
	float dx = position.x - otherPosition.x;
	float dz = position.z - otherPosition.z;

	//XZ平面で2人の距離を計算する
	float distanceSq = dx * dx + dz * dz;
	float minDistance = radius + other.GetRadius();

	//距離が半径の合計以上なら衝突していないので、押し戻さない
	if (distanceSq >= minDistance * minDistance)
	{
		return;
	}

	//肩車できる場合は肩車状態にする
	if (canRide)
	{
		float otherTop = otherPosition.y + other.GetHeight();

		//自分が相手の上にいる場合は、肩車にしない
		bool isAboveOther = position.y >= otherTop - 0.2f; //判定(ゆるさ)

		if (!isAboveOther && !isRiding && rideTimer <= 0.0f)
		{
			StartRiding(other);
		}

		return;
	}


	//完全に同じ位置にいる場合は、X方向へ押し戻す
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
		//自分だけ押し戻す
		position.x += dx * push;
		position.z += dz * push;

		UpdateTransform();
	}
}


//ジャンプ入力処理
bool Player::InputJump()
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	if (gamePad.GetButtonDown() & GamePad::BTN_A)
	{
		if (JumpCount < JumpLimit)
		{
			velocity.y = 0.0f;
			Jump(JumpSpeed);
			JumpCount++;
			return true;
		}
	}
	return false;
}

//弾丸入力処理
void Player::InputProjectile()
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	//直進弾丸発射
	if (gamePad.GetButtonDown() & GamePad::BTN_X)
	{
		//前方向
		DirectX::XMFLOAT3 dir;
		dir.x = sinf(angle.y);
		dir.y = 0.0f;
		dir.z = cosf(angle.y);

		//発射位置（プレイヤーの腰当たり）
		DirectX::XMFLOAT3 pos;
		pos.x = position.x;
		pos.y = position.y + 0.5f;
		pos.z = position.z;

		//発射
		ProjectileStraight* projectile = new ProjectileStraight(&projectileManager);
		projectile->Launch(dir, pos);
	}

	//追尾弾発射
	if (gamePad.GetButtonDown() & GamePad::BTN_Y)
	{
		//前方向
		DirectX::XMFLOAT3 dir;
		dir.x = sinf(angle.y);
		dir.y = 0.0f;
		dir.z = cosf(angle.y);

		//発射位置（プレイヤーの腰当たり）
		DirectX::XMFLOAT3 pos;
		pos.x = position.x;
		pos.y = position.y + 0.5f;
		pos.z = position.z;

		//ターゲット（デフォルトではプレイヤーの前方）
		DirectX::XMFLOAT3 target;
		target.x = dir.x;
		target.y = dir.y;
		target.z = dir.z;

		//一番近くの敵をターゲットにする
		float dist = FLT_MAX;
		EnemyManager& enemyManager = EnemyManager::Instance();
		int enemyCount = enemyManager.GetEnemyCount();
		for (int i = 0;i < enemyCount;++i)
		{
			Enemy* enemy = enemyManager.GetEnemy(i);
			if (!enemy)	continue;
			//敵との距離判定

			DirectX::XMFLOAT3 epos = enemy->GetPosition();

			float dx = epos.x - pos.x;
			float dy = epos.y - pos.y;
			float dz = epos.z - pos.z;
			float distSq = dx * dx + dy * dy + dz * dz;

			if (distSq < dist)
			{
				dist = distSq;
				target = epos;
				target.y += enemy->GetHeight() * 0.5f;
			}
		}

		//発射
		ProjectileHoming* projectile = new ProjectileHoming(&projectileManager);
		projectile->Launch(dir, pos, target);
	}
}

//操作対象から外れたときに移動を止め、待機状態に戻す
void Player::StopControl()
{
	velocity.x = 0.0f;
	velocity.z = 0.0f;
	ChangeState(std::make_unique<IdleState>());
}

//肩車開始処理
void Player::StartRiding(Player& target)
{
	//肩車中にする
	isRiding = true;

	//乗る相手を保存する
	ridingTarget = &target;

	//前の移動入力や速度を消す
	ResetMove();

	//肩車状態へ切り替える
	ChangeState(std::make_unique<RideState>());
}

//肩車状態の更新処理
void Player::UpdateRiding(float elapsedTime)
{
	if (ridingTarget == nullptr)
	{
		isRiding = false;
		ChangeState(std::make_unique<IdleState>());
		return;
	}

	DirectX::XMFLOAT3 targetPosition = ridingTarget->GetPosition();

	//乗っている相手の位置に合わせる
	position.x = targetPosition.x;
	position.z = targetPosition.z;

	//肩車する高さを計算する
	float targetY = targetPosition.y + ridingTarget->GetHeight();

	//肩車位置まで上がる速さ
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

	bodyCollider.SetCenter({ position.x, position.y - 0.1f, position.z });
	bodyCollider.SetSize({ 0.5f, 0.1f, 0.5f });

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


bool Player::IsRideReady() const
{
	if (ridingTarget == nullptr)
	{
		return false;
	}

	float targetY = ridingTarget->GetPosition().y + ridingTarget->GetHeight();

	return position.y >= targetY - 0.01f;
}
