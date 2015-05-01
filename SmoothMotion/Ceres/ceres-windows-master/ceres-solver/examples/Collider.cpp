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
	Vector3d intersectionVector = PlanePosition - point;
	auto intersection = intersectionVector.dot(PlaneNormal);
	return intersection < 0 ? 0 : intersection;
	/*auto temp = point.x();
	auto intersection = point.x() - PlanePosition.x();
	return intersection < 0 ? 0 : intersection;*/
}

Collider::~Collider()
{
}
