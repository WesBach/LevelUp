#include "cEnemy.h"

void cEnemy::attack(double deltaTime) {
	//TODO:: make the enemy attack
}


void cEnemy::removeProjectile(cGameObject* theProjectile) {

	for (std::vector<cGameObject*>::iterator it = this->projectilesToDraw.begin(); it != this->projectilesToDraw.end(); it++) {
		//remove the projectile from the drawing vector
		if (*it == theProjectile) {
			it = this->projectilesToDraw.erase(it);
			break;
		}
	}

	for (int i = 0; i < this->projectilePool.size(); i++)
	{
		if (this->projectilePool[i].object == theProjectile)
		{
			this->projectilePool[i].inUse = false;
		}
	}
}


int cEnemy::getNextProjectileIndex() {
	int index = -1;

	for (int i = 0; i < this->projectilePool.size(); i++) {
		if (this->projectilePool[i].inUse == false) {
			index = i;
			this->projectilePool[i].inUse = true;
			break;
		}
	}

	return index;
}