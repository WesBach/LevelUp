#include "cPlayer.h"
#include "cGameObject.h"

cPlayer::cPlayer() {
	this->thePlayerObject = new cGameObject();
	this->playerSpeed = 0.0f;
	this->rotationSpeed = 0.0f;
	this->forwardVector = glm::vec3(1.0f, 0.0f, 0.0f);
	this->maxHealth = 100.0f;
	this->currentHealth = 100.0f;

	//for shooting
	glm::vec3 forwardVector;

	maxProjectilesAllowed = 10;
	projectileIndex = -1;
}
	

cPlayer::~cPlayer() {
	delete this->thePlayerObject;
}

void cPlayer::movePlayer(glm::vec3 directionToMove) {
	this->thePlayerObject->position += directionToMove * this->playerSpeed * (float)1.f / 60.f;
}

void cPlayer::rotatePlayer(glm::vec3 directionToRotate) {
	this->thePlayerObject->orientation2.y += directionToRotate.y * this->rotationSpeed * (float)1.f / 60.f;
}

int cPlayer::getNextProjectileIndex() {
	int index = -1;
	//check to see what projectile index will be

	for (int i = 0; i < this->projectilePool.size(); i++) {
		if (this->projectilePool[i].inUse == false)
		{
			index = i;
			this->projectilePool[i].inUse = true;
			break;
		}
	}

	return index;
}

void cPlayer::setProjectileInUse(sProjectile& projectile) {
	for (int i = 0; i < this->projectilePool.size(); i++) {
		//if the memory addresses match set the projectile to be in use
		if (this->projectilePool[i].object == projectile.object)
		{
			this->projectilePool[i].inUse = true;
		}
	}
}

void cPlayer::removeProjectile(cGameObject* theProjectile) {
	for (std::vector<cGameObject*>::iterator it = this->projectilesToDraw.begin(); it != this->projectilesToDraw.end();it++) {
		//remove the projectile from the drawing vector
		if (*it == theProjectile) {
			it = this->projectilesToDraw.erase(it);
			break;
		}
	}

	//remove the projectile from the projetiles vector
	for (std::vector<sProjectile>::iterator it = this->projectiles.begin(); it != this->projectiles.end(); it++) {
		//remove the projectile from the drawing vector
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
