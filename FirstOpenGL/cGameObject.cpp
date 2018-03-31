#include "cGameObject.h"
#include "cPathingNode.h"

cGameObject::cGameObject()
{
	this->scale = 1.0f;	// (not zero)
	this->position = glm::vec3(0.0f);
	this->previousPosition = glm::vec3(0.0f);
	this->orientation = glm::vec3(0.0f);
	this->orientation2 = glm::vec3(0.0f);

	this->vel = glm::vec3(0.0f);
	this->accel = glm::vec3(0.0f);	
	this->pathingNode = new cPathingNode();
	// If you aren't sure what the 4th value should be, 
	//	make it a 1.0f ("alpha" or transparency)
	this->diffuseColour = glm::vec4( 0.0f, 0.0f, 0.0f, 1.0f );

	//Assume everything is simulated 
	this->theBoundingBox = new AABB_Center_Radius();
	this->bIsUpdatedInPhysics = true; //??? 
	this->bHasAABB = false;
	this->radius = 0.0f;	// Is this the best value??? Who knows?

	this->typeOfObject = eTypeOfObject::UNKNOWN;	// Technically non-standard
	this->theState = NULL;
	//this->typeOfObject = UNKNOWN;

	this->bIsWireFrame = false;
	this->bIsSkyBoxObject = false;
	return;
}

cGameObject::~cGameObject()
{
	delete this->theBoundingBox;
	delete this->pathingNode;
	delete this->theState;

	if (this->theBody != NULL) {
		delete this->theBody;
	}
	return;
}

void cGameObject::synchronize() {
	 this->theBody->GetPosition(this->position);
	 this->theBody->GetVelocity(this->vel);
	 this->theBody->GetRotation(this->orientation2);
}
