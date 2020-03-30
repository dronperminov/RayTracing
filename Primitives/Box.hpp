#pragma once

#include <iostream>
#include "Primitive.hpp"

class Box : public Primitive {
    Vec center;
    Vec min;
    Vec max;
public:
    Box(std::istream& is, Material material); // конструктор из потока
    Primitive* Intersect(const Ray &ray, double tmin, double tmax, double &t); // пересечение с лучем
    Vec GetNormal(const Vec& point); // получение нормали
};

// конструктор из потока
Box::Box(std::istream& is, Material material) {
    Vec size;
    is >> center >> size;
    this->material = material;

    min = center - size / 2;
    max = center + size / 2;
}

// пересечение с лучем
Primitive* Box::Intersect(const Ray &ray, double tmin, double tmax, double &t) {
    double dx = ray.invDirection.x;
    double dy = ray.invDirection.y;
    double dz = ray.invDirection.z;

    double t_min = (min.x - ray.origin.x) * dx;
    double t_max = (max.x - ray.origin.x) * dx;

    if (t_min > t_max)
        std::swap(t_min, t_max);

    double tymin = (min.y - ray.origin.y) * dy;
    double tymax = (max.y - ray.origin.y) * dy;

    if (tymin > tymax)
        std::swap(tymin, tymax);

    if ((t_min > tymax) || (tymin > t_max))
        return nullptr;

    if (tymin > t_min)
        t_min = tymin;

    if (tymax < t_max)
        t_max = tymax;

    double tzmin = (min.z - ray.origin.z) * dz;
    double tzmax = (max.z - ray.origin.z) * dz;

    if (tzmin > tzmax)
        std::swap(tzmin, tzmax);

    if ((t_min > tzmax) || (tzmin > t_max))
        return nullptr;

    if (tzmin > t_min)
        t_min = tzmin;

    if (tzmax < t_max)
        t_max = tzmax;

    t = t_min;

    if (t_min < tmin)
        t = t_max;

    if (t < tmin || t >= tmax)
        return nullptr;

    return this;
}

// получение нормали
Vec Box::GetNormal(const Vec& point) {
    return (point - center).Normalized();
}