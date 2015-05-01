#include "MaintainBoneLengthFunction.h"

MaintainBoneLengthFunction::MaintainBoneLengthFunction(std::vector<Collider> colliders, double* parentPosition, double sqBoneLength)
{
	_colliders = colliders;

	_parentPosition << parentPosition[0], parentPosition[1], parentPosition[2];
	_sqBoneLength = sqBoneLength;
}

bool MaintainBoneLengthFunction::operator()(const double* const x, double* residual) const
{
	auto parameterIndex = 0;

	Vector3d bonePosition(x[0], x[1], x[2]);

	for (int i = 0; i < _colliders.size(); i++)
	{
		auto collider = _colliders[i];
		residual[0] = collider.IntersectsPlane(bonePosition);
	}

	auto boneLength = (_parentPosition - bonePosition).norm();
	auto diff = boneLength - _sqBoneLength;
	residual[1] = diff;

	return true;
}