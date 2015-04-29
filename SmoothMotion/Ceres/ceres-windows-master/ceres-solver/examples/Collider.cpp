#include "Collider.h"

Collider::Collider()
{
	PlanePosition = Vector3d();
	PlaneNormal = Vector3d();
}

Collider::Collider(Vector3d position, Vector3d normal)
{
	PlanePosition = position;
	PlaneNormal = normal;
}

double Collider::IntersectsPlane(Vector3d point)
{
	Vector3d intersectionVector = point - PlanePosition;
	return intersectionVector.dot(PlaneNormal);
}

Collider::~Collider()
{
}
