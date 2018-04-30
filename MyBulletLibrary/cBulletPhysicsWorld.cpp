#include "cBulletPhysicsWorld.h"
#include "nConvert.h"
#include "cBulletRigidBody.h"
#include "cBullet6DOFConstraint.h"
#include "cBulletBallAndSocketConstraint.h"
#include "cBulletHingeConstraint.h"
#include "cBulletSliderConstraint.h"
#include "cBulletConeTwistConstraint.h"

namespace nPhysics
{
	//destructor
	cBulletPhysicsWorld::~cBulletPhysicsWorld() {
		delete this->mDynamicsWorld;
		delete this->mSolver;
		delete this->mOverlappingPairCache;
		delete this->mDispatcher;
		delete this->mCollisionConfiguration;
	}

	//constructor
	cBulletPhysicsWorld::cBulletPhysicsWorld() {
		this->mCollisionConfiguration = new btDefaultCollisionConfiguration();
		this->mDispatcher = new btCollisionDispatcher(mCollisionConfiguration);
		this->mOverlappingPairCache = new btDbvtBroadphase();
		this->mSolver = new btSequentialImpulseConstraintSolver;
		this->mDynamicsWorld = new btDiscreteDynamicsWorld(mDispatcher, mOverlappingPairCache, mSolver, mCollisionConfiguration);
		this->mDynamicsWorld->setGravity(btVector3(0, -9.8, 0));
	}

	//timestep function
	void cBulletPhysicsWorld::TimeStep(double time, float deltaTime) {
		this->mDynamicsWorld->stepSimulation(deltaTime);
	}

	//add rigid body to the physics world
	void cBulletPhysicsWorld::AddRigidBody(iRigidBody* rigidBody) {
		cBulletRigidBody* myBody = dynamic_cast<cBulletRigidBody*>(rigidBody);

		btRigidBody* btBody = myBody->getBulletRigidBody();
		if (btBody->isInWorld() == false)
		{
			this->mDynamicsWorld->addRigidBody(btBody);
		}
	}

	//remove rigid body from the world
	void cBulletPhysicsWorld::RemoveRigidBody(iRigidBody* rigidBody) {
		cBulletRigidBody* myBody = dynamic_cast<cBulletRigidBody*>(rigidBody);

		btRigidBody* btBody = myBody->getBulletRigidBody();
		if (btBody->isInWorld())
		{
			//remove body
			this->mDynamicsWorld->removeRigidBody(btBody);
		}
	}

	//add a constraint to the physics world
	void cBulletPhysicsWorld::addConstraint(iConstraint* theConstraint) {
		eConstraintType type = theConstraint->GetConstraintType();
		cBullet6DOFConstraint* pDOF;
		cBulletHingeConstraint* pHinge;
		cBulletBallAndSocketConstriant* pBallAndSocket;
		cBulletSliderConstraint* pSlider;
		cBulletConeTwistConstraint* pConeTwist;

		if (type == eConstraintType::CONSTRAINT_TYPE_HINGE) {
			pHinge = dynamic_cast<cBulletHingeConstraint*>(theConstraint);
			this->mDynamicsWorld->addConstraint(pHinge->GetBulletConstraint());
		}
		else if (type == eConstraintType::CONSTRAINT_TYPE_BALL_AND_SOCKET) {
			pBallAndSocket = dynamic_cast<cBulletBallAndSocketConstriant*>(theConstraint);
			btTypedConstraint* con = pBallAndSocket->GetBulletConstraint();
			this->mDynamicsWorld->addConstraint(con,true);
		}
		else if (type == eConstraintType::CONSTRAINT_TYPE_SLIDER) {
			pSlider = dynamic_cast<cBulletSliderConstraint*>(theConstraint);
			this->mDynamicsWorld->addConstraint(pSlider->GetBulletConstraint());
		}
		else if (type == eConstraintType::CONSTRAINT_TYPE_CONE_TWIST) {
			pConeTwist = dynamic_cast<cBulletConeTwistConstraint*>(theConstraint);
			//m_ctc->setLimit(btScalar(SIMD_PI_4*0.6f), btScalar(SIMD_PI_4), btScalar(SIMD_PI) * 0.8f, 0.5f);
			pConeTwist->GetBulletConstraint()->setLimit(btScalar(0.5),btScalar(0.5),btScalar(0));
			this->mDynamicsWorld->addConstraint(pConeTwist->GetBulletConstraint());
		}
		else {
			pDOF = dynamic_cast<cBullet6DOFConstraint*>(theConstraint);
			this->mDynamicsWorld->addConstraint(pDOF->GetBulletConstraint());
		}
	}

	//remove constraint from the physics world
	void cBulletPhysicsWorld::removeConstraint(iConstraint* theConstraint) {
		eConstraintType type = theConstraint->GetConstraintType();
		cBullet6DOFConstraint* pDOF;
		cBulletHingeConstraint* pHinge;
		cBulletBallAndSocketConstriant* pBallAndSocket;
		cBulletSliderConstraint* pSlider;
		cBulletConeTwistConstraint* pConeTwist;

		if (type == eConstraintType::CONSTRAINT_TYPE_HINGE) {
			pHinge = dynamic_cast<cBulletHingeConstraint*>(theConstraint);
			this->mDynamicsWorld->removeConstraint(pHinge->GetBulletConstraint());
		}
		else if (type == eConstraintType::CONSTRAINT_TYPE_BALL_AND_SOCKET) {
			pBallAndSocket = dynamic_cast<cBulletBallAndSocketConstriant*>(theConstraint);
			this->mDynamicsWorld->removeConstraint(pBallAndSocket->GetBulletConstraint());
		}
		else if (type == eConstraintType::CONSTRAINT_TYPE_SLIDER) {
			pSlider = dynamic_cast<cBulletSliderConstraint*>(theConstraint);
			this->mDynamicsWorld->removeConstraint(pSlider->GetBulletConstraint());
		}
		else if (type == eConstraintType::CONSTRAINT_TYPE_CONE_TWIST) {
			pConeTwist = dynamic_cast<cBulletConeTwistConstraint*>(theConstraint);
			this->mDynamicsWorld->removeConstraint(pConeTwist->GetBulletConstraint());
		}
		else {
			pDOF = dynamic_cast<cBullet6DOFConstraint*>(theConstraint);
			this->mDynamicsWorld->removeConstraint(pDOF->GetBulletConstraint());
		}
	}

	//integration type
	void cBulletPhysicsWorld::setIntegrationType(IntegrationType theType) {
		
	}
}

