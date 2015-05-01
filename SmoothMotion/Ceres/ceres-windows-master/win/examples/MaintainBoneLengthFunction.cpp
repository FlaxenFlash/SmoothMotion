#include "MaintainBoneLengthFunction.h"

MaintainBoneLengthFunction::MaintainBoneLengthFunction(std::vector<Collider> colliders, double* parentPosition, double sqBoneLength)
{
	_colliders = colliders;

	_parentPosition = parentPosition;
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

	Vector3d parentPosition(_parentPosition[0], _parentPosition[1], _parentPosition[2]);
	auto boneLength = (parentPosition - bonePosition).norm();
	auto diff = boneLength - _sqBoneLength;
	residual[1] = diff;

	return true;
}