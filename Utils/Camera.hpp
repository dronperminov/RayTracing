#pragma once

#include <cmath>
#include "../Geometry/Vec.hpp"

class Camera {
	Vec position;
	double R[3][3];

	void InitMatrix(const Vec &rotation);
public:
	Camera(const Vec &position, const Vec &rotation);

	void SetPosition(const Vec& position); // задание положения камеры
	void SetRotation(const Vec& rotation); // задание поворота камеры

	Ray GetRay(double wx, double wy) const; // получение луча
};

Camera::Camera(const Vec &position, const Vec &rotation) {
	this->position = position;
	InitMatrix(rotation);
}

void Camera::InitMatrix(const Vec &rotation) {
	R[0][0] = cos(rotation.GetY());
	R[0][1] = cos(rotation.GetX()) * sin(rotation.GetY());
	R[0][2] = sin(rotation.GetX()) * sin(rotation.GetY());

	R[1][0] = 0;
	R[1][1] = sin(rotation.GetX());
	R[1][2] = -cos(rotation.GetX());

	R[2][0] = -sin(rotation.GetY());
	R[2][1] = cos(rotation.GetX()) * cos(rotation.GetY());
	R[2][2] = sin(rotation.GetX()) * cos(rotation.GetY());
}

// задание положения камеры
void Camera::SetPosition(const Vec& position) {
	this->position = position;
}

// задание поворота камеры
void Camera::SetRotation(const Vec& rotation) {
	InitMatrix(rotation);
}

// получение луча
Ray Camera::GetRay(double wx, double wy) const {
	Vec direction = Vec(wx, wy, 1).Transform(R);
	return Ray(position, direction.Normalized());
}
