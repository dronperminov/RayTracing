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
    
    double Intersect(const Ray &ray); // пересечение с лучём
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
double Triangle::Intersect(const Ray &ray) {
    Vec h = ray.GetDirection().Cross(v13);
    double a = v12.Dot(h);

    if (fabs(a) < EPSILON)
        return INF;

    double f = 1.0/a;
    Vec s = ray.GetOrigin() - v1;
    double u = f * s.Dot(h);

    if (u < 0.0 || u > 1.0)
        return INF;

    Vec q = s.Cross(v12);
    double v = f * ray.GetDirection().Dot(q);

    if (v < 0.0 || u + v > 1.0)
        return INF;

    double t = f * v13.Dot(q);

    if (t < EPSILON)
        return INF;

    return t;
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