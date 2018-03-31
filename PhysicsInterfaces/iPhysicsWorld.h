#pragma once
#include "iRigidBody.h"
#include "enums.h"

namespace nPhysics
{
	class iPhysicsWorld
	{
	public:
		virtual ~iPhysicsWorld() {}

		virtual void TimeStep(double time,float deltaTime) = 0;
		virtual void AddRigidBody(iRigidBody* rigidBody) = 0;
		virtual void RemoveRigidBody(iRigidBody* rigidBody) = 0;
		virtual void setIntegrationType(IntegrationType theType) = 0;
	};
}