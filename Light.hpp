#pragma once

#include <iostream>
#include "Geometry/Vec.hpp"

// тип света
enum class LightType {
    Ambient, // постоянный источник
    Point, // точечный источник
    Directional, // направленный источник
    Spot // конусный источник
};

struct Light {
    LightType type; // тип источника
    Vec color;
    double energy; // энергия света
};

// постоянный источник света
struct AmbientLight : public Light {
    AmbientLight(std::istream &is);
};

// точечный источник света
struct PointLight : public Light {
    Vec position; // положение света
    PointLight(std::istream &is);
};

// направленный источник света
struct DirectionalLight : public Light {
    Vec direction; // направление света
    DirectionalLight(std::istream &is);
};

struct SpotLight : public Light {
    Vec position;
    Vec direction;
    double angle;
    SpotLight(std::istream &is);
};

AmbientLight::AmbientLight(std::istream &is) {
    this->type = LightType::Ambient;
    is >> energy >> color;
    color = color / 255;
}

PointLight::PointLight(std::istream &is) {
    this->type = LightType::Point;
    is >> energy >> color >> position;
    color = color / 255;
}

DirectionalLight::DirectionalLight(std::istream &is) {
    this->type = LightType::Directional;
    is >> energy >> color >> direction;
    color = color / 255;
    direction.Normalize();
}

SpotLight::SpotLight(std::istream &is) {
    this->type = LightType::Spot;
    is >> energy >> color >> position >> direction >> angle;
    color = color / 255;
    direction.Normalize();
    angle = angle / 360 * M_PI;
}
