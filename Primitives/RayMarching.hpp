#pragma once

#include <iostream>
#include "Primitive.hpp"

const int MAX_RAY_STEPS = 2048;

class RayMarching : public Primitive {
public:
    RayMarching(Material material); // конструктор из потока

    Primitive* Intersect(const Ray &ray, double tmin, double tmax, double &t); // пересечение с лучём
    Vec GetNormal(const Vec &point); // получение нормали

    virtual double DE(const Vec &pos) = 0;
};

// конструктор из потока
RayMarching::RayMarching(Material material) {
    this->material = material;
}

// пересечение с лучём
Primitive* RayMarching::Intersect(const Ray &ray, double tmin, double tmax, double &t) {
    t = 0;
    
    for (int steps = 0; steps < MAX_RAY_STEPS; steps++) {
        double distance = DE(ray.GetPoint(t));
        
        if (distance < EPSILON)
            return t >= tmin && t < tmax ? this : nullptr;

        t += distance;
    }

    return nullptr;
}

// получение нормали
Vec RayMarching::GetNormal(const Vec &point) {
    double EPS = 0.01;
    double x = point.x;
    double y = point.y;
    double z = point.z;

    double dx = DE(Vec(x + EPS, y, z)) - DE(Vec(x - EPS, y, z));
    double dy = DE(Vec(x, y + EPS, z)) - DE(Vec(x, y - EPS, z));
    double dz = DE(Vec(x, y, z + EPS)) - DE(Vec(x, y, z - EPS));

    return Vec(dx, dy, dz).Normalized();
}