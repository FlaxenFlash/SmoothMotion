#include "MaintainBoneLengthChildren.h"

MaintainBoneLengthChildren::MaintainBoneLengthChildren(std::vector<Collider> colliders, std::vector<double*> childPositions, std::vector<double> sqBoneLengths, int skeletonDepth)
{
	_colliders = colliders;
	
	_childrenPositions = childPositions;

	_sqBoneLengths = sqBoneLengths;

	_skeletonDepth = skeletonDepth;
}

bool MaintainBoneLengthChildren::operator()(const double* const x, double* residual) const
{
	auto parameterIndex = 0;

	Vector3d bonePosition(x[0], x[1], x[2]);

	for (int i = 0; i < _colliders.size(); i++)
	{
		auto collider = _colliders[i];
		residual[0] = collider.IntersectsPlane(bonePosition)*1000;
	}

	residual[1] = 0;

	for (auto i = 0; i < _childrenPositions.size(); i++)
	{
		Vector3d child(_childrenPositions[i][0], _childrenPositions[i][1], _childrenPositions[i][2]);
		auto boneLength = (child - bonePosition).norm();
		auto diff = boneLength - _sqBoneLengths[i];
		residual[1] += diff*_skeletonDepth;
	}

	return true;
}