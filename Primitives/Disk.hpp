#pragma once

#include <iostream>
#include "Primitive.hpp"

// диск
class Disk : public Primitive {
protected:
	Vec normal; // нормаль
	Vec center; // центр
	double radius; // радиус

public:
	Disk(std::istream &is, Material material); // конструктор из потока

	double Intersect(const Ray &ray); // пересечение с лучём
	Vec GetNormal(const Vec &point); // получение нормали
};

// конструктор из потока
Disk::Disk(std::istream &is, Material material) {
	this->material = material;
	is >> normal >> center >> radius;
	normal = normal.Normalized();
}

// пересечение с лучём
double Disk::Intersect(const Ray &ray) {
	double denom = ray.GetDirection().Dot(normal);

	if (fabs(denom) < EPSILON)
		return INF;

	double t = (center - ray.GetOrigin()).Dot(normal) / denom;
	Vec point = ray.GetPoint(t);

	if ((point - center).Norm() > radius)
		return INF;

	return t;
}

// получение нормали
Vec Disk::GetNormal(const Vec &point) {
	return normal;
}