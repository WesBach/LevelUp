#ifndef _iState_HG_
#define _iState_HG_
#include <glm\vec3.hpp>
#include <glm\vec4.hpp>

enum StateType {
	ANGRY,
	CURIOUS,
	FOLLOWER
};

enum ActionType {
	PURSUE,
	EVADE,
	ARRIVE,
	FOLLOW,
	IDLE
};

class cGameObject;

class iState {
	public:
	virtual ~iState() {}
	virtual void performAction(cGameObject* player, cGameObject* me, float deltaTime) = 0;
	virtual StateType getStateType() = 0;
	virtual void performEnemyAction(cGameObject* firstObject, cGameObject* secondObject, glm::vec3 enemyForward, float deltaTime) = 0;
};

#endif