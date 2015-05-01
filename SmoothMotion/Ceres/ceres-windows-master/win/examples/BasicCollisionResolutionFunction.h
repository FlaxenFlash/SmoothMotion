#pragma once
#include "../../ceres-solver/examples/Collider.h"
#include <vector>

class BasicCollisionResolutionFunction
{
private:
	std::vector<Collider> _colliders;

public:
	BasicCollisionResolutionFunction(std::vector<Collider> colliders);
	bool operator()(const double* const x, double* residual) const;
};
