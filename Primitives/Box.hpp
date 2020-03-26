#pragma once

#include <iostream>
#include "Primitive.hpp"

class Box : public Primitive {
    Vec center;
    Vec min;
    Vec max;
public:
    Box(std::istream& is, Material material); // конструктор из потока
    Primitive* Intersect(const Ray &ray, double &t); // пересечение с лучем
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
Primitive* Box::Intersect(const Ray &ray, double &t) {
    double dx = ray.invDirection.x;
    double dy = ray.invDirection.y;
    double dz = ray.invDirection.z;

    double tmin = (min.x - ray.origin.x) * dx;
    double tmax = (max.x - ray.origin.x) * dx;
 
    if (tmin > tmax)
        std::swap(tmin, tmax); 
 
    double tymin = (min.y - ray.origin.y) * dy;
    double tymax = (max.y - ray.origin.y) * dy;

    if (tymin > tymax)
        std::swap(tymin, tymax); 
 
    if ((tmin > tymax) || (tymin > tmax)) {
        t = INF;
        return nullptr;
    }
 
    if (tymin > tmin) 
        tmin = tymin; 
 
    if (tymax < tmax) 
        tmax = tymax; 

    double tzmin = (min.z - ray.origin.z) * dz;
    double tzmax = (max.z - ray.origin.z) * dz;
 
    if (tzmin > tzmax) 
        std::swap(tzmin, tzmax); 
 
    if ((tmin > tzmax) || (tzmin > tmax)) {
        t = INF;
        return nullptr;
    }
 
    if (tzmin > tmin) 
        tmin = tzmin; 
 
    if (tzmax < tmax) 
        tmax = tzmax;

    t = tmin;
    if (tmin < EPSILON)
        t = tmax;

    if (t < EPSILON) {
        t = INF;
        return nullptr;
    }

    return this;
}

// получение нормали
Vec Box::GetNormal(const Vec& point) {
    return (point - center).Normalized();
}