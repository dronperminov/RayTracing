#pragma once

#include <iostream>
#include "Primitive.hpp"
#include "../Utils/Picture.hpp"

class Sphere : public Primitive {
    Vec center; // центр
    double radius; // радиус

public:
    Sphere(Vec center, double radius, Material material);
    Sphere(std::istream& is, Material material); // создание из потока
    
    Primitive* Intersect(const Ray &ray, double tmin, double tmax, double &t); // пересечение с лучём
    Vec GetNormal(const Vec &point); // получение нормали в точке
    void UpdateBbox(Vec& min, Vec &max); // обновление ограничивающего объёма
};

class TexturedSphere : public Sphere {
    Picture texture;
public:
    TexturedSphere(std::istream& is, Material material); // создание из потока
    Vec GetColor(const Vec &point);
};

Sphere::Sphere(Vec center, double radius, Material material) {
    this->center = center;
    this->radius = radius;
    this->material = material;
}

// создание из потока
Sphere::Sphere(std::istream& is, Material material) {
    this->material = material;
    is >> center >> radius;
}

// пересечение сферы с лучём
Primitive* Sphere::Intersect(const Ray &ray, double tmin, double tmax, double &t) {
    Vec oc = ray.origin - center;

    double k = -oc.Dot(ray.direction);
    double d = k * k - oc.Dot(oc) + radius * radius;

    if (d < EPSILON)
        return nullptr;

    d = sqrt(d);

    t = k - d;

    if (t < tmin)
        t = k + d;

    if (t < tmin || t >= tmax)
        return nullptr;

    return this;
}

// получение нормали в точке
Vec Sphere::GetNormal(const Vec &point) {
    return (point - center) * (1.0 / radius);
}

// обновление ограничивающего объёма
void Sphere::UpdateBbox(Vec& min, Vec &max) {
    min = min.Min(center - Vec(radius, radius, radius));
    max = max.Max(center + Vec(radius, radius, radius));
}

// создание из потока
TexturedSphere::TexturedSphere(std::istream& is, Material material) : Sphere(is, material), texture(is) {

}

Vec TexturedSphere::GetColor(const Vec &point) {
    Vec normal = GetNormal(point);
    double u = atan2(normal.z, normal.x) / (2 * M_PI) + 0.5;
    double v = acos(normal.y) / M_PI;

    return texture.GetPixel(u, v); // возвращаем цвет фона
}
