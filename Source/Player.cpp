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

////�R���X�g���N�^
//void Player::Initialize()
//{
//	//model = new Model("Data/Model/Mr.Incredible/Mr.Incredible.mdl");
//
//	model = std::make_unique<Model>("Data/Model/Jammo/Jammo.mdl");
//
//	//���f�����傫���̂ŃX�P�[�����O
//	scale.x = scale.y = scale.z = 0.01f;
//
//	//�q�b�g�G�t�F�N�g�ǂݍ���
//	hitEffect = new Effect("Data/Effect/Hit.efk");
//
//	//�q�b�gSE�ǂݍ���
//	hitSE = Audio::Instance().LoadAudioSource("Data/Sound/Hit.wav");
//
//	//�A�j���[�V�����R���g���[���[�Ƀ��f����Z�b�g
//	animation.setModel(model.get());
//
//	//�����X�e�[�g��IdleState�ɐݒ�
//	state = std::make_unique<IdleState>();
//	state->Initialize(*this);
//}
//
////�f�X�g���N�^
//void Player::Finalize()
//{
//	delete hitSE;
//	delete hitEffect;
//	//delete model;
//}
//
////�X�e�[�g�ύX����
//void Player::ChangeState(std::unique_ptr<PlayerState> newState)
//{
//	//���݂̃X�e�[�g�̏I������
//	if (state)
//	{
//		state->Finalize(*this);
//	}
//	//�V�����X�e�[�g�ɕύX
//	state = std::move(newState);
//	//�V�����X�e�[�g�̏���������
//	if (state)
//	{
//		state->Initialize(*this);
//	}
//}
//
////�X�V����
//void Player::Update(float elapsedTime)
//{
//	//�X�e�[�g�̍X�V����
//	if(state)
//	{
//		state->Update(*this, elapsedTime);
//	}
//
//	//�ړ����͏���
//	//InputMove(elapsedTime);
//
//	//�W�����v���͏���
//	//InputJump();
//
//	//���͏����X�V
//	UpdateVelocity(elapsedTime);
//
//	//�e�ۓ��͏���
//	InputProjectile();
//
//	//�A�j���[�V�����X�V����
//	animation.UpdateAnimation(elapsedTime);
//
//	//�e�ۍX�V����
//	projectileManager.Update(elapsedTime);
//
//	//���G���ԍX�V
//	UpdateInvincibleTimer(elapsedTime);
//
//	//����łق����Ȃ��̂Ŏ��񂾂��
//	if (health <= 0)	health = 100;
//
//	//�v���C���[�ƓG�Ƃ̏Փˏ���
//	CollisionPlayerVsEnemies();
//
//	//�e�ۂƓG�̏Փˏ���
//	CollisionProjectilesVsEnemies();
//
//	//�I�u�W�F�N�g�s���X�V
//	UpdateTransform();
//
//	//���f���s��X�V
//	model->UpdateTransform();
//}
//
////�`�揈��
//void Player::Render(const RenderContext& rc, ModelRenderer* renderer)
//{
//	renderer->Render(rc, transform, model.get(), ShaderId::Lambert);
//
//	//�e�ە`�揈��
//	projectileManager.Render(rc, renderer);
//}
//
////�X�V����
//void Player::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
//{
//	//���N���X�̊֐��̌Ăяo��
//	Character::RenderDebugPrimitive(rc, renderer);
//
//	//�e�ۃf�o�b�O�v���~�e�B�u�`��
//	projectileManager.RenderDebugPrimitive(rc, renderer);
//}
//
////�f�o�b�O�pGUI�`��
//void Player::DrawDebugGUI()
//{
//	ImVec2 pos = ImGui::GetMainViewport()->GetWorkPos();
//	ImGui::SetNextWindowPos(ImVec2(pos.x + 10, pos.y + 10), ImGuiCond_Once);
//	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
//
//	if (ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None))
//	{
//		//�g�����X�t�H�[��
//		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
//		{
//			//�ʒu
//			ImGui::InputFloat3("Position", &position.x);
//			//��]
//			DirectX::XMFLOAT3 a;
//			a.x = DirectX::XMConvertToDegrees(angle.x);
//			a.y = DirectX::XMConvertToDegrees(angle.y);
//			a.z = DirectX::XMConvertToDegrees(angle.z);
//			ImGui::InputFloat3("Angle", &a.x);
//			angle.x = DirectX::XMConvertToRadians(a.x);
//			angle.y = DirectX::XMConvertToRadians(a.y);
//			angle.z = DirectX::XMConvertToRadians(a.z);
//			//�X�P�[��	
//			ImGui::InputFloat3("Scale", &scale.x);
//		}
//	}
//	ImGui::End();
//}
//
//DirectX::XMFLOAT3 Player::GetMoveVec()const
//{
//	//���͏���擾
//	GamePad& gamePad = Input::Instance().GetGamePad();
//	float ax = gamePad.GetAxisLX();
//	float ay = gamePad.GetAxisLY();
//
//	//�J���������ƃX�e�B�b�N�̓��͒l�ɂ���Đi�s������v�Z����
//	Camera& camera = Camera::Instance();
//	const DirectX::XMFLOAT3& cameraRight = camera.GetRight();
//	const DirectX::XMFLOAT3& cameraFront = camera.GetFront();
//
//	//�ړ��x�N�g����XZ���ʂɐ����ȃx�N�g���ɂȂ�悤�ɂ���
//
//	//�J�����E�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�
//	float cameraRightX = cameraRight.x;
//	float cameraRightZ = cameraRight.z;
//	float cameraRightLength = sqrtf(cameraRightX * cameraRightX+cameraRightZ*cameraRightZ);
//	if (cameraRightLength > 0.0f)
//	{
//		//�P�ʃx�N�g����
//		cameraRightX /= cameraRightLength;
//		cameraRightZ /= cameraRightLength;
//	}
//
//	//�J�����O�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�
//	float cameraFrontX = cameraFront.x;
//	float cameraFrontZ = cameraFront.z;
//	float cameraFrontLength = sqrtf(cameraFrontX *cameraFrontX + cameraFrontZ*cameraFrontZ);
//	if (cameraFrontLength > 0.0f)
//	{
//		//�P�ʃx�N�g����
//		cameraFrontX /= cameraFrontLength;
//		cameraFrontZ /= cameraFrontLength;
//	}
//
//	//�X�e�B�b�N�̐������͒l��J�����E�����ɔ��f���A
//	//�X�e�B�b�N�̐������͒l��J�����O�����ɔ��f���A
//	//�i�s�x�N�g����v�Z����
//	DirectX::XMFLOAT3 vec;
//	/*vec.x = cameraRightX * ax+cameraRightZ*ay;
//	vec.z = cameraFrontX * ax+cameraFrontZ*ay;*/
//
//	vec.x = cameraRightX * ax + cameraFrontX * ay;
//	vec.z = cameraRightZ * ax + cameraFrontZ * ay;
//	//Y�������ɂ͈ړ����Ȃ�
//	vec.y = 0.0f;
//
//	return vec;
//}
//
////���n�����Ƃ��ɌĂ΂��
//void Player::OnLanding()
//{
//	JumpCount = 0;
//}
//
////�ړ����͏���
//void Player::InputMove(float elapsedTime)
//{
//	//�i�s�x�N�g���擾
//	DirectX::XMFLOAT3 moveVec = GetMoveVec();
//
//	//�ړ�����
//	Move(elapsedTime, moveVec.x, moveVec.z, moveSpeed);
//
//	//���񏈗�
//	Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);
//}
//
//
////�v���C���[�ƃG�l�~�[�Ƃ̏Փˏ���
//void Player::CollisionPlayerVsEnemies()
//{
//	EnemyManager& enemyManager = EnemyManager::Instance();
//
//	//�S�Ă̓G�Ƒ�������ŏՓˏ���
//	int enemyCount = enemyManager.GetEnemyCount();
//	for (int i = 0;i < enemyCount;++i)
//	{
//		Enemy* enemy = enemyManager.GetEnemy(i);
//
//		//�Փˏ���
//		DirectX::XMFLOAT3 outPosition;
//		//���Ƌ��̂ق�
//		//if (Collision::IntersectSphereVsSphere(position, radius,
//		//	enemy->GetPosition(), enemy->GetRadius(), 
//		//	outPosition))
//		//{
//		//	//�����o����̈ʒu�ݒ�
//		//	outPosition.x += enemy->GetPosition().x;
//		//	outPosition.y += enemy->GetPosition().y;
//		//	outPosition.z += enemy->GetPosition().z;
//		//	enemy->SetPosition(outPosition);
//		//}
//		if (Collision::IntersectCylinderVsCylinder(
//			position, radius, height, enemy->GetPosition(),
//			enemy->GetRadius(), enemy->GetHeight(), outPosition
//		))
//		{
//			//�O�Ő搶������Ă���
//			DirectX::HXMVECTOR P = DirectX::XMLoadFloat3(&position);
//			DirectX::HXMVECTOR E = DirectX::XMLoadFloat3(&enemy->GetPosition());
//			DirectX::HXMVECTOR V = DirectX::XMVectorSubtract(P, E);
//			DirectX::HXMVECTOR N = DirectX::XMVector3Normalize(V);
//			DirectX::XMFLOAT3 normal;
//			DirectX::XMStoreFloat3(&normal, N);
//			float enemyTop = enemy->GetPosition().y + enemy->GetHeight();
//			//���Ԃ�x�N�g���ł�锻��@�������ʂ̃x�N�g���ł��܂����Ƃ�肻��
//			/*if (enemyTop <= position.y - velocity.y)
//			{
//			    �����ł�������
//				velocity.y = JumpSpeed*0.5f;
//			}*/
//			//�ォ�瓥�񂾎��W�����v�i�v���C���[����G�ւ̃x�N�g���ł���Ă�H�j
//			if (normal.y > 0.8f)
//			{
//				//�_���[�W��^����
//				enemy->ApplyDamage(1,0.5f);
//
//				Jump(JumpSpeed * 0.5f);
//			}
//			//�����o����̈ʒu�ݒ�
//			else
//			enemy->SetPosition(outPosition);
//		}
//	}
//}
//
////�e�ۂƓG�̏Փˏ���
//void Player::CollisionProjectilesVsEnemies()
//{
//	EnemyManager& enemyManager = EnemyManager::Instance();
//
//	//�S�Ă̒e�ۂƂ��ׂĂ̓G�𑍓�����ŏՓˏ���
//	int projectileCount = projectileManager.GetProjectileCount();
//	int enemyCount = enemyManager.GetEnemyCount();
//	for (int i = 0;i < projectileCount;i++)
//	{
//		Projectile* projectile = projectileManager.GetProjectile(i);
//		for (int j = 0;j < enemyCount;++j)
//		{
//			Enemy* enemy = enemyManager.GetEnemy(j);
//
//			//�Փˏ���
//			DirectX::XMFLOAT3 outPosition;
//			if (Collision::IntersectSphereVsCylinder(
//				projectile->GetPosition(),
//				projectile->GetRadius(),
//				enemy->GetPosition(),
//				enemy->GetRadius(),
//				enemy->GetHeight(),
//				outPosition))
//			{
//				//�_���[�W��^����
//				if (enemy->ApplyDamage(1, 0.5f))
//				{
//					//������΂�
//					{
//						DirectX::XMFLOAT3 impulse;
//						DirectX::XMVECTOR pPos = DirectX::XMLoadFloat3(&projectile->GetPosition());
//						DirectX::XMVECTOR ePos = DirectX::XMLoadFloat3(&enemy->GetPosition());
//
//						DirectX::XMVECTOR dir = DirectX::XMVectorSubtract(ePos, pPos);//����
//						dir = DirectX::XMVector3Normalize(dir);//���K��
//
//						float power = 10.0f;
//						dir = DirectX::XMVectorScale(dir, power);
//
//						DirectX::XMStoreFloat3(&impulse, dir);
//
//						//��ɂ͏��������͂˂�
//						impulse.y=power*0.5f;
//						
//
//						enemy->AddImpulse(impulse);
//					}
//
//					//�q�b�g�G�t�F�N�g�Đ�
//					{
//						DirectX::XMFLOAT3 e = enemy->GetPosition();
//						e.y += enemy->GetHeight() * 0.5f;
//						hitEffect->Play(e);
//					}
//
//					//�q�b�gSE�Đ�
//					{
//						hitSE->Play(false);
//					}
//
//					//�e�۔j��
//					projectile->Destroy();
//				}
//			}
//		}
//	}
//}
//
////�W�����v���͏���
//bool Player::InputJump()
//{
//	GamePad& gamePad = Input::Instance().GetGamePad();
//	if (gamePad.GetButtonDown() & GamePad::BTN_A)
//	{
//		if (JumpCount < JumpLimit)
//		{
//			velocity.y = 0.0f;
//			Jump(JumpSpeed);
//			JumpCount++;
//			return true;
//		}
//	}
//	return false;
//}
//
////�e�ۓ��͏���
//void Player::InputProjectile()
//{
//	GamePad& gamePad = Input::Instance().GetGamePad();
//
//	//���i�e�۔���
//	if (gamePad.GetButtonDown() & GamePad::BTN_X)
//	{
//		//�O����
//		DirectX::XMFLOAT3 dir;
//		dir.x = sinf(angle.y);
//		dir.y = 0.0f;
//		dir.z = cosf(angle.y);
//
//		//���ˈʒu�i�v���C���[�̍�������j
//		DirectX::XMFLOAT3 pos;
//		pos.x = position.x;
//		pos.y = position.y+0.5f;
//		pos.z = position.z;
//
//		//����
//		ProjectileStraight* projectile = new ProjectileStraight(&projectileManager);
//		projectile->Launch(dir, pos);
//	}
//
//	//�ǔ��e����
//	if (gamePad.GetButtonDown() & GamePad::BTN_Y)
//	{
//		//�O����
//		DirectX::XMFLOAT3 dir;
//		dir.x = sinf(angle.y);
//		dir.y = 0.0f;
//		dir.z = cosf(angle.y);
//
//		//���ˈʒu�i�v���C���[�̍�������j
//		DirectX::XMFLOAT3 pos;
//		pos.x = position.x;
//		pos.y = position.y + 0.5f;
//		pos.z = position.z;
//
//		//�^�[�Q�b�g�i�f�t�H���g�ł̓v���C���[�̑O���j
//		DirectX::XMFLOAT3 target;
//		target.x = dir.x;
//		target.y = dir.y;
//		target.z = dir.z;
//
//		//��ԋ߂��̓G��^�[�Q�b�g�ɂ���
//		float dist = FLT_MAX;
//		EnemyManager& enemyManager = EnemyManager::Instance();
//		int enemyCount = enemyManager.GetEnemyCount();
//		for (int i = 0;i < enemyCount;++i)
//		{
//			Enemy* enemy = enemyManager.GetEnemy(i);
//			if (!enemy)	continue;
//			//�G�Ƃ̋�������
//			
//			DirectX::XMFLOAT3 epos = enemy->GetPosition();
//
//			float dx = epos.x - pos.x;
//			float dy = epos.y - pos.y;
//			float dz = epos.z - pos.z;
//			float distSq = dx * dx + dy * dy + dz * dz;
//
//			if (distSq < dist)
//			{
//				dist = distSq;
//				target = epos;
//				target.y += enemy->GetHeight() * 0.5f;
//			}
//		}
//
//		//����
//		ProjectileHoming* projectile = new ProjectileHoming(&projectileManager);
//		projectile->Launch(dir, pos,target);
//	}
//}

//なんか文字化けしてるから
// コンストラクタ
void Player::Initialize()
{
	//model = new Model("Data/Model/Mr.Incredible/Mr.Incredible.mdl");

	model = std::make_unique<Model>("Data/Model/Jammo/Jammo.mdl");

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
void Player::Update(float elapsedTime)
{
	//ステートの更新処理
	if (state)
	{
		state->Update(*this, elapsedTime);
	}

	//移動入力処理
	//InputMove(elapsedTime);

	//ジャンプ入力処理
	//InputJump();

	//速力処理更新
	UpdateVelocity(elapsedTime);

	//コライダーのセット
	bodyCollider.SetCenter({ position.x, position.y - 0.1f, position.z });
	bodyCollider.SetSize({ 0.5f,0.1,0.5f });

	//弾丸入力処理
	InputProjectile();

	//アニメーション更新処理
	animation.UpdateAnimation(elapsedTime);

	//弾丸更新処理
	projectileManager.Update(elapsedTime);

	//無敵時間更新
	UpdateInvincibleTimer(elapsedTime);

	//死んでほしくないので死んだら回復
	if (health <= 0)	health = 100;

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

//着地したときに呼ばれる
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