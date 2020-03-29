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

class Light {
protected:
    LightType type; // тип источника
    Vec color;
    double energy; // энергия света
public:
    LightType GetType() const; // получение типа
    double GetEnergy() const; // получение энергии
    Vec GetColor() const; // получение цвета
};

// постоянный источник света
class AmbientLight : public Light {
public:
    AmbientLight(std::istream &is);
};

// точечный источник света
class PointLight : public Light {
    Vec position; // положение света
public:
    PointLight(std::istream &is);
    Vec GetPosition() const; // получение позиции
};

// направленный источник света
class DirectionalLight : public Light {
    Vec direction; // направление света
public:
    DirectionalLight(std::istream &is);
    Vec GetDirection() const; // получение направления
};

class SpotLight : public Light {
    Vec position;
    Vec direction;
    double angle;
public:
    SpotLight(std::istream &is);
    double GetAnlge() const; // получение угла
    Vec GetPosition() const; // получение позиции
    Vec GetDirection() const; // получение направления
};

LightType Light::GetType() const {
    return type;
}

// получение энергии
double Light::GetEnergy() const {
    return energy;
}

// получение цвета
Vec Light::GetColor() const {
    return color;
}

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

// получение позиции
Vec PointLight::GetPosition() const {
    return position;
}

DirectionalLight::DirectionalLight(std::istream &is) {
    this->type = LightType::Directional;
    is >> energy >> color >> direction;
    color = color / 255;
    direction.Normalize();
}

// получение направления
Vec DirectionalLight::GetDirection() const {
    return direction;
}

SpotLight::SpotLight(std::istream &is) {
    this->type = LightType::Spot;
    is >> energy >> color >> position >> direction >> angle;
    color = color / 255;
    direction.Normalize();
    angle = angle / 360 * M_PI;
}

// получение угла
double SpotLight::GetAnlge() const {
    return angle;
}

// получение позиции
Vec SpotLight::GetPosition() const {
    return position;
}

// получение направления
Vec SpotLight::GetDirection() const {
    return direction;
}
