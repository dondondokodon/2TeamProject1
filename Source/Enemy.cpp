#include"Enemy.h"
#include"EnemyManager.h"

//”jŠü
void Enemy::Destroy()
{
	//”jŠüƒŠƒXƒg‚É’Ç‰Á
	EnemyManager::Instance().Remove(this);
}