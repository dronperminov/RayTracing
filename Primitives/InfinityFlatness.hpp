#pragma once

#include <iostream>
#include "Primitive.hpp"

// бесконечная плоскость
class InfinityFlatness : public Primitive {
protected:
	Vec normal; // нормаль
	double bias; // смещение

public:
	InfinityFlatness(std::istream &is, Material material); // конструктор из потока

	double Intersect(const Ray &ray); // пересечение с лучём
	Vec GetNormal(const Vec &point); // получение нормали
};

// конструктор из потока
InfinityFlatness::InfinityFlatness(std::istream &is, Material material) {
	this->material = material;
	is >> normal >> bias;
	normal = normal.Normalized();
}

// пересечение с лучём
double InfinityFlatness::Intersect(const Ray &ray) {
	double denom = ray.GetDirection().Dot(normal);

	if (fabs(denom) < EPSILON)
		return INF;

	return -(bias + ray.GetOrigin().Dot(normal)) / denom;
}

// получение нормали
Vec InfinityFlatness::GetNormal(const Vec &point) {
	return normal;
}