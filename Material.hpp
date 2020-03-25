#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <iostream>
#include "Geometry/Vec.hpp"

struct Material {
    Vec color; // основной цвет
    double s; // коэффициент блеска
    double eta; // коэффициент среды

    double diffuse; // коэффициент диффузности
    double specular; // коэффициент зеркальности
    double reflection; // коэффициент отражения
    double refraction; // коэффициент преломления 

    Material();
    Material(std::istream &is);

};

Material::Material() {
    color = Vec(rand() % 255, rand() % 255, rand() % 255) / 255.0;
    s = rand() % 1500;
    eta = 1;

    diffuse = 1;
    specular = 1;
    reflection = (rand() % 11) / 10.0;
    refraction = 0;
}

Material::Material(std::istream &is) {
    is >> color;
    is >> s >> eta;
    is >> diffuse;
    is >> specular;
    is >> reflection;
    is >> refraction;

    color = color / 255.0;
}

#endif