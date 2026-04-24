#include"Player.h"
#include"System/Input.h"
#include"System/Audio.h"
#include<imgui.h>
#include"Camera.h"
#include"EnemyManager.h"
#include"Collision.h"
#include"ProjectileStraight.h"
#include"ProjectileHoming.h"

//�R���X�g���N�^
void Player::Initialize()
{
	//model = new Model("Data/Model/Mr.Incredible/Mr.Incredible.mdl");

	model = std::make_unique<Model>("Data/Model/Jammo/Jammo.mdl");

	//���f�����傫���̂ŃX�P�[�����O
	scale.x = scale.y = scale.z = 0.01f;

	//�q�b�g�G�t�F�N�g�ǂݍ���
	hitEffect = new Effect("Data/Effect/Hit.efk");

	//�q�b�gSE�ǂݍ���
	hitSE = Audio::Instance().LoadAudioSource("Data/Sound/Hit.wav");

	//�A�j���[�V�����R���g���[���[�Ƀ��f����Z�b�g
	animation.setModel(model.get());

	//�����X�e�[�g��IdleState�ɐݒ�
	state = std::make_unique<IdleState>();
	state->Initialize(*this);
}

//�f�X�g���N�^
void Player::Finalize()
{
	delete hitSE;
	delete hitEffect;
	//delete model;
}

//�X�e�[�g�ύX����
void Player::ChangeState(std::unique_ptr<PlayerState> newState)
{
	//���݂̃X�e�[�g�̏I������
	if (state)
	{
		state->Finalize(*this);
	}
	//�V�����X�e�[�g�ɕύX
	state = std::move(newState);
	//�V�����X�e�[�g�̏���������
	if (state)
	{
		state->Initialize(*this);
	}
}

//�X�V����
void Player::Update(float elapsedTime)
{
	//�X�e�[�g�̍X�V����
	if(state)
	{
		state->Update(*this, elapsedTime);
	}

	//�ړ����͏���
	//InputMove(elapsedTime);

	//�W�����v���͏���
	//InputJump();

	//���͏����X�V
	UpdateVelocity(elapsedTime);

	//�e�ۓ��͏���
	InputProjectile();

	//�A�j���[�V�����X�V����
	animation.UpdateAnimation(elapsedTime);

	//�e�ۍX�V����
	projectileManager.Update(elapsedTime);

	//���G���ԍX�V
	UpdateInvincibleTimer(elapsedTime);

	//����łق����Ȃ��̂Ŏ��񂾂��
	if (health <= 0)	health = 100;

	//�v���C���[�ƓG�Ƃ̏Փˏ���
	CollisionPlayerVsEnemies();

	//�e�ۂƓG�̏Փˏ���
	CollisionProjectilesVsEnemies();

	//�I�u�W�F�N�g�s���X�V
	UpdateTransform();

	//���f���s��X�V
	model->UpdateTransform();
}

//�`�揈��
void Player::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	renderer->Render(rc, transform, model.get(), ShaderId::Lambert);

	//�e�ە`�揈��
	projectileManager.Render(rc, renderer);
}

//�X�V����
void Player::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
	//���N���X�̊֐��̌Ăяo��
	Character::RenderDebugPrimitive(rc, renderer);

	//�e�ۃf�o�b�O�v���~�e�B�u�`��
	projectileManager.RenderDebugPrimitive(rc, renderer);
}

//�f�o�b�O�pGUI�`��
void Player::DrawDebugGUI()
{
	ImVec2 pos = ImGui::GetMainViewport()->GetWorkPos();
	ImGui::SetNextWindowPos(ImVec2(pos.x + 10, pos.y + 10), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None))
	{
		//�g�����X�t�H�[��
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			//�ʒu
			ImGui::InputFloat3("Position", &position.x);
			//��]
			DirectX::XMFLOAT3 a;
			a.x = DirectX::XMConvertToDegrees(angle.x);
			a.y = DirectX::XMConvertToDegrees(angle.y);
			a.z = DirectX::XMConvertToDegrees(angle.z);
			ImGui::InputFloat3("Angle", &a.x);
			angle.x = DirectX::XMConvertToRadians(a.x);
			angle.y = DirectX::XMConvertToRadians(a.y);
			angle.z = DirectX::XMConvertToRadians(a.z);
			//�X�P�[��	
			ImGui::InputFloat3("Scale", &scale.x);
		}
	}
	ImGui::End();
}

DirectX::XMFLOAT3 Player::GetMoveVec()const
{
	//���͏���擾
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ax = gamePad.GetAxisLX();
	float ay = gamePad.GetAxisLY();

	//�J���������ƃX�e�B�b�N�̓��͒l�ɂ���Đi�s������v�Z����
	Camera& camera = Camera::Instance();
	const DirectX::XMFLOAT3& cameraRight = camera.GetRight();
	const DirectX::XMFLOAT3& cameraFront = camera.GetFront();

	//�ړ��x�N�g����XZ���ʂɐ����ȃx�N�g���ɂȂ�悤�ɂ���

	//�J�����E�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�
	float cameraRightX = cameraRight.x;
	float cameraRightZ = cameraRight.z;
	float cameraRightLength = sqrtf(cameraRightX * cameraRightX+cameraRightZ*cameraRightZ);
	if (cameraRightLength > 0.0f)
	{
		//�P�ʃx�N�g����
		cameraRightX /= cameraRightLength;
		cameraRightZ /= cameraRightLength;
	}

	//�J�����O�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�
	float cameraFrontX = cameraFront.x;
	float cameraFrontZ = cameraFront.z;
	float cameraFrontLength = sqrtf(cameraFrontX *cameraFrontX + cameraFrontZ*cameraFrontZ);
	if (cameraFrontLength > 0.0f)
	{
		//�P�ʃx�N�g����
		cameraFrontX /= cameraFrontLength;
		cameraFrontZ /= cameraFrontLength;
	}

	//�X�e�B�b�N�̐������͒l��J�����E�����ɔ��f���A
	//�X�e�B�b�N�̐������͒l��J�����O�����ɔ��f���A
	//�i�s�x�N�g����v�Z����
	DirectX::XMFLOAT3 vec;
	/*vec.x = cameraRightX * ax+cameraRightZ*ay;
	vec.z = cameraFrontX * ax+cameraFrontZ*ay;*/

	vec.x = cameraRightX * ax + cameraFrontX * ay;
	vec.z = cameraRightZ * ax + cameraFrontZ * ay;
	//Y�������ɂ͈ړ����Ȃ�
	vec.y = 0.0f;

	return vec;
}

//���n�����Ƃ��ɌĂ΂��
void Player::OnLanding()
{
	JumpCount = 0;
}

//�ړ����͏���
void Player::InputMove(float elapsedTime)
{
	//�i�s�x�N�g���擾
	DirectX::XMFLOAT3 moveVec = GetMoveVec();

	//�ړ�����
	Move(elapsedTime, moveVec.x, moveVec.z, moveSpeed);

	//���񏈗�
	Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);
}


//�v���C���[�ƃG�l�~�[�Ƃ̏Փˏ���
void Player::CollisionPlayerVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();

	//�S�Ă̓G�Ƒ�������ŏՓˏ���
	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0;i < enemyCount;++i)
	{
		Enemy* enemy = enemyManager.GetEnemy(i);

		//�Փˏ���
		DirectX::XMFLOAT3 outPosition;
		//���Ƌ��̂ق�
		//if (Collision::IntersectSphereVsSphere(position, radius,
		//	enemy->GetPosition(), enemy->GetRadius(), 
		//	outPosition))
		//{
		//	//�����o����̈ʒu�ݒ�
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
			//�O�Ő搶������Ă���
			DirectX::HXMVECTOR P = DirectX::XMLoadFloat3(&position);
			DirectX::HXMVECTOR E = DirectX::XMLoadFloat3(&enemy->GetPosition());
			DirectX::HXMVECTOR V = DirectX::XMVectorSubtract(P, E);
			DirectX::HXMVECTOR N = DirectX::XMVector3Normalize(V);
			DirectX::XMFLOAT3 normal;
			DirectX::XMStoreFloat3(&normal, N);
			float enemyTop = enemy->GetPosition().y + enemy->GetHeight();
			//���Ԃ�x�N�g���ł�锻��@�������ʂ̃x�N�g���ł��܂����Ƃ�肻��
			/*if (enemyTop <= position.y - velocity.y)
			{
			    �����ł�������
				velocity.y = JumpSpeed*0.5f;
			}*/
			//�ォ�瓥�񂾎��W�����v�i�v���C���[����G�ւ̃x�N�g���ł���Ă�H�j
			if (normal.y > 0.8f)
			{
				//�_���[�W��^����
				enemy->ApplyDamage(1,0.5f);

				Jump(JumpSpeed * 0.5f);
			}
			//�����o����̈ʒu�ݒ�
			else
			enemy->SetPosition(outPosition);
		}
	}
}

//�e�ۂƓG�̏Փˏ���
void Player::CollisionProjectilesVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();

	//�S�Ă̒e�ۂƂ��ׂĂ̓G�𑍓�����ŏՓˏ���
	int projectileCount = projectileManager.GetProjectileCount();
	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0;i < projectileCount;i++)
	{
		Projectile* projectile = projectileManager.GetProjectile(i);
		for (int j = 0;j < enemyCount;++j)
		{
			Enemy* enemy = enemyManager.GetEnemy(j);

			//�Փˏ���
			DirectX::XMFLOAT3 outPosition;
			if (Collision::IntersectSphereVsCylinder(
				projectile->GetPosition(),
				projectile->GetRadius(),
				enemy->GetPosition(),
				enemy->GetRadius(),
				enemy->GetHeight(),
				outPosition))
			{
				//�_���[�W��^����
				if (enemy->ApplyDamage(1, 0.5f))
				{
					//������΂�
					{
						DirectX::XMFLOAT3 impulse;
						DirectX::XMVECTOR pPos = DirectX::XMLoadFloat3(&projectile->GetPosition());
						DirectX::XMVECTOR ePos = DirectX::XMLoadFloat3(&enemy->GetPosition());

						DirectX::XMVECTOR dir = DirectX::XMVectorSubtract(ePos, pPos);//����
						dir = DirectX::XMVector3Normalize(dir);//���K��

						float power = 10.0f;
						dir = DirectX::XMVectorScale(dir, power);

						DirectX::XMStoreFloat3(&impulse, dir);

						//��ɂ͏��������͂˂�
						impulse.y=power*0.5f;
						

						enemy->AddImpulse(impulse);
					}

					//�q�b�g�G�t�F�N�g�Đ�
					{
						DirectX::XMFLOAT3 e = enemy->GetPosition();
						e.y += enemy->GetHeight() * 0.5f;
						hitEffect->Play(e);
					}

					//�q�b�gSE�Đ�
					{
						hitSE->Play(false);
					}

					//�e�۔j��
					projectile->Destroy();
				}
			}
		}
	}
}

//�W�����v���͏���
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

//�e�ۓ��͏���
void Player::InputProjectile()
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	//���i�e�۔���
	if (gamePad.GetButtonDown() & GamePad::BTN_X)
	{
		//�O����
		DirectX::XMFLOAT3 dir;
		dir.x = sinf(angle.y);
		dir.y = 0.0f;
		dir.z = cosf(angle.y);

		//���ˈʒu�i�v���C���[�̍�������j
		DirectX::XMFLOAT3 pos;
		pos.x = position.x;
		pos.y = position.y+0.5f;
		pos.z = position.z;

		//����
		ProjectileStraight* projectile = new ProjectileStraight(&projectileManager);
		projectile->Launch(dir, pos);
	}

	//�ǔ��e����
	if (gamePad.GetButtonDown() & GamePad::BTN_Y)
	{
		//�O����
		DirectX::XMFLOAT3 dir;
		dir.x = sinf(angle.y);
		dir.y = 0.0f;
		dir.z = cosf(angle.y);

		//���ˈʒu�i�v���C���[�̍�������j
		DirectX::XMFLOAT3 pos;
		pos.x = position.x;
		pos.y = position.y + 0.5f;
		pos.z = position.z;

		//�^�[�Q�b�g�i�f�t�H���g�ł̓v���C���[�̑O���j
		DirectX::XMFLOAT3 target;
		target.x = dir.x;
		target.y = dir.y;
		target.z = dir.z;

		//��ԋ߂��̓G��^�[�Q�b�g�ɂ���
		float dist = FLT_MAX;
		EnemyManager& enemyManager = EnemyManager::Instance();
		int enemyCount = enemyManager.GetEnemyCount();
		for (int i = 0;i < enemyCount;++i)
		{
			Enemy* enemy = enemyManager.GetEnemy(i);
			if (!enemy)	continue;
			//�G�Ƃ̋�������
			
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

		//����
		ProjectileHoming* projectile = new ProjectileHoming(&projectileManager);
		projectile->Launch(dir, pos,target);
	}
}