#include "cCuriousState.h"
#include "cGameObject.h"
#include <glm\glm.hpp>
#include "cMathHelper.h"
#include "cPlayer.h"

extern cPlayer* g_pThePlayer;

cCuriousState::cCuriousState() {
	this->mAction = ActionType::IDLE;	
	this->mState = StateType::CURIOUS;
	this->visionDistance = 7.0f;
	this->chaseThreshold = 14.0f;
	this->speed = 0.0f;
	this->timeInRadius = 0.0f;
}

const glm::vec4 stateGrey	(	0.5f, 0.5f, 0.5f, 1.0f);
const glm::vec4 stateBlue	(	0.0f, 0.0f, 1.0f, 1.0f);
const glm::vec4 statePurple	(	1.0f, 0.0f, 1.0f, 1.0f);

void cCuriousState::performAction(cGameObject* player, cGameObject* me,float deltaTime) {
	float enemySpeed = 2.0f;

	glm::mat4 rotationMatrix = IDENTITYMATRIX;
	Matrix RotationMatrix = IDENTITY_MATRIX;

	
	RotateAboutY(&RotationMatrix, me->orientation2.y);
	glm::vec3 forwardVector = glm::vec3(1.0f, 0.0f, 0.0f);
	forwardVector = VectorMultiplyMatrices(&RotationMatrix, &forwardVector);

	RotationMatrix = IDENTITY_MATRIX;
	RotateAboutY(&RotationMatrix, player->orientation2.y);
	glm::vec3 playerforwardVector = glm::vec3(1.0f, 0.0f, 0.0f);
	playerforwardVector = VectorMultiplyMatrices(&RotationMatrix, &playerforwardVector);

	//set the players forward vector 
	g_pThePlayer->forwardVector = playerforwardVector;

	float playerLookingAtMe = glm::dot(forwardVector, playerforwardVector);

	//set evade or arrive based on where the player is looking
	if (playerLookingAtMe < 0.0f) {
		this->mAction = ActionType::EVADE;
		this->isBehindPlayer = false;
	}
	else if(playerLookingAtMe > 0.0f){
		this->mAction = ActionType::ARRIVE;
		this->isBehindPlayer = true;
	}

	//if the player is outside of the vision then idle
	if (abs(glm::distance(me->position, player->position)) >= chaseThreshold){
		this->mAction = ActionType::IDLE;
	}

	
	if (this->mAction == ActionType::ARRIVE) {
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

		//check to see how far the rotation went 
		if (me->orientation2.y - me->orientation2.y + (rotationSpeed * rotAngle) < 0.01f || me->orientation2.y - me->orientation2.y + (rotationSpeed * rotAngle) > -0.1f)
		{
			//move towards the character 
			me->diffuseColour = stateBlue;
			glm::vec3 direction = player->position - me->position;
			direction = glm::normalize(direction);
			direction.y = 0.0f;

			//make the enemy move towards the player at a set speed 
			me->position += direction * this->speed * deltaTime;
		}

		me->orientation2.y += (rotationSpeed * rotAngle);

		//TODO:: make an enemy class for the speed and timeinradius 
		//do the damage
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
	else if (this->mAction == ActionType::EVADE) {
		//turn away from the person and run
		//TODO:: figure out how to make the enemy run away 
		glm::vec3 difVector = glm::vec3((player->position.x - me->position.x),
			(player->position.y - me->position.y),
			(player->position.z - me->position.z)
		);

		forwardVector = -forwardVector;

		float forwardMag = GetMag(forwardVector);
		float difMag = GetMag(difVector);

		glm::vec3 unitForVec = GetUnitVector(forwardVector, forwardMag);
		glm::vec3 unitDifVec = GetUnitVector(difVector, difMag);
		//determing the rotation
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
			me->diffuseColour = statePurple;
			//move awat from the character 
			glm::vec3 direction = player->position - me->position;
			direction = glm::normalize(direction);
			direction.y = 0.0f;

			//make the enemy move towards the player at a set speed 
			me->position += -direction * this->speed * deltaTime;
		}

		me->orientation2.y += (rotationSpeed * rotAngle);
	}

	else if (this->mAction == ActionType::IDLE) {
		//do nothing
		//get the distance between the player and the object 
		me->diffuseColour = stateGrey;
		float dist = glm::distance(player->position, me->position);
		if (dist < this->visionDistance) {
			//change my action type
			if (this->isBehindPlayer) {
				this->mAction = ActionType::ARRIVE;
			}
			else {
				this->mAction = ActionType::EVADE;
			}
		}
	}
}

StateType cCuriousState::getStateType() {
	return this->mState;
}