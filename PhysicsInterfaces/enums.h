#pragma once

namespace nPhysics {
	enum IntegrationType {
		RK4,
		VERLET
	};

	enum eConstraintType {
		CONSTRAINT_TYPE_BALL_AND_SOCKET,
		CONSTRAINT_TYPE_HINGE,
		CONSTRAINT_TYPE_6DOF,
		CONSTRAINT_TYPE_SLIDER,
		CONSTRAINT_TYPE_CONE_TWIST
	};
}
