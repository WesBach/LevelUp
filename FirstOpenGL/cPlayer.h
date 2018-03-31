#ifndef _cPlayer_HG_
#define _cPlayer_HG_
#include <glm\vec3.hpp>
#include <vector>
#include "sProjectile.h"
class cGameObject;

class cPlayer {
public:
	cPlayer();
	~cPlayer();
	cGameObject* thePlayerObject;
	unsigned int currentHealth;
	unsigned int maxHealth;
	float playerSpeed;
	float rotationSpeed;
	void movePlayer(glm::vec3 amountToMove);
	void rotatePlayer(glm::vec3 rotationDirection);

	void removeProjectile(cGameObject* theProjectile);
	void setProjectileInUse(sProjectile& projectile);

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
