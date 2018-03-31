#ifndef _cFollowState_HG_
#define _cFollowState_HG_
#include "iState.h"
class cGameObject;

class cFollowState : public iState {
public:
	cFollowState();
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

#endif // !_cState_HG_
