#include "cFollowState.h"

#include "cGameObject.h"
#include <glm\glm.hpp>
#include "cMathHelper.h"
#include "cPlayer.h"

extern cPlayer* g_pThePlayer;

cFollowState::cFollowState() {
	this->mAction = ActionType::IDLE;
	this->mState = StateType::FOLLOWER;
	this->visionDistance = 7.0f;
	this->chaseThreshold = 14.0f;
	this->speed = 0.0f;
	float timeInRadius = 0.0f;
}

const glm::vec4 stateGreen(0.0f, 1.f, 0.f,1.0f);
const glm::vec4 stateTeal(0.0f, 1.0f, 1.0f,1.0f);
const glm::vec4 stateBlack(0.0f, 0.0f, 0.0f,1.0f);

void cFollowState::performAction(cGameObject* player, cGameObject* me, float deltaTime) {
	float enemySpeed = 2.0f;
	//enemy direction
	Matrix RotationMatrix = IDENTITY_MATRIX;
	RotateAboutY(&RotationMatrix, me->orientation2.y);
	glm::vec3 forwardVector = glm::vec3(1.0f, 0.0f, 0.0f);
	forwardVector = VectorMultiplyMatrices(&RotationMatrix, &forwardVector);

	//player direction
	RotationMatrix = IDENTITY_MATRIX;
	RotateAboutY(&RotationMatrix, player->orientation2.y);
	glm::vec3 playerforwardVector = glm::vec3(1.0f, 0.0f, 0.0f);
	playerforwardVector = VectorMultiplyMatrices(&RotationMatrix, &playerforwardVector);

	float playerLookingAtMe = glm::dot(forwardVector, playerforwardVector);

	//determine what way the player is facing
	if (playerLookingAtMe < 0.0f) {
		this->mAction = ActionType::EVADE;
		this->isBehindPlayer = false;
	}
	else if (playerLookingAtMe > 0.0f) {
		this->mAction = ActionType::FOLLOW;
		this->isBehindPlayer = true;
	}

	float distanceFromPlayer = abs(glm::distance(me->position, player->position));

	//determine what action to perform 
	if (distanceFromPlayer > chaseThreshold) {
		this->mAction = ActionType::IDLE;
	}

	if (this->mAction == ActionType::EVADE) {
		glm::vec3 difVector = glm::vec3((player->position.x - me->position.x),
			(player->position.y - me->position.y),
			(player->position.z - me->position.z)
		);
		//reverse the forward vector
		forwardVector = -forwardVector;

		float forwardMag = GetMag(forwardVector);
		float difMag = GetMag(difVector);

		glm::vec3 unitForVec = GetUnitVector(forwardVector, forwardMag);
		glm::vec3 unitDifVec = GetUnitVector(difVector, difMag);

		glm::vec3 axisRotation = glm::cross(unitForVec, unitDifVec);

		float rotAngle = 0.f;
		if (axisRotation.y < 0)
		{
			rotAngle = 1.f;
		}
		else if (axisRotation.y > 0)
		{
			rotAngle = -1.f;
		}
		float rotationSpeed = 0.002f;

		//check to see how far the rotation went 
		if (me->orientation2.y - me->orientation2.y + (rotationSpeed * rotAngle) < 0.01f || me->orientation2.y - me->orientation2.y + (rotationSpeed * rotAngle) > -0.1f)
		{
			me->diffuseColour = stateTeal;
			//move awat from the character 
			glm::vec3 direction = player->position - me->position;
			direction = glm::normalize(direction);
			direction.y = 0.0f;

			//make the enemy move towards the player at a set speed 
			me->position += -direction * this->speed * deltaTime;
		}

		me->orientation2.y += (rotationSpeed * rotAngle);
	}
	else if (this->mAction == ActionType::FOLLOW) {

		glm::vec3 difVector = glm::vec3((player->position.x - me->position.x),
			(player->position.y - me->position.y),
			(player->position.z - me->position.z)
		);

		float forwardMag = GetMag(forwardVector);
		float difMag = GetMag(difVector);

		glm::vec3 unitForVec = GetUnitVector(forwardVector, forwardMag);
		glm::vec3 unitDifVec = GetUnitVector(difVector, difMag);

		glm::vec3 axisRotation = glm::cross(unitForVec, unitDifVec);

		float rotAngle = 0.f;
		if (axisRotation.y > 0)
		{
			rotAngle = 1.f;
		}
		else if (axisRotation.y < 0)
		{
			rotAngle = -1.f;
		}
		float rotationSpeed = 0.01f;

		//and to get the new forward use the rotation vector dot the 
		//check to see how far the rotation went 
		if (me->orientation2.y - me->orientation2.y + (rotationSpeed * rotAngle) < 0.01f || me->orientation2.y - me->orientation2.y + (rotationSpeed * rotAngle) > -0.1f)
		{
			me->diffuseColour = stateGreen;
			//move towards the character 
			glm::vec3 direction = player->position - me->position;
			direction = glm::normalize(direction);
			direction.y = 0.0f;

			//make the enemy move towards the player at a set speed 

			me->position += direction * this->speed * deltaTime;
			
		}

		//set the players orientation
		me->orientation2.y += (rotationSpeed * rotAngle);

		//make sure its the player that were close to 
		if (g_pThePlayer->thePlayerObject == player) {
			//make sure its within the radius
			if (glm::distance(me->position, player->position) < 2.0f) {
				this->timeInRadius += deltaTime;
				//make sure the enemy is within the radius for a second before doing any damage
				if (timeInRadius >= 1.0f) {
					if (g_pThePlayer->currentHealth > 0) {
						g_pThePlayer->currentHealth -= 1.f;
						//reset the counter
						this->timeInRadius = 0.f;
					}
				}
			}
			else {
				//if the player moves outside of the radius reset the timer
				this->timeInRadius = 0.0f;
			}
		}
	}
	else if (this->mAction == ActionType::IDLE) {
		//do nothing
		//get the distance between the player and the object
		me->diffuseColour = stateBlack;
		float dist = abs(glm::distance( me->position, player->position));
		if (dist < this->visionDistance) {
			//change my action type
			if (this->isBehindPlayer) {
				this->mAction = ActionType::FOLLOW;
			}
			else if(this->isBehindPlayer == false){
				this->mAction = ActionType::EVADE;
			}

		}
	}
}

StateType cFollowState::getStateType() {
	return this->mState;
}