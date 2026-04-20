#include"EnemyManager.h"
#include"Collision.h"

//更新処理
void EnemyManager::Update(float elapsedTime)
{
	//範囲for文で持っているエネミーのUpdateを実行
	for (Enemy* enemy : enemies)
	{
		enemy->Update(elapsedTime);
	}
	//破棄処理
	//※enemiesの範囲for文中でerase()すると不具合が発生してしまうため、
	// 更新処理が終わった後に破棄リストに積まれたオブジェクトを削除する
	for (Enemy* enemy : removes)
	{
		//std::vectorから要素を削除する場合はイテレーターで削除しなければならない
		std::vector<Enemy*>::iterator it = std::find(enemies.begin(),
			enemies.end(), enemy);
		if (it != enemies.end())
		{
			enemies.erase(it);
		}
		delete enemy;
	}
	removes.clear();
	//敵同士の衝突処理
	CollisionEnemyVsEnemies();
}

//描画処理
void EnemyManager::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	for (Enemy* enemy : enemies)
	{
		enemy->Render(rc, renderer);
	}
}

//エネミー登録
void EnemyManager::Register(Enemy* enemy)
{
	enemies.emplace_back(enemy);
}

//エネミー全削除
void EnemyManager::Clear()
{
	for (Enemy* enemy : enemies)
	{
		delete enemy;
	}
	enemies.clear();
}

//デバッグプリミティブ描画
void EnemyManager::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
	for (Enemy* enemy : enemies)
	{
		enemy->RenderDebugPrimitive(rc, renderer);
	}
}

//エネミー同士の衝突処理
void EnemyManager::CollisionEnemyVsEnemies()
{
	//全ての敵と総当たりで衝突処理
	int enemyCount = GetEnemyCount();
	for (int i = 0;enemyCount > i;i++)
	{
		for (int n = i+1;enemyCount > n;n++)
		{
			DirectX::XMFLOAT3 outPosition;
			//球と球のほう
			//if (Collision::IntersectSphereVsSphere(enemies[i]->GetPosition(), enemies[i]->GetRadius(),
			//	enemies[n]->GetPosition(), enemies[n]->GetRadius(), outPosition))
			//{
			//	{
			//		//押し出し後の位置設置
			//		outPosition.x += enemies[n]->GetPosition().x;
			//		outPosition.y += enemies[n]->GetPosition().y;
			//		outPosition.z += enemies[n]->GetPosition().z;
			//		enemies[n]->SetPosition(outPosition);
			//	}
			//}
			if (Collision::IntersectCylinderVsCylinder(enemies[i]->GetPosition(), enemies[i]->GetRadius(), enemies[i]->GetHeight(),
				enemies[n]->GetPosition(), enemies[n]->GetRadius(), enemies[n]->GetHeight(), outPosition))
			{
				enemies[n]->SetPosition(outPosition);
			}
		}
	}
}

//エネミー削除
void EnemyManager::Remove(Enemy* enemy)
{
	//破棄リストに追加
	removes.insert(enemy);
}