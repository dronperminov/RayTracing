#pragma once

#include <iostream>
#include "Vec.hpp"

struct Ray {
    Vec origin; // начало луча
    Vec direction; // направление луча
    Vec invDirection;

    Ray(const Vec& origin, const Vec& direction);

    Vec GetPoint(double t) const; // получение точки
};

Ray::Ray(const Vec& origin, const Vec& direction) {
    this->origin = origin;
    this->direction = direction;
    this->invDirection = direction.Inv();
}

// получение точки
Vec Ray::GetPoint(double t) const {
    return origin + direction * t;
}

// получение наименьшего числа, большего threshold
double MinThreshold(double t1, double t2, double threshold) {
    if (t1 > threshold)
        return t1;

    if (t2 > threshold)
        return t2;

    return INF;
}
