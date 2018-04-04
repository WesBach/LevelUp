#ifndef _cPlayer_HG_
#define _cPlayer_HG_
#include <glm\vec3.hpp>
#include <vector>
#include "sProjectile.h"
class cGameObject;
class cEmitter;

class cPlayer {
public:
	cPlayer();
	~cPlayer();
	cGameObject* thePlayerObject;

	bool isInExplosionRadius;
	unsigned int currentHealth;
	unsigned int maxHealth;

	float timeInExplosion;
	float playerSpeed;
	float rotationSpeed;
	float projectileRange;

	void movePlayer(glm::vec3 amountToMove);
	void rotatePlayer(glm::vec3 rotationDirection);
	void removeProjectile(cGameObject* theProjectile);
	void setProjectileInUse(sProjectile& projectile);
	void setProjectileRange();
	void resetProjectiles();

	int getNextProjectileIndex();
	//for shooting
	glm::vec3 forwardVector;

	int maxProjectilesAllowed;
	int projectileIndex;

	std::vector<sProjectile> projectilePool;
	std::vector<sProjectile> projectiles;
	std::vector<cGameObject*> projectilesToDraw;
};

#endif
