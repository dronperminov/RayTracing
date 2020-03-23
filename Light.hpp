#ifndef LIGHT_HPP
#define LIGHT_HPP

#include <iostream>

#include "Geometry/Vec.hpp"

// тип света
enum class LightType {
    Ambient, // постоянный источник
    Point, // точечный источник
    Directional // направленный источник
};

class Light {
protected:
    LightType type; // тип источника
    double energy; // энергия света
public:
    LightType GetType() const; // получение типа
    double GetEnergy() const; // получение энергии
};

// постоянный источник света
class AmbientLight : public Light {
public:
    AmbientLight(double energy);
    AmbientLight(std::istream &is);
};

// точечный источник света
class PointLight : public Light {
    Vec position; // положение света
public:
    PointLight(double energy, const Vec& position);
    PointLight(std::istream &is);
    Vec GetPosition() const; // получение позиции
};

// направленный источник света
class DirectionalLight : public Light {
    Vec direction; // направление света
public:
    DirectionalLight(double energy, const Vec& direction);
    DirectionalLight(std::istream &is);
    Vec GetDirection() const; // получение направления
};

LightType Light::GetType() const {
    return type;
}

// получение энергии
double Light::GetEnergy() const {
    return energy;
}

AmbientLight::AmbientLight(double energy) {
    this->type = LightType::Ambient;
    this->energy = energy;
}

AmbientLight::AmbientLight(std::istream &is) {
    this->type = LightType::Ambient;
    is >> energy;
}

PointLight::PointLight(double energy, const Vec& position) {
    this->type = LightType::Point;
    this->energy = energy;
    this->position = position;
}

PointLight::PointLight(std::istream &is) {
    this->type = LightType::Point;
    is >> energy >> position;
}

// получение позиции
Vec PointLight::GetPosition() const {
    return position;
}

DirectionalLight::DirectionalLight(double energy, const Vec& direction) {
    this->type = LightType::Directional;
    this->energy = energy;
    this->direction = direction.Normalized();
}

DirectionalLight::DirectionalLight(std::istream &is) {
    this->type = LightType::Directional;
    is >> energy >> direction;
    direction = direction.Normalized();
}

// получение направления
Vec DirectionalLight::GetDirection() const {
    return direction;
}

#endif