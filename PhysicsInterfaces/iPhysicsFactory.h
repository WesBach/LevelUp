#pragma once
#include "iPhysicsWorld.h"
#include "iRigidBody.h"
#include "sRigidBodyDesc.h"

namespace nPhysics
{
	class iPhysicsFactory
	{
	public:
		virtual ~iPhysicsFactory() {}

		virtual iPhysicsWorld* CreateWorld() = 0;

		virtual iRigidBody* CreateRigidBody(const sRigidBodyDesc& desc, iShape* shape) = 0;
		
		virtual iShape* CreateSphere(float radius) = 0;
		virtual iShape* CreatePlane(const glm::vec3& normal, float planeConst) = 0;
	};
}