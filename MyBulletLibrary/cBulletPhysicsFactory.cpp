#include "cBulletPhysicsFactory.h"
#include "cBulletRigidBody.h"
#include "shapes.h"
#include "cBulletPhysicsWorld.h"
#include "cBullet6DOFConstraint.h"
#include "cBulletBallAndSocketConstraint.h"
#include "cBulletHingeConstraint.h"
#include "cBulletSliderConstraint.h"
#include "cBulletConeTwistConstraint.h"
#include "nConvert.h"

namespace nPhysics {

	//destructor
	cBulletPhysicsFactory::~cBulletPhysicsFactory(){
	}

	//constructor
	cBulletPhysicsFactory::cBulletPhysicsFactory() {
	}

	//create world function
	iPhysicsWorld * cBulletPhysicsFactory::CreateWorld()
	{
		return new cBulletPhysicsWorld();
	}

	//create and return rigid body
	iRigidBody* cBulletPhysicsFactory::CreateRigidBody(const sRigidBodyDesc& desc, iShape* shape) {
		return new cBulletRigidBody(desc, shape);
	}

	//create and return sphere shape
	iShape* cBulletPhysicsFactory::CreateSphere(float radius) {
		return new cBulletSphereShape(radius);
	}

	//create and return plane shape
	iShape* cBulletPhysicsFactory::CreatePlane(const glm::vec3& normal, float planeConst) {
		return new cBulletPlaneShape(normal,planeConst);
	}

	//create and return cylinder shape
	iShape* cBulletPhysicsFactory::CreateCylinder(const glm::vec3& halfExtents) {
		return new cBulletCylinderShape(halfExtents);
	}

	//create and return box shape
	iShape* cBulletPhysicsFactory::CreateBox(const glm::vec3& halfExtents) {
		return new cBulletBoxShape(halfExtents);
	}

	iShape* cBulletPhysicsFactory::CreateCone(const float& radius, const float& height) {
		return new cBulletConeShape(radius, height);
	}

	//create and return hinge constraint
	iConstraint*  cBulletPhysicsFactory::CreateHingeConstraint(iRigidBody* bodyA, const glm::vec3& pivot, const glm::vec3& axis, iRigidBody* bodyB, const glm::vec3& pivotB, const glm::vec3& axisB) {
		//cast the rigid bodies
		cBulletRigidBody* pA = dynamic_cast<cBulletRigidBody*>(bodyA);
		cBulletRigidBody* pB = dynamic_cast<cBulletRigidBody*>(bodyB);

		return new cBulletHingeConstraint(pA,pivot, axis , pB, pivotB, axisB);
	}

	//create and return hinge constraint
	iConstraint*  cBulletPhysicsFactory::CreateHingeConstraint(iRigidBody* bodyA, const glm::vec3& pivot, const glm::vec3& axis) {
		//cast the rigid bodies
		cBulletRigidBody* pA = dynamic_cast<cBulletRigidBody*>(bodyA);

		return new cBulletHingeConstraint(pA,pivot,axis);
	}

	//create and return hinge constraint
	iConstraint*  cBulletPhysicsFactory::CreateHingeConstraint(iRigidBody* bodyA, const glm::vec3& origin, iRigidBody* bodyB, const glm::vec3& originB) {
		cBulletRigidBody* pA = dynamic_cast<cBulletRigidBody*>(bodyA);
		cBulletRigidBody* pB = dynamic_cast<cBulletRigidBody*>(bodyB);

		return new cBulletHingeConstraint(pA,origin,pB,originB);
	}

	//create and return hinge constraint
	iConstraint*  cBulletPhysicsFactory::CreateHingeConstraint(iRigidBody* bodyA, const glm::vec3& origin) {
		cBulletRigidBody* pA = dynamic_cast<cBulletRigidBody*>(bodyA);
		
		return new cBulletHingeConstraint(pA,origin);
	}

	//create and return BallAndSocket constraint
	iConstraint*  cBulletPhysicsFactory::CreateBallAndSocketConstraint(iRigidBody* bodyA, const glm::vec3& pivotPoint) {
		btVector3 axisInA(0.0f,1.0f,0.0f);

		cBulletRigidBody* rigidA = dynamic_cast<cBulletRigidBody*>(bodyA);

		rigidA->getBulletRigidBody()->getCenterOfMassTransform().getBasis() * axisInA;

		cBulletBallAndSocketConstriant* point2point;
		point2point = new cBulletBallAndSocketConstriant(rigidA, pivotPoint);

		return point2point;
	}

	//create and return BallAndSocket constraint
	iConstraint*  cBulletPhysicsFactory::CreateBallAndSocketConstraint(iRigidBody* bodyA, const glm::vec3& pivotPointA, iRigidBody* bodyB, const glm::vec3& pivotPointB) {
		btVector3 axisInA(0.0f, 1.0f, 0.0f);
		btVector3 axisInB(0.0f, 0.0f, 1.0f);

		cBulletRigidBody* rigidA = dynamic_cast<cBulletRigidBody*>(bodyA);
		rigidA->getBulletRigidBody()->getCenterOfMassTransform().getBasis() * axisInA;

		cBulletRigidBody* rigidB = dynamic_cast<cBulletRigidBody*>(bodyB);
		rigidB->getBulletRigidBody()->getCenterOfMassTransform().getBasis() * axisInB;

		glm::vec3 temp(1.f);
		cBulletBallAndSocketConstriant* point2point;
		point2point = new cBulletBallAndSocketConstriant(rigidA,rigidB, temp,pivotPointB);

		return point2point;
	}

	//create and return Slider constraint
	iConstraint*  cBulletPhysicsFactory::CreateSliderConstraint(iRigidBody* bodyA, iRigidBody* bodyB, const glm::vec3& originA, const glm::vec3& originB, bool useLinearReferenceFrameA) {
		//cast the rigid bodies
		cBulletRigidBody* pA = dynamic_cast<cBulletRigidBody*>(bodyA);
		cBulletRigidBody* pB = dynamic_cast<cBulletRigidBody*>(bodyB);
		//return the new constraint
		return new cBulletSliderConstraint(pA, pB, originA, originB,useLinearReferenceFrameA);
	}

	//create and return Slider constraint
	iConstraint*  cBulletPhysicsFactory::CreateSliderConstraint(iRigidBody* bodyB, const glm::vec3& originB, bool useLinearReferenceFrameA) {
		cBulletRigidBody* pB = dynamic_cast<cBulletRigidBody*>(bodyB);
		return new cBulletSliderConstraint(pB, originB, useLinearReferenceFrameA);

	}

	//create and return 6DOF constraint
	iConstraint*  cBulletPhysicsFactory::Create6DOFConstraint(iRigidBody* bodyA, iRigidBody* bodyB, const glm::vec3& originA, const glm::vec3& originB, bool useLinearReferenceFrameA) {
		//cast the rigid bodies
		cBulletRigidBody* pA = dynamic_cast<cBulletRigidBody*>(bodyA);
		cBulletRigidBody* pB = dynamic_cast<cBulletRigidBody*>(bodyB);

		return new cBullet6DOFConstraint(pA, pB, originA, originB, useLinearReferenceFrameA);
	}

	//create and return 6DOF constraint
	iConstraint*  cBulletPhysicsFactory::Create6DOFConstraint(iRigidBody* bodyB, const glm::vec3& originB, bool useLinearReferenceFrameA) {
		//cast the rigid bodies
		cBulletRigidBody* pB = dynamic_cast<cBulletRigidBody*>(bodyB);

		return new cBullet6DOFConstraint(pB, originB, useLinearReferenceFrameA);
	}

	//create and return ConeTwist constraint
	iConstraint* cBulletPhysicsFactory::CreateConeTwistConstraint(iRigidBody* bodyA, iRigidBody* bodyB) {
		cBulletRigidBody* pA = dynamic_cast<cBulletRigidBody*>(bodyA);
		cBulletRigidBody* pB = dynamic_cast<cBulletRigidBody*>(bodyB);
		return new cBulletConeTwistConstraint(pA,pB);
	}

	//create and return ConeTwist constraint
	iConstraint* cBulletPhysicsFactory::CreateConeTwistConstraint(iRigidBody* bodyA) {
		cBulletRigidBody* pA = dynamic_cast<cBulletRigidBody*>(bodyA);
		return new cBulletConeTwistConstraint(pA);
	}

}