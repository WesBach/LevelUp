#include "cEnemy.h"
#include "cGameObject.h"

void cEnemy::attack(glm::vec3 direction,float deltaTime) {
	if (this->attackType == eAttackType::PROJECTILE)
	{
		//add the time to counter
		this->timeElapsedBetweenProjectiles += deltaTime;
		//get the projectile
		sProjectile projectile = this->createProjectileBasedOnEnemyStats();

		//check projectile state
		for (int i = 0; i < this->projectilePool.size(); i++) { //go through the pool 
			//try to find the projectile in the projectiles vector
			bool foundCurrentProjectile = false;
			
			for (int index = 0; index < this->projectiles.size(); index++) {
				if (this->projectilePool[i].object == this->projectiles[index].object) {
					foundCurrentProjectile = true;
				}
			}
			//if we didn't find the projectile set in use to false
			if (foundCurrentProjectile == false) {
				this->projectilePool[i].inUse = false;
			}
		}

		if (projectile.object != NULL) {
			if (this->timeElapsedBetweenProjectiles >= 0.5f) {
				//set the direction for the projectile
				projectile.direction = direction;
				projectile.object->position = this->theEnemyObject->position + projectile.direction;
				projectile.damage = 10.f;
				//push back the projectile
				this->projectilesToDraw.push_back(projectile.object);
				this->projectiles.push_back(projectile);
				//reset the time
				this->timeElapsedBetweenProjectiles = 0.0f;
			}
		}
	}
	else if (this->attackType == eAttackType::EXPLOSION) {
		//TODO:: explosion effect
		//probably just a sphere getting bigger where the enemy was
	}

}

cEnemy::cEnemy() {
	this->direction = glm::vec3(0.f);
	this->projectileRange = 10.f;
	this->timeElapsedBetweenProjectiles = 0.5f;

	for (int i = 0; i < 10; i++) {
		cGameObject* projectileObject = new cGameObject();
		projectileObject->meshName = "Sphere_xyz_n_uv.ply";
		projectileObject->position = glm::vec3(0.f);
		projectileObject->bIsWireFrame = true;
		projectileObject->bIsSkyBoxObject = false;
		projectileObject->diffuseColour = glm::vec4(1.f);
		projectileObject->scale = 0.25f;
		projectileObject->radius = 0.125f;

		sProjectile pro;
		pro.object = projectileObject;
		pro.speed = 2.f;
		pro.inUse = false;
		this->projectilePool.push_back(pro);
	}
}

cEnemy::~cEnemy() {
	//delete this->theEnemyObject;
}

void cEnemy::removeProjectile(cGameObject* theProjectile) {

	for (std::vector<cGameObject*>::iterator it = this->projectilesToDraw.begin(); it != this->projectilesToDraw.end(); it++) {
		//remove the projectile from the drawing vector
		if (*it == theProjectile) {
			it = this->projectilesToDraw.erase(it);
			break;
		}
	}

	for (std::vector<sProjectile>::iterator it = this->projectiles.begin(); it != this->projectiles.end(); it++) {
		if ((*it).object == theProjectile) {
			it = this->projectiles.erase(it);
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

sProjectile cEnemy::createProjectileBasedOnEnemyStats() {
	sProjectile tempObject;
	int index = this->getNextProjectileIndex();

	//get the projectile
	if (index >= 0)
	{
		tempObject = this->projectilePool[index];
	}
	else {
		return sProjectile();
	}

	return tempObject;
}