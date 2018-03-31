#ifndef _cGameObject_HG_
#define _cGameObject_HG_
#include "cContactInfo.h"
#include "AABB_Center_Radius.h"
#include "sMeshDrawInfo.h"
#include "sTextureBindBlendInfo.h"
#include "iState.h"
#include <iRigidBody.h>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <string>
#include <vector>

class cPathingNode;

enum eTypeOfObject
{	// Ok people, here's the deal:
	SPHERE = 0,		// - it's a sphere
	PLANE = 1,		// - it's a plane
	CAPSULE = 2,    // - it's a capsule
	AAbb = 3,
	ATAT = 4,// 3- it's an axis-aligned box
	UNKNOWN = 99	// I don't know
};


class cGameObject
{
public:
	cGameObject();		// constructor
	~cGameObject();		// destructor
	void synchronize();

	glm::vec3 position;
	glm::vec3 previousPosition;
	glm::vec3 orientation;
	glm::vec3 orientation2;		// HACK (will elimiate this with)
	glm::vec3 vel;			// Velocity
	glm::vec3 accel;		// Acceleration
	glm::vec4 diffuseColour;	// 

	float scale;
	float radius;

	bool bIsWireFrame;
	bool bIsSkyBoxObject;
	bool hasTexture;
	bool bIsUpdatedInPhysics;
	bool bHasAABB;

	nPhysics::iRigidBody* theBody = NULL;
	eTypeOfObject typeOfObject;		// (really an int)
	cPathingNode* pathingNode;
	iState* theState;
	AABB_Center_Radius* theBoundingBox;
	std::vector<cContactInfo*> contactPoints;
	std::string meshName;		// mesh I'd like to draw
	std::vector<sTextureBindBlendInfo> vecMehs2DTextures;
	std::vector<sTextureBindBlendInfo> vecMeshCubeMaps;
};

#endif
