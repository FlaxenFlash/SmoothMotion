#pragma once

#include "MaintainBoneLengthFunction.h"
#include "../../ceres-solver/examples/Collider.h"
#include <vector>

class MaintainBoneLengthFunction
{
private:
	std::vector<Collider> _colliders;
	double* _parentPosition;
	double _sqBoneLength;

public:
	MaintainBoneLengthFunction(std::vector<Collider> colliders, double* parentPosition, double sqBoneLength);
	bool operator()(const double* const x, double* residual) const;
};