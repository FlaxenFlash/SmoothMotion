#include "BasicCollisionResolutionFunction.h"

BasicCollisionResolutionFunction::BasicCollisionResolutionFunction(std::vector<Collider> colliders)
{
	_colliders = colliders;
}

bool BasicCollisionResolutionFunction::operator()(const double* const x, double* residual) const
{
	int parameterIndex = 0;

	Vector3d bonePosition(x[0], x[1], x[2]);

	for (int i = 0; i < _colliders.size(); i++)
	{
		auto collider = _colliders[i];
		*residual = collider.IntersectsPlane(bonePosition);
	}

	return true;
}