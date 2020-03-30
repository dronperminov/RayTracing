#pragma once

#include <iostream>
#include "Primitive.hpp"

class ChessFlatness : public Primitive {
    Vec normal; // нормаль
    Vec center; // центр
    Vec size; // размер плоскости
    Vec cellSize;
    Material material2;
public:
    ChessFlatness(std::istream &is, Material material, Material material2); // конструктор из потока

    Primitive* Intersect(const Ray &ray, double tmin, double tmax, double &t); // пересечение с лучём
    Vec GetNormal(const Vec &point); // получение нормали
    Material GetMaterial(const Vec& point); // получение цвета в точке
};

// конструктор из потока
ChessFlatness::ChessFlatness(std::istream &is, Material material, Material material2) {
    double sz;
    int cells;
    this->material = material;
    this->material2 = material2;
    is >> normal >> center >> sz >> cells;

    normal.Normalize();

    double dx = sqrt(1 - normal.x * normal.x);
    double dy = sqrt(1 - normal.y * normal.y);
    double dz = sqrt(1 - normal.z * normal.z);

    size = Vec(dx, dy, dz) * sz;
    cellSize = size * (2.0 / cells);
}

// пересечение с лучём
Primitive* ChessFlatness::Intersect(const Ray &ray, double tmin, double tmax, double &t) {
    double denom = ray.direction.Dot(normal);

    if (fabs(denom) < EPSILON){
        t = INF;
        return nullptr;
    }

    t = (center - ray.origin).Dot(normal) / denom; // пересечение с плоскостью

    if (t < tmin || t >= tmax)
        return nullptr;

    Vec point = ray.GetPoint(t); // находим точку на плоскости

    Vec delta = point - center;

    if (fabs(delta.x) > size.x + EPSILON || fabs(delta.y) > size.y + EPSILON || fabs(delta.z) > size.z + EPSILON)
        return nullptr;

    return this;
}

// получение нормали
Vec ChessFlatness::GetNormal(const Vec &point) {
    return normal;
}

// получение цвета в точке
Material ChessFlatness::GetMaterial(const Vec& point) {
    Vec delta = point - center;

    double ax = asin(normal.x);
    double ay = asin(normal.y);
    double az = asin(normal.z);

    delta = delta.RotateZ(az).RotateX(ax);

    double x = delta.x;
    double y = delta.y;
    double z = delta.z;

    int dx = (fmod(fabs(x), cellSize.x) < cellSize.x / 2) ^ (x < 0);
    int dz = (fmod(fabs(z), cellSize.z) < cellSize.z / 2) ^ (z < 0);

    if (dx ^ dz)
        return material;

    return material2;
}
