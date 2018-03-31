#ifndef _cAngryState_HG_
#define _cAngryState_HG_
#include "iState.h"
class cGameObject;

class cAngryState : public iState {
public:
	cAngryState();
	virtual void performAction(cGameObject* player, cGameObject* me, float deltaTime);
	virtual StateType getStateType();
	float speed;
private:
	float visionDistance;
	float chaseThreshold;
	StateType mState;
	ActionType mAction;
	bool isBehindPlayer;

	//timer for damage 
	float timeInRadius;

};

#endif // !_cState_HG_
