#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include <iostream>
#include "Primitive.hpp"

class Triangle : public Primitive {
    Vec v1, v2, v3; // вершины
    Vec v12, v13; // вспомогательные вектора
    Vec normal; // нормаль

public:
    Triangle(const Vec& v1, const Vec& v2, const Vec& v3, Material material);
    Triangle(std::istream& is, Material material); // создание из потока
    
    Primitive* Intersect(const Ray &ray, double &t); // пересечение с лучём
    Vec GetNormal(const Vec &point); // получение нормали
    void UpdateBbox(Vec& min, Vec &max); // обновление ограничивающего объёма
};

Triangle::Triangle(const Vec& v1, const Vec& v2, const Vec& v3, Material material) {
    this->v1 = v1;
    this->v2 = v2;
    this->v3 = v3;
    this->material = material;

    v12 = v2 - v1;
    v13 = v3 - v1;
    normal = v12.Cross(v13).Normalized(); // нормаль = векторное произведение
}

// создание из потока
Triangle::Triangle(std::istream& is, Material material) {
    this->material = material;
    is >> v1 >> v2 >> v3;
    v12 = v2 - v1;
    v13 = v3 - v1;
    normal = v12.Cross(v13).Normalized(); // нормаль = векторное произведение
}

// пересечение с лучём
Primitive* Triangle::Intersect(const Ray &ray, double &t) {
    Vec h = ray.direction.Cross(v13);
    double a = v12.Dot(h);

    if (fabs(a) < EPSILON) {
        t = INF;
        return nullptr;
    }

    double f = 1.0/a;
    Vec s = ray.origin - v1;
    double u = f * s.Dot(h);

    if (u < 0.0 || u > 1.0) {
        t = INF;
        return nullptr;
    }

    Vec q = s.Cross(v12);
    double v = f * ray.direction.Dot(q);

    if (v < 0.0 || u + v > 1.0) {
        t = INF;
        return nullptr;
    }

    t = f * v13.Dot(q);

    if (t < EPSILON) {
        t = INF;
        return nullptr;
    }

    return this;
}

// получение нормали
Vec Triangle::GetNormal(const Vec &point) {
    return normal;
}

// обновление ограничивающего объёма
void Triangle::UpdateBbox(Vec& min, Vec &max) {
    min = min.Min(v1).Min(v2).Min(v3);
    max = max.Max(v1).Max(v2).Max(v3);
}

#endif