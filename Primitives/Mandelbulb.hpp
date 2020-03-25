#pragma once

#include <iostream>
#include "Primitive.hpp"

const int MAX_RAY_STEPS = 128;

// диск
class Mandelbulb : public Primitive {
	double power;
	double radius;
	Vec center;

public:
	Mandelbulb(std::istream &is, Material material); // конструктор из потока

	Primitive* Intersect(const Ray &ray, double &t); // пересечение с лучём
	Vec GetNormal(const Vec &point); // получение нормали

	double DE(const Vec &pos) {
		Vec z0 = pos - center;
		Vec z = z0;
		double dr = 1.0;
		double r = 0.0;

		for (int i = 0; i < 64; i++) {
			r = z.Norm();
			
			if (r > radius)
				break;
			
			double theta = acos(z.z / r);
			double phi = atan2(z.y, z.x);
			dr = pow(r, power - 1.0) * power * dr + 1.0;
			
			double zr = pow(r, power);
			theta = theta * power;
			phi = phi * power;
			
			z = Vec(sin(theta) * cos(phi), sin(phi) * sin(theta), cos(theta)) * zr + z0;
		}

		return 0.5*log(r)*r/dr;
	}
};

// конструктор из потока
Mandelbulb::Mandelbulb(std::istream &is, Material material) {
	is >> center >> power >> radius;
	this->material = material;
}

// пересечение с лучём
Primitive* Mandelbulb::Intersect(const Ray &ray, double &t) {
	t = 0;
	
	for (int steps = 0; steps < MAX_RAY_STEPS; steps++) {
		double distance = DE(ray.GetPoint(t));
		
		if (distance < EPSILON)
			return this;

		t += distance;
	}

	t = INF;
	return nullptr;
}

// получение нормали
Vec Mandelbulb::GetNormal(const Vec &point) {
	double EPS = 0.01;
	double x = point.x;
	double y = point.y;
	double z = point.z;

	double dx = DE(Vec(x + EPS, y, z)) - DE(Vec(x - EPS, y, z));
	double dy = DE(Vec(x, y + EPS, z)) - DE(Vec(x, y - EPS, z));
	double dz = DE(Vec(x, y, z + EPS)) - DE(Vec(x, y, z - EPS));

	return Vec(dx, dy, dz).Normalized();
}