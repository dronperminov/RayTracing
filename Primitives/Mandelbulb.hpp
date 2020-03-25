#pragma once

#include <iostream>
#include "RayMarching.hpp"

// фрактал вида Zn+1 = Zn^power + C
class Mandelbulb : public RayMarching {
    double power;
    double radius;
    Vec center;

public:
    Mandelbulb(std::istream &is, Material material); // конструктор из потока

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
Mandelbulb::Mandelbulb(std::istream &is, Material material) : RayMarching(material) {
    is >> center >> power >> radius;
}