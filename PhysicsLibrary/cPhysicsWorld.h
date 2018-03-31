#pragma once
#include "iPhysicsWorld.h"
#include <vector>
#include "cRigidBody.h"
#include "cIntegrator.h"

namespace nPhysics
{
	class cPhysicsWorld : public iPhysicsWorld
	{
	public:
		virtual ~cPhysicsWorld();

		virtual void TimeStep(double time, float deltaTime);
		virtual void AddRigidBody(iRigidBody* rigidBody);
		virtual void RemoveRigidBody(iRigidBody* rigidBody);
		void Collide(iRigidBody* rigidBodyA, iRigidBody* rigidBodyB);
		virtual void setIntegrationType(IntegrationType theType);

	private:	
		std::vector<cRigidBody*> mRigidBody;
		cIntegrator* mIntegrator;

		float mDeltaTime;
		double mTime;
	};
}