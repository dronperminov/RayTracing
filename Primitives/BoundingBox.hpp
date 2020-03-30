#pragma once

#include <iostream>
#include "Primitive.hpp"

class BoundingBox : public Primitive {
    std::vector<Primitive *> primitives;
    Vec min;
    Vec max;
public:
    BoundingBox();

    Primitive* Intersect(const Ray &ray, double tmin, double tmax, double &t); // поиск пересечения
    Vec GetNormal(const Vec &point); // нормаль
    Material GetMaterial(const Vec &point); // получение цвета в точке

    void AddPrimitive(Primitive *primitive); // добавление примитива
    void AddBox(BoundingBox *box);
};

BoundingBox::BoundingBox() {
    min = Vec(INF, INF, INF);
    max = Vec(-INF, -INF, -INF);
}

// поиск пересечения
Primitive* BoundingBox::Intersect(const Ray &ray, double tmin, double tmax, double &t) {
    double dx = ray.invDirection.x;
    double dy = ray.invDirection.y;
    double dz = ray.invDirection.z;

    double t_min = (min.x - ray.origin.x) * dx;
    double t_max = (max.x - ray.origin.x) * dx;

    if (t_min > t_max)
        std::swap(t_min, t_max);

    double tymin = (min.y - ray.origin.y) * dy;
    double tymax = (max.y - ray.origin.y) * dy;

    if (tymin > tymax)
        std::swap(tymin, tymax);

    if ((t_min > tymax) || (tymin > t_max))
        return nullptr;

    if (tymin > t_min)
        t_min = tymin;

    if (tymax < t_max)
        t_max = tymax;

    double tzmin = (min.z - ray.origin.z) * dz;
    double tzmax = (max.z - ray.origin.z) * dz;

    if (tzmin > tzmax)
        std::swap(tzmin, tzmax);

    if ((t_min > tzmax) || (tzmin > t_max))
        return nullptr;

    if (tzmin > t_min)
        t_min = tzmin;

    if (tzmax < t_max)
        t_max = tzmax;

    t = t_min;

    if (t_min < tmin)
        t = t_max;

    if (t < tmin || t >= tmax)
        return nullptr;

    t = tmax;
    Primitive *nearest = nullptr;

    for (size_t i = 0; i < primitives.size(); i++) {
        double ti;
        Primitive *pi = primitives[i]->Intersect(ray, tmin, tmax, ti);

        if (pi && ti < t) {
            t = ti; // обновляем ближайшее расстоние
            nearest = pi;
        }
    }

    return nearest;
}

// нормаль
Vec BoundingBox::GetNormal(const Vec &point) {
    // return nearest->GetNormal(point);
    throw "";
}

// получение цвета в точке
Material BoundingBox::GetMaterial(const Vec &point) {
    // return nearest->GetMaterial(point);
    throw "";
}

// добавление примитива
void BoundingBox::AddPrimitive(Primitive *primitive) {
    primitives.push_back(primitive);
    primitive->UpdateBbox(min, max);
}

void BoundingBox::AddBox(BoundingBox *primitive) {
    primitives.push_back(primitive);
    this->min = this->min.Min(primitive->min);
    this->max = this->max.Max(primitive->max);
}
