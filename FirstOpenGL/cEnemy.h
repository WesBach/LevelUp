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
	cEnemy();
	~cEnemy();
	//for attacking
	eEnemyType enemyType;
	eAttackType attackType;
	glm::vec3 direction;
	float projectileRange;
	float health;

	cGameObject* theEnemyObject;

	std::vector<cGameObject*> projectilesToDraw;
	std::vector<sProjectile> projectilePool;	
	std::vector<sProjectile> projectiles;
	float timeElapsedBetweenProjectiles;

	void attack(glm::vec3 direction,float deltaTime);
	void removeProjectile(cGameObject* theProjectile);
	sProjectile createProjectileBasedOnEnemyStats();
	int getNextProjectileIndex();
};

#endif // !_Enemy_HG_
