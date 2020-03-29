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

    Primitive* Intersect(const Ray &ray, double &t); // пересечение с лучём
    Vec GetNormal(const Vec &point); // получение нормали
};

// конструктор из потока
Disk::Disk(std::istream &is, Material material) {
    this->material = material;
    is >> normal >> center >> radius;
    normal.Normalize();
}

// пересечение с лучём
Primitive* Disk::Intersect(const Ray &ray, double &t) {
    double denom = ray.direction.Dot(normal);

    if (fabs(denom) < EPSILON) {
        t = INF;
        return nullptr;
    }

    t = (center - ray.origin).Dot(normal) / denom;
    Vec point = ray.GetPoint(t);

    if ((point - center).Norm() > radius) {
        t = INF;
        return nullptr;
    }

    return this;
}

// получение нормали
Vec Disk::GetNormal(const Vec &point) {
    return normal;
}