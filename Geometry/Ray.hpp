#ifndef RAY_HPP
#define RAY_HPP

#include <iostream>
#include "Vec.hpp"

class Ray {
	Vec origin; // начало луча
	Vec direction; // направление луча
	Vec invDirection;
public:
	Ray(const Vec& origin, const Vec& direction);

	Vec GetPoint(double t) const; // получение точки
	Vec GetOrigin() const; // получение начала луча
	Vec GetDirection() const; // получение направления луча
	Vec GetInvDirection() const; // получение направления луча
};

Ray::Ray(const Vec& origin, const Vec& direction) {
	this->origin = origin;
	this->direction = direction;

	this->invDirection = Vec(1.0 / direction.x, 1.0 / direction.y, 1.0 / direction.z);
}

// получение точки
Vec Ray::GetPoint(double t) const {
	return origin + direction * t;
}

// получение начала луча
Vec Ray::GetOrigin() const {
	return origin;
}

// получение направления луча
Vec Ray::GetDirection() const {
	return direction;
}

// получение направления луча
Vec Ray::GetInvDirection() const {
	return invDirection;
}

// получение наименьшего числа, большего threshold
double MinThreshold(double t1, double t2, double threshold) {
	if (t1 > threshold)
		return t1;

	if (t2 > threshold)
		return t2;

	return INF;
}

#endif