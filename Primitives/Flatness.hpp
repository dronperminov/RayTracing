#ifndef FLATNESS_HPP
#define FLATNESS_HPP

#include <iostream>
#include "Primitive.hpp"

// конечная плоскость
class Flatness : public Primitive {
protected:
	Vec normal; // нормаль
	Vec center; // центр
	Vec size; // размер плоскости
public:
	Flatness(std::istream &is, Material material); // конструктор из потока

	double Intersect(const Ray &ray); // пересечение с лучём
	Vec GetNormal(const Vec &point); // получение нормали
};

// конструктор из потока
Flatness::Flatness(std::istream &is, Material material) {
	double sz;
	this->material = material;
	is >> normal >> center >> sz;

	normal = normal.Normalized();

	double dx = sqrt(1 - normal.GetX() * normal.GetX());
	double dy = sqrt(1 - normal.GetY() * normal.GetY());
	double dz = sqrt(1 - normal.GetZ() * normal.GetZ());

	size = Vec(dx, dy, dz) * sz;
}

// пересечение с лучём
double Flatness::Intersect(const Ray &ray) {
	double denom = ray.GetDirection().Dot(normal);

	if (fabs(denom) < EPSILON)
		return INF;

	double t = (center - ray.GetOrigin()).Dot(normal) / denom; // пересечение с плоскостью
	Vec point = ray.GetPoint(t); // находим точку на плоскости

	Vec delta = point - center;

	if (fabs(delta.GetX()) > size.GetX() + EPSILON || fabs(delta.GetY()) > size.GetY() + EPSILON || fabs(delta.GetZ()) > size.GetZ() + EPSILON)
		return INF;

	return t;
}

// получение нормали
Vec Flatness::GetNormal(const Vec &point) {
	return normal;
}

#endif