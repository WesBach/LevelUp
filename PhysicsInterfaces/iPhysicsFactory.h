#pragma once
#include "iPhysicsWorld.h"
#include "iRigidBody.h"
#include "sRigidBodyDesc.h"
#include "iConstraint.h"

namespace nPhysics
{
	class iPhysicsFactory
	{
	public:
		virtual ~iPhysicsFactory() {}
		//world creation function
		virtual iPhysicsWorld* CreateWorld() = 0;
		//rigid body creation function
		virtual iRigidBody* CreateRigidBody(const sRigidBodyDesc& desc, iShape* shape) = 0;
		
		//shape creation functions
		virtual iShape* CreateSphere(float radius) = 0;
		virtual iShape* CreatePlane(const glm::vec3& normal, float planeConst) = 0;
		virtual iShape* CreateBox(const glm::vec3& halfExtents) = 0;
		virtual iShape* CreateCylinder(const glm::vec3& halfExtents) = 0;
		virtual iShape* CreateCone(const float& radius, const float& height) = 0;


		//constraint creation functions
		virtual iConstraint* CreateHingeConstraint(iRigidBody* bodyA, const glm::vec3& pivot, const glm::vec3& axis, iRigidBody* bodyB, const glm::vec3& pivotB, const glm::vec3& axisB) = 0;
		virtual iConstraint* CreateHingeConstraint(iRigidBody* bodyA, const glm::vec3& pivot, const glm::vec3& axis) = 0;
		virtual iConstraint* CreateHingeConstraint(iRigidBody* bodyA, const glm::vec3& origin, iRigidBody* bodyB, const glm::vec3& originB) = 0;
		virtual iConstraint* CreateHingeConstraint(iRigidBody* bodyA, const glm::vec3& origin) = 0;

		virtual iConstraint* CreateBallAndSocketConstraint(iRigidBody* bodyA, const glm::vec3& pivotPoint) = 0;
		virtual iConstraint* CreateBallAndSocketConstraint(iRigidBody* bodyA, const glm::vec3& pivotPointA, iRigidBody* bodyB, const glm::vec3& pivotPointB) = 0;

		virtual iConstraint* CreateSliderConstraint(iRigidBody* bodyA, iRigidBody* bodyB, const glm::vec3& originA, const glm::vec3& originB, bool useLinearReferenceFrameA) = 0;
		virtual iConstraint* CreateSliderConstraint(iRigidBody* bodyB, const glm::vec3& originB, bool useLinearReferenceFrameA) = 0;

		virtual iConstraint* Create6DOFConstraint(iRigidBody* bodyA, iRigidBody* bodyB, const glm::vec3& originA, const glm::vec3& originB, bool useLinearReferenceFrameA) = 0;
		virtual iConstraint* Create6DOFConstraint(iRigidBody* bodyB, const glm::vec3& originB, bool useLinearReferenceFrameA) = 0;

		virtual iConstraint* CreateConeTwistConstraint(iRigidBody* bodyA, iRigidBody* bodyB) = 0;
		virtual iConstraint* CreateConeTwistConstraint(iRigidBody* bodyA) = 0;
	};
}