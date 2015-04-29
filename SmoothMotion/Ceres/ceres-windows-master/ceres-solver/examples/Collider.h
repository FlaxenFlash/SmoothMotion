#pragma once
#include "Eigen\Dense"

using Eigen::Vector3d;

class Collider
{
public:
	Vector3d PlanePosition;
	Vector3d PlaneNormal;

	Collider();
	Collider(Vector3d position, Vector3d normal);
	double IntersectsPlane(Vector3d point);
	~Collider();
};

