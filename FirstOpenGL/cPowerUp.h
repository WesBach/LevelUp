#ifndef _cPowerUp_HG_
#define _cPowerUp_HG_


enum ePickupType {
	PICKUP_HEALTH,
	PICKUP_SPEEDUP,
	PICKUP_RANGE_INCREASE,
	PICKUP_DAMAGE_INCREASE
};

enum eModifierType {
	MODIFIER_HEALTH = 30,
	MODIFIER_SPEEDUP = 4,
	MODIFIER_RANGE_INCREASE = 2,
	MODIFIER_DAMAGE_INCREASE = 10
};

class cGameObject;

class cPowerUp {
public:
	cPowerUp(eModifierType type, ePickupType pickupType,cGameObject* theObject);
	ePickupType getPowerUpType();
	eModifierType getModifierType();
	void setModifierType(eModifierType type);
	//will be different for each type
	float modifierValue;
	cGameObject* thePowerUp;
private:
	ePickupType mPowerUpType;
	eModifierType mModifierType;
};

#endif // !_cPickup_HG
