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
        Primitive *pi = primitive->Intersect(ray, tmin, tmax, ti);

        if (pi && ti < t) {
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
        if (primitive->Intersect(ray, tmin, tmax, t))
            return true;

    return false;
}

// расчёт освещения
Vec Scene::Shading(const Vec &point, const Vec &direction, const Vec &normal, const Vec &color, const Material& material) const {
    Vec diffuse(0, 0, 0);
    Vec specular(0, 0, 0);

    // проходимся по всем источникам освещения
    for (Light *light : lights) {
        if (light->type == LightType::Ambient) {
            diffuse += light->color * light->energy;
            continue;
        }

        Vec lightDir; // вектор источника света
        double tmax; // максимальное расстояние
        double energy = light->energy; // энергия источника

        if (light->type == LightType::Point) {
            lightDir = ((PointLight*) light)->position - point;
            tmax = lightDir.Norm() - EPSILON;
            lightDir.Normalize();
            energy /= tmax;
        }
        else if (light->type == LightType::Directional) {
            lightDir = ((DirectionalLight *) light)->direction;
            tmax = INF;
        }
        else if (light->type == LightType::Spot) {
            lightDir = ((SpotLight *) light)->direction - point;
            tmax = lightDir.Norm() - EPSILON;
            lightDir.Normalize();

            if (acos(((SpotLight *) light)->direction.Dot(lightDir)) > ((SpotLight *) light)->angle)
                continue;

            energy /= tmax;
        }

        // если есть объекты на пути к источнику света, то объект в тени
        if (HaveIntersection(Ray(point, lightDir), EPSILON, tmax))
            continue;

        double diffuseIntensity = std::max(0.0, lightDir.Dot(normal));
        double specularIntensity = std::max(0.0, lightDir.Reflect(normal).Dot(direction));

        diffuse += light->color * (diffuseIntensity * energy);
        specular += light->color * (pow(specularIntensity, material.s) * energy);
    }

    diffuse *= material.diffuse;
    specular *= material.specular;

    return color * diffuse + specular;
}