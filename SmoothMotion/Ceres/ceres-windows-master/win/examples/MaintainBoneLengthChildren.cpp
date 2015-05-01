#include "MaintainBoneLengthChildren.h"

MaintainBoneLengthChildren::MaintainBoneLengthChildren(std::vector<Collider> colliders, std::vector<double*> childPositions, std::vector<double> sqBoneLengths)
{
	_colliders = colliders;

	_childrenPositions = std::vector<Vector3d>(childPositions.size()/3);
	auto position = 0;
	for (auto i = 0; i < _childrenPositions.size(); i++)
	{
		_childrenPositions[i] << *childPositions[position++], *childPositions[position++], *childPositions[position++];
	}
	_sqBoneLengths = sqBoneLengths;
}

bool MaintainBoneLengthChildren::operator()(const double* const x, double* residual) const
{
	auto parameterIndex = 0;

	Vector3d bonePosition(x[0], x[1], x[2]);

	for (int i = 0; i < _colliders.size(); i++)
	{
		auto collider = _colliders[i];
		residual[0] = collider.IntersectsPlane(bonePosition)*100;
	}

	residual[1] = 0;

	for (auto i = 0; i < _childrenPositions.size(); i++)
	{
		auto boneLength = (_childrenPositions[i] - bonePosition).norm();
		auto diff = boneLength - _sqBoneLengths[i];
		residual[1] += diff;
	}

	return true;
}