#ifndef _cCuriousState_HG_
#define _cCuriousState_HG_
#include "iState.h"
class cGameObject;

class cCuriousState : public iState {
public:
	cCuriousState();
	virtual void performAction(cGameObject* player, cGameObject* me, float deltaTime);
	virtual StateType getStateType();
	float speed;

private:
	float visionDistance;
	float chaseThreshold;
	StateType mState;
	ActionType mAction;
	bool isBehindPlayer;
	float timeInRadius;
};

#endif