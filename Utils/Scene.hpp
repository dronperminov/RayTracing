#pragma once

#include <vector>
#include <map>
#include <string>

#include "../Geometry/Vec.hpp"
#include "../Material.hpp"
#include "../Light.hpp"
#include "../Primitives/Primitive.hpp"
#include "Picture.hpp"

struct Scene {
    Vec backgroundColor; // цвет фона
    Vec position; // точка просмотра
    Vec rotation; // поворот камеры

    int width; // ширина изображения
    int height; // высота изображения
    int depth; // глубина трассировки

    Picture *envmap; // карта окружения
    std::map<std::string, Material> materials; // материалы
    std::vector<Primitive*> primitives; // примитивы
    std::vector<Light*> lights; // источники света

    Primitive* FindIntersection(const Ray &ray, double tmin, double tmax, double &t) const; // поиск ближайшего к лучу примитива
    bool HaveIntersection(const Ray &ray, double tmin, double tmax) const; // проверка наличия пересечения луча
    Vec Shading(const Vec &point, const Vec &direction, const Vec &normal, const Vec &color, const Material& material) const; // расчёт освещения
};

// поиск ближайшего к лучу примитива
Primitive* Scene::FindIntersection(const Ray &ray, double tmin, double tmax, double &t) const {
    t = INF; // предполагаем, что нет пересечения
    Primitive *closest = nullptr;

    // проходимся по всем фигурам
    for (Primitive* primitive : primitives) {
        double ti;
        Primitive *pi = primitive->Intersect(ray, ti);

        if (pi && ti >= tmin && ti < tmax && ti < t) {
            t = ti; // обновляем ближайшее расстоние
            closest = pi; // и запоминаем ближайший примитив
        }
    }

    return closest;
}

// проверка наличия пересечения луча
bool Scene::HaveIntersection(const Ray &ray, double tmin, double tmax) const {
    double t;
    for (Primitive* primitive : primitives)
        if (primitive->Intersect(ray, t) && t >= tmin && t < tmax)
            return true;

    return false;
}

// расчёт освещения
Vec Scene::Shading(const Vec &point, const Vec &direction, const Vec &normal, const Vec &color, const Material& material) const {
    Vec diffuse(0, 0, 0);
    Vec specular(0, 0, 0);

    // проходимся по всем источникам освещения
    for (Light *light : lights) {
        if (light->GetType() == LightType::Ambient) {
            diffuse += light->GetColor() * light->GetEnergy();
            continue;
        }

        Vec lightDir; // вектор источника света
        double tmax; // максимальное расстояние
        double energy = light->GetEnergy(); // энергия источника

        if (light->GetType() == LightType::Point) {
            lightDir = ((PointLight*) light)->GetPosition() - point;
            tmax = lightDir.Norm() - EPSILON;
            lightDir.Normalize();
            energy /= tmax;
        }
        else if (light->GetType() == LightType::Directional) {
            lightDir = ((DirectionalLight *) light)->GetDirection();
            tmax = INF;
        }
        else if (light->GetType() == LightType::Spot) {
            lightDir = ((SpotLight *) light)->GetPosition() - point;
            tmax = lightDir.Norm() - EPSILON;
            lightDir.Normalize();

            if (acos(((SpotLight *) light)->GetDirection().Dot(lightDir)) > ((SpotLight *) light)->GetAnlge())
                continue;

            energy /= tmax;
        }

        // если есть объекты на пути к источнику света, то объект в тени
        if (HaveIntersection(Ray(point, lightDir), EPSILON, tmax))
            continue;

        double diffuseIntensity = std::max(0.0, lightDir.Dot(normal));
        double specularIntensity = std::max(0.0, lightDir.Reflect(normal).Dot(direction));

        diffuse += light->GetColor() * (diffuseIntensity * energy);
        specular += light->GetColor() * (pow(specularIntensity, material.s) * energy);
    }

    diffuse *= material.diffuse;
    specular *= material.specular;

    return color * diffuse + specular;
}