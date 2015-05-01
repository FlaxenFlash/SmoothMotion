#pragma once

#include "../../ceres-solver/examples/Collider.h"
#include <vector>

class MaintainBoneLengthChildren
{
private:
	std::vector<Collider> _colliders;
	std::vector<Vector3d> _childrenPositions;
	std::vector<double> _sqBoneLengths;

public:
	MaintainBoneLengthChildren(std::vector<Collider> colliders, std::vector<double*> parentPosition, std::vector<double> sqBoneLength);
	bool operator()(const double* const x, double* residual) const;
};