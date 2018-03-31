#include "cIntegrator.h"
#include "cRigidBody.h"

namespace nPhysics {
	cIntegrator::cIntegrator(IntegrationType type) {
		this->mIntegrationype = type;
	}

	cIntegrator::~cIntegrator() {

	}

	IntegrationType cIntegrator::getType() {
		return this->mIntegrationype;
	}

	void cIntegrator::integrate(std::vector<cRigidBody*>& theBodies, double time, float dt) {
		if (this->mIntegrationype == IntegrationType::RK4)
		{
			glm::vec3 pos;
			glm::vec3 vel;
			glm::vec3 accel(0.0f, -4.8f, 0.0f);
			glm::vec3 rot;
			glm::vec3 angularVelocity;


			for (int i = 0; i < theBodies.size(); i++)
			{
				if (theBodies[i]->GetShape()->GetShapeType() == SHAPE_TYPE_PLANE)
					continue;

				//set the old position
				theBodies[i]->GetPosition(pos);
				theBodies[i]->GetVelocity(vel);
				theBodies[i]->GetRotation(rot);
				theBodies[i]->GetAngularVelocity(angularVelocity);

				//reduce angular velocity by 10%
				angularVelocity *= 0.9f;
				theBodies[i]->SetAngularVelocity(angularVelocity);

				theBodies[i]->SetPreviousPosition(pos);
				integrationStepRK4(pos, vel, accel, rot, angularVelocity,dt);

				//set the old position / velocity
				theBodies[i]->SetPosition(pos);
				theBodies[i]->SetVelocity(vel);
				theBodies[i]->SetRotation(rot);
			}
		}
		else if (this->mIntegrationype == IntegrationType::VERLET)
		{
			integrationStepVerlet(theBodies, dt);
		}
	}

	Derivative& cIntegrator::evaluate(const RK4State& initial,
		float dt,
		const Derivative & d)
	{
		RK4State s;
		s.position = initial.position + d.deltaPosition * dt;
		s.velocity = initial.velocity + d.deltaPosition * dt;

		Derivative output;
		output.deltaPosition = s.velocity;
		output.deltaVelocity = initial.acceleration;
		return output;
	}

	glm::vec3 cIntegrator::acceleration(cRigidBody state, double t)
	{
		glm::vec3 velocity;
		glm::vec3 position;

		state.GetPosition(position);
		state.GetVelocity(velocity);

		const float k = 15.0f;
		const float b = 0.1f;
		return -k * position - b * velocity;
	}

	void cIntegrator::integrationStepRK4(glm::vec3& pos, glm::vec3& vel, glm::vec3& accel,glm::vec3& rot, glm::vec3& angularVelocity, float dt) {
		Derivative a, b, c, d;

		RK4State state(pos, vel, accel);
		a = evaluate(state, 0.0f, Derivative());
		b = evaluate(state, dt*0.5f, a);
		c = evaluate(state, dt*0.5f, b);
		d = evaluate(state, dt, c);

		glm::vec3 dxdt = (a.deltaPosition + ((b.deltaPosition + c.deltaPosition)* 2.0f) + d.deltaPosition) * (1.f / 6.f);
		glm::vec3 dvdt = (a.deltaVelocity + ((b.deltaVelocity + c.deltaVelocity)* 2.0f) + d.deltaVelocity) * (1.f / 6.f);

		rot += (angularVelocity * dt);
		pos += dxdt * dt;
		vel += dvdt * dt;
	}

	void cIntegrator::integrationStepEuler(std::vector<cRigidBody*>& theBodies, float dt) {
		return;
	}

	void cIntegrator::integrationStepVerlet(std::vector<cRigidBody*>& theBodies, float dt) {
		for (int i = 0; i < theBodies.size(); i++)
		{

			if (theBodies[i]->GetShape()->GetShapeType() == SHAPE_TYPE_PLANE)
				continue;

			glm::vec3 Accel = glm::vec3(0, -9.81, 0);
			glm::vec3 OldVel;
			glm::vec3 currVelocity;
			glm::vec3 position;
			glm::vec3 rot;
			glm::vec3 angularVelocity;

			theBodies[i]->GetVelocity(OldVel);
			theBodies[i]->GetPosition(position);
			theBodies[i]->GetVelocity(currVelocity);
			theBodies[i]->GetAngularVelocity(angularVelocity);
			theBodies[i]->GetRotation(rot);

			//reduce angular velocity by 1%
			angularVelocity *= 0.9f;
			theBodies[i]->SetAngularVelocity(angularVelocity);

			//set previous position
			theBodies[i]->SetPreviousPosition(position);

			//calculate new velocity and position
			currVelocity = currVelocity + Accel * dt;
			position = position + (OldVel + currVelocity) * 0.5f * dt;

			//set the new velocity and position
			rot += (angularVelocity * dt);
			theBodies[i]->SetVelocity(currVelocity);
			theBodies[i]->SetPosition(position);
		}
	}
}

