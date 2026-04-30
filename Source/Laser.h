#pragma once
#include<memory>
#include"StageObject.h"
#include"StageObjectManager.h"
#include"BoxCollider.h"

struct LaserSegment
{
	DirectX::XMFLOAT3 start;
	DirectX::XMFLOAT3 end;
};

struct LaserHit
{
	bool hit = false;
	DirectX::XMFLOAT3 point;
	DirectX::XMFLOAT3 normal;
	float penetration = 0.0f;//�����蔻��̊ђʗʁi�����j
};

class LaserBeam
{
public:
	DirectX::XMFLOAT3 origin;
	DirectX::XMFLOAT3 direction;
	float maxLength = 50.0f;
	float isActive = true;
	float radius = 0.5f;
	int maxReflection = 3;
	bool isRotating = false;


	std::vector<LaserSegment> segments;

	void Update(float elaspedTime);
	LaserHit CheckHitAABB(const BoxCollider& box) const;

	

	void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
	{
		// LaserBeam �̐�����`��
		for (auto& seg : segments)
		{
			// start �� end �̃x�N�g��
			DirectX::XMVECTOR s = DirectX::XMLoadFloat3(&seg.start);
			DirectX::XMVECTOR e = DirectX::XMLoadFloat3(&seg.end);

			// ���S�ʒu
			DirectX::XMVECTOR c = DirectX::XMVectorScale(DirectX::XMVectorAdd(s , e) , 0.5f);
			DirectX::XMFLOAT3 center;
			DirectX::XMStoreFloat3(&center, c);

			// ����
			float length = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(e , s)));
			length *= 0.5f; // Box���f�����u���S����O��0.5���v�Ȃ�A�����𔼕��ɂ���K�v������܂��B

			// �����x�N�g��
			DirectX::XMVECTOR dir = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(e , s));

			// Z������[�U�[�����ɍ��킹���]
			DirectX::XMVECTOR z = dir;
			DirectX::XMVECTOR y = DirectX::XMVectorSet(0, 1, 0, 0);
			DirectX::XMVECTOR x = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(y, z));
			y = DirectX::XMVector3Cross(z, x);

			DirectX::XMFLOAT4X4 rot;
			DirectX::XMStoreFloat4x4(&rot, DirectX::XMMatrixSet(
				DirectX::XMVectorGetX(x), DirectX::XMVectorGetY(x), DirectX::XMVectorGetZ(x), 0,
				DirectX::XMVectorGetX(y), DirectX::XMVectorGetY(y), DirectX::XMVectorGetZ(y), 0,
				DirectX::XMVectorGetX(z), DirectX::XMVectorGetY(z), DirectX::XMVectorGetZ(z), 0,
				0, 0, 0, 1
			));

			// �T�C�Y�i���� 0.1�A���� length�j
			DirectX::XMFLOAT3 size = { radius, radius, length };

			//�p�x(�f�o�b�O�p)
			DirectX::XMFLOAT3 angle;

			// yaw�iY��]�j
			angle.y = atan2f(dir.m128_f32[0], dir.m128_f32[2]);

			// pitch�iX��]�j
			angle.x = -asinf(dir.m128_f32[1]);

			angle.z = 0.0f;


			// �`��
			renderer->RenderBox(rc, center, angle, size, DirectX::XMFLOAT4(1, 1, 0, 1));
		}
	}

	//�f�o�b�O�pGUI�`��
	void DrawDebugGUI();

private:
	/*bool RayCastMesh(
		const DirectX::XMFLOAT3& start,
		const DirectX::XMFLOAT3& end,
		const Model* model,
		DirectX::XMFLOAT3& hitPos,
		DirectX::XMFLOAT3& hitNormal
	) const;*/
};

//���[�U�[�̖{��
class Laser :public StageObject
{
	public:
	
		//�f�o�b�O�v���~�e�B�u�`��
		void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)override
		{
			StageObject::RenderDebugPrimitive(rc, renderer);
			//�r�[���̃f�o�b�O�`��
			beam.RenderDebugPrimitive(rc, renderer);
			DirectX::XMFLOAT3 angle = { 0, currentAngleY, 0 };
			renderer->RenderBox(rc, startPos, angle, { 1, 1, 0.1f }, DirectX::XMFLOAT4(1, 0, 0, 1));
		}

		//�f�o�b�O�pGUI�`��
		void DrawDebugGUI()
		{
			beam.DrawDebugGUI();
		}

public:
	Laser() {}
	~Laser() {}

	void Initialize(
		const DirectX::XMFLOAT3& emitterPos,
		const DirectX::XMFLOAT3& direction,
		float maxLength);

	void Update(float elapsedTime) override;
	void Render(const RenderContext& rc, ModelRenderer* renderer) override;

	void SetActive(bool flag) { isActive = flag; }
	bool IsActive() const { return isActive; }

	void setManager(StageObjectManager* mgr) { manager = mgr; }

	//レーザー回転
	void RotateAroundCenter(const DirectX::XMFLOAT3& center, float angleY);

	LaserBeam& GetBeam() { return beam; }
	
	bool IsRotating() const { return isRotating; }
	
	const LaserBeam& GetBeam() const { return beam; }

private:
	LaserBeam beam;  // �� ���ꂪ���[�U�[�̖{��

	DirectX::XMFLOAT3 startPos;
	DirectX::XMFLOAT3 direction;
	float maxLength = 20.0f;

	bool isActive = true;

	void ResolvePlayerCollision();
	StageObjectManager* manager = nullptr;
};