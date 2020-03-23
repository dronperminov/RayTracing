#ifndef CHESS_FLATNESS_HPP
#define CHESS_FLATNESS_HPP

#include <iostream>
#include "Primitive.hpp"

class ChessFlatness : public Primitive {
	Vec normal; // нормаль
	Vec center; // центр
	Vec size; // размер плоскости
	Vec cellSize;
public:
	ChessFlatness(std::istream &is, Material material); // конструктор из потока

	double Intersect(const Ray &ray); // пересечение с лучём
	Vec GetNormal(const Vec &point); // получение нормали
	Vec GetColor(const Vec& point); // получение цвета в точке
};

// конструктор из потока
ChessFlatness::ChessFlatness(std::istream &is, Material material) {
	double sz;
	int cells;
	this->material = material;
	is >> normal >> center >> sz >> cells;

	normal = normal.Normalized();

	double dx = sqrt(1 - normal.GetX() * normal.GetX());
	double dy = sqrt(1 - normal.GetY() * normal.GetY());
	double dz = sqrt(1 - normal.GetZ() * normal.GetZ());

	size = Vec(dx, dy, dz) * sz;
	cellSize = size * (2.0 / cells);
}

// пересечение с лучём
double ChessFlatness::Intersect(const Ray &ray) {
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
Vec ChessFlatness::GetNormal(const Vec &point) {
	return normal;
}

// получение цвета в точке
Vec ChessFlatness::GetColor(const Vec& point) {
	Vec delta = point - center;

	double ax = asin(normal.GetX());
	double ay = asin(normal.GetY());
	double az = asin(normal.GetZ());

	delta = delta.RotateZ(az).RotateX(ax);

	double x = delta.GetX();
	double y = delta.GetY();
	double z = delta.GetZ();

	int dx = (fmod(fabs(x), cellSize.GetX()) < cellSize.GetX() / 2) ^ (x < 0);
	int dz = (fmod(fabs(z), cellSize.GetZ()) < cellSize.GetZ() / 2) ^ (z < 0);

	if (dx ^ dz)
		return material.color;

	return Vec(255, 255, 255) - material.color;
}

#endif