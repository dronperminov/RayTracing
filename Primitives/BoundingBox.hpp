#ifndef BOUNDING_BOX_HPP
#define BOUNDING_BOX_HPP

#include <iostream>
#include "Primitive.hpp"

class BoundingBox : public Primitive {
    std::vector<Primitive *> primitives;
    Vec min;
    Vec max;
public:
    BoundingBox();
    
    Primitive* Intersect(const Ray &ray, double &t); // поиск пересечения
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
Primitive* BoundingBox::Intersect(const Ray &ray, double &t) {
    double dx = ray.GetInvDirection().x;
    double dy = ray.GetInvDirection().y;
    double dz = ray.GetInvDirection().z;

    double tmin = (min.x - ray.GetOrigin().x) * dx;
    double tmax = (max.x - ray.GetOrigin().x) * dx;
 
    if (tmin > tmax)
        std::swap(tmin, tmax); 
 
    double tymin = (min.y - ray.GetOrigin().y) * dy;
    double tymax = (max.y - ray.GetOrigin().y) * dy;

    if (tymin > tymax)
        std::swap(tymin, tymax); 
 
    if ((tmin > tymax) || (tymin > tmax)) {
        t = INF;
        return nullptr;
    }
 
    if (tymin > tmin) 
        tmin = tymin; 
 
    if (tymax < tmax) 
        tmax = tymax; 

    double tzmin = (min.z - ray.GetOrigin().z) * dz;
    double tzmax = (max.z - ray.GetOrigin().z) * dz;
 
    if (tzmin > tzmax) 
        std::swap(tzmin, tzmax); 
 
    if ((tmin > tzmax) || (tzmin > tmax)) {
        t = INF;
        return nullptr;
    }
 
    if (tzmin > tmin) 
        tmin = tzmin; 
 
    if (tzmax < tmax) 
        tmax = tzmax;


    t = INF;
    Primitive *nearest = nullptr;

    for (size_t i = 0; i < primitives.size(); i++) {
        double ti;
        Primitive *pi = primitives[i]->Intersect(ray, ti);

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

#endif