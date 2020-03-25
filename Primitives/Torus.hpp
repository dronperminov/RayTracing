#pragma once

#include <iostream>
#include "RayMarching.hpp"

class Torus : public RayMarching {
    double radius1; // первый радиус
    double radius2; // второй радиус
    Vec center; // центр
public:
    Torus(std::istream& is, Material material); // конструктор из потока
    double DE(const Vec &point);
};

// конструктор из потока
Torus::Torus(std::istream& is, Material material) : RayMarching(material) {
    is >> center >> radius1 >> radius2;
}

double Torus::DE(const Vec &point) {
    double xz = sqrt((point.x - center.x) * (point.x - center.x) + (point.y - center.y) * (point.y - center.y));
    double x = xz - radius1;
    double y = point.z - center.z;
    
    return sqrt(x*x + y*y) - radius2;
}