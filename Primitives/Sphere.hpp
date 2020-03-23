#ifndef SPHERE_HPP
#define SPHERE_HPP

#include <iostream>
#include "Primitive.hpp"

class Sphere : public Primitive {
	Vec center; // центр
	double radius; // радиус

public:
	Sphere(Vec center, double radius, Material material);
	Sphere(std::istream& is, Material material); // создание из потока
	
	Primitive* Intersect(const Ray &ray, double &t); // пересечение с лучём
	Vec GetNormal(const Vec &point); // получение нормали в точке
	void UpdateBbox(Vec& min, Vec &max); // обновление ограничивающего объёма
};

Sphere::Sphere(Vec center, double radius, Material material) {
	this->center = center;
	this->radius = radius;
	this->material = material;
}

// создание из потока
Sphere::Sphere(std::istream& is, Material material) {
	this->material = material;
	is >> center >> radius;
}

// пересечение сферы с лучём
Primitive* Sphere::Intersect(const Ray &ray, double &t) {
	Vec oc = ray.GetOrigin() - center;

	double k = -oc.Dot(ray.GetDirection());
	double d = k * k - oc.Dot(oc) + radius * radius;

	if (d < EPSILON) {
        t = INF;
        return nullptr;
    }

	d = sqrt(d);

	t = MinThreshold(k - d, k + d, EPSILON);
	return this;
}

// получение нормали в точке
Vec Sphere::GetNormal(const Vec &point) {
	return (point - center) * (1.0 / radius);
}

// обновление ограничивающего объёма
void Sphere::UpdateBbox(Vec& min, Vec &max) {
	min = min.Min(center - Vec(radius, radius, radius));
	max = max.Max(center + Vec(radius, radius, radius));
}

#endif