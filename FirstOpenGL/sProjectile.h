#ifndef _sProjectile_HG_
#define _sProjectile_HG_
#include <glm\vec3.hpp>

class cGameObject;

struct sProjectile {
	glm::vec3 direction;
	float speed = 0.0f;
	cGameObject* object = NULL;
	int damage = -1;
	bool inUse = false;
};

#endif // 

