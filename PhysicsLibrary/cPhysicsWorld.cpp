#include "cPhysicsWorld.h"
#include "collision.h"
#include "collisionResponse.h"
#include "shapes.h"
#include <algorithm>

namespace nPhysics
{

	cPhysicsWorld::~cPhysicsWorld()
	{
		delete this->mIntegrator;
	}

	void cPhysicsWorld::TimeStep(double time, float deltaTime)
	{
		//set the delta time each time the TimeStep is called
		this->mDeltaTime = deltaTime;

		std::vector<cRigidBody*>::iterator itRigidBody = mRigidBody.begin();
		while (itRigidBody != mRigidBody.end())
		{
			cRigidBody* rb = *itRigidBody;
			rb->mPosition += rb->mVelocity * deltaTime;

			itRigidBody++;
		}

		//do the integration
		this->mIntegrator->integrate(this->mRigidBody, this->mTime, this->mDeltaTime);

		//check for collisions 
		for (int i = 0; i < this->mRigidBody.size() - 1; i++)
		{
			for (unsigned int index = i + 1; index < this->mRigidBody.size(); index++)
			{
				this->Collide(mRigidBody[i], mRigidBody[index]);
			}
		}

	}

	void cPhysicsWorld::AddRigidBody(iRigidBody* rigidBody)
	{
		cRigidBody* rb = dynamic_cast<cRigidBody*>(rigidBody);
		if (!rb)
		{
			return;
		}

		std::vector<cRigidBody*>::iterator itRigidBody;
		itRigidBody = std::find(mRigidBody.begin(), mRigidBody.end(), rb);
		if (itRigidBody == mRigidBody.end())
		{
			mRigidBody.push_back(rb);
		}
	}

	void cPhysicsWorld::RemoveRigidBody(iRigidBody* rigidBody)
	{
		cRigidBody* rb = dynamic_cast<cRigidBody*>(rigidBody);
		if (!rb)
		{
			return;
		}
		std::vector<cRigidBody*>::iterator itRigidBody;

		itRigidBody = std::remove(mRigidBody.begin(), mRigidBody.end(), rb);
		if (itRigidBody != mRigidBody.end())
		{
			mRigidBody.resize(mRigidBody.size() - 1);
		}
	}

	void cPhysicsWorld::setIntegrationType(IntegrationType theType) {
		this->mIntegrator = new cIntegrator(theType);
	}

	void cPhysicsWorld::Collide(iRigidBody* rigidBodyA, iRigidBody* rigidBodyB) {
		eShapeType shapeA = rigidBodyA->GetShape()->GetShapeType();
		eShapeType shapeB = rigidBodyB->GetShape()->GetShapeType();


		if (shapeA == SHAPE_TYPE_SPHERE)
		{
			if (shapeB == SHAPE_TYPE_SPHERE)
			{
				//do sphere sphere collision
				glm::vec3 centerA;
				glm::vec3 centerB;
				float radiusA;
				float radiusB;
				glm::vec3 velA;
				glm::vec3 velB;
				rigidBodyA->GetPosition(centerA);
				rigidBodyB->GetPosition(centerB);

				rigidBodyA->GetShape()->GetSphereRadius(radiusA);
				rigidBodyB->GetShape()->GetSphereRadius(radiusB);

				rigidBodyA->GetVelocity(velA);
				rigidBodyB->GetVelocity(velB);

				//intersect_moving_sphere_plane(const vec3& c, float r, const vec3& v, const vec3& n, float d, float& t, vec3& q)
				int collided = glm::intersect_moving_sphere_sphere(centerA, radiusA, velA, centerB, radiusB, velB, this->mDeltaTime);

				if (collided > 0)
				{
					sphereSphereCollisionResponse(rigidBodyA, rigidBodyB, this->mDeltaTime);
				}
				if (collided == -1)
				{
					glm::vec3 prevPos;

					rigidBodyA->GetPreviousPosition(prevPos);
					rigidBodyA->SetPosition(prevPos);

					glm::vec3 prevPosB;

					rigidBodyB->GetPreviousPosition(prevPosB);
					rigidBodyB->SetPosition(prevPosB);
				}
			}
			else if (shapeB == SHAPE_TYPE_PLANE)
			{
				//do sphere plane collision
				glm::vec3 centerA;
				float radiusA;
				glm::vec3 velA;

				rigidBodyA->GetPosition(centerA);
				rigidBodyA->GetVelocity(velA);
				rigidBodyA->GetShape()->GetSphereRadius(radiusA);

				glm::vec3 planeNormal;
				rigidBodyB->GetShape()->GetPlaneNormal(planeNormal);
				float planeDot;
				rigidBodyB->GetShape()->GetPlaneConst(planeDot);

				//	inline vec3 closest_point_on_plane(const vec3& point, const vec3& n, float d)
				glm::vec3 closetsPoint = glm::closest_point_on_plane(centerA, planeNormal, planeDot);

				//inline int intersect_moving_sphere_plane(const vec3& c, float r, const vec3& v, const vec3& n, float d, float& t, vec3& q)
				glm::vec3 collisionPoint;
				float collision;
				int collided = glm::intersect_moving_sphere_plane(centerA, radiusA, velA, planeNormal, planeDot, collision, collisionPoint);

				glm::vec3 finalVelocity;

				if (collided > 0)
				{
					//do the reflection
					glm::vec3 prevPos;
					glm::vec3 normal;
					glm::vec3 currPos;
					glm::vec3 rotation;

					rigidBodyA->GetPosition(currPos);
					rigidBodyA->GetPreviousPosition(prevPos);
					rigidBodyA->SetPosition(prevPos);
					rigidBodyA->GetRotation(rotation);


					//	void cRigidBody::ApplyImpulseAtPoint(const glm::vec3& impulse, const glm::vec3& relativePoint) {

					//rotational inertia
					//2/5(m)(r)(r)

					//rotate the sphere in the opposite direction of the impact
					rotation += -glm::reflect(velA, planeNormal);

					finalVelocity = glm::reflect(velA, planeNormal);
					rigidBodyA->ApplyImpulseAtPoint(finalVelocity, collisionPoint);

					finalVelocity *= 0.5f;
					rigidBodyA->SetVelocity(finalVelocity);					
				}
				else if (collided < 0)
				{
					glm::vec3 prevPos;
					glm::vec3 zero(0.0f);
					finalVelocity = glm::reflect(velA, planeNormal);
					finalVelocity *= 0.5f;
					rigidBodyA->SetVelocity(finalVelocity);
					rigidBodyA->GetPreviousPosition(prevPos);
					rigidBodyA->SetPosition(prevPos);
					rigidBodyA->SetAngularVelocity(zero);
				}
			}
		}
	}
}