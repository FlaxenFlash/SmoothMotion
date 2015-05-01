#pragma once

#include "../../ceres-solver/examples/Collider.h"
#include <vector>

class MaintainBoneLengthChildren
{
private:
	std::vector<Collider> _colliders;
	std::vector<double*> _childrenPositions;
	std::vector<double> _sqBoneLengths;
	int _skeletonDepth;

public:
	MaintainBoneLengthChildren(std::vector<Collider> colliders, std::vector<double*> parentPosition, std::vector<double> sqBoneLength, int skeletonDepth);
	bool operator()(const double* const x, double* residual) const;
};