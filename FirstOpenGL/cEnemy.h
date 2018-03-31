#ifndef _cEnemy_HG_
#define _cEnemy_HG_
#include "sProjectile.h"
#include <vector>

enum eEnemyType {
	GUNNER,
	SUICIDE
};

enum eAttackType {
	PROJECTILE,
	EXPLOSION
};

class cEnemy {
public:
	//for attacking
	eEnemyType enemyType;
	eAttackType attackType;
	glm::vec3 direction;

	cGameObject* theEnemyObject;

	std::vector<cGameObject*> projectilesToDraw;
	std::vector<sProjectile> projectilePool;

	void attack(double deltaTime);
	void removeProjectile(cGameObject* theProjectile);
	int getNextProjectileIndex();
};

#endif // !_Enemy_HG_
