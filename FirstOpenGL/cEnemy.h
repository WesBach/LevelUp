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
	int particleManagerEmitterIndex;

	float projectileRange;
	float health;
	float maxHealth;

	bool isExploding;
	cGameObject* theEnemyObject;

	std::vector<cGameObject*> projectilesToDraw;
	std::vector<sProjectile> projectilePool;	
	std::vector<sProjectile> projectiles;
	//vector for sake of not writing another render function
	std::vector<cGameObject*> explosion;

	float timeElapsedBetweenProjectiles;
	float explosionSize;

	void attack(glm::vec3 direction,float deltaTime);
	void removeProjectile(cGameObject* theProjectile);
	void removeExplosion(cGameObject* theExplosion);
	sProjectile createProjectileBasedOnEnemyStats();
	int getNextProjectileIndex();
};

#endif // !_Enemy_HG_
