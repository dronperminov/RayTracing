#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>

#include "Geometry/Constants.hpp"
#include "Geometry/Vec.hpp"
#include "Geometry/Ray.hpp"
#include "Utils/Scene.hpp"
#include "Utils/Camera.hpp"
#include "Utils/Picture.hpp"

class RayTracer {
    Vec GetReflectColor(const Scene& scene, const Vec &point, const Vec &direction, const Vec &normal, Material material, int depth);
    Vec GetRefractColor(const Scene& scene, const Vec &point, const Vec &direction, const Vec &normal, Material material, int depth);
    Vec CastRay(const Scene& scene, const Ray &ray, double tmin, double tmax, int depth); // трассировка луча
public:
    Picture Render(const Scene& scene, int antialiasing);
};

Vec RayTracer::GetReflectColor(const Scene& scene, const Vec &point, const Vec &direction, const Vec &normal, Material material, int depth) {
    if (material.reflection == 0) 
        return Vec();

    Vec reflected = direction.Reflect(normal);

    return CastRay(scene, Ray(point, reflected), EPSILON, INF, depth) * material.reflection;
}

Vec RayTracer::GetRefractColor(const Scene& scene, const Vec &point, const Vec &direction, const Vec &normal, Material material, int depth) {
    if (material.refraction == 0)
        return Vec();

    double cs = -direction.Dot(normal);
    double eta = cs > 0 ? 1.0 / material.eta : material.eta;
    int sign = cs > 0 ? -1 : 1;
    double k = 1 - eta * eta * (1 - cs * cs);

    if (k < 0)
        return Vec();

    Vec refracted = direction * eta + normal * (eta * cs + sign * sqrt(k));
    return CastRay(scene, Ray(point, refracted), EPSILON, INF, depth) * material.refraction;
}

// трассировка луча
Vec RayTracer::CastRay(const Scene& scene, const Ray &ray, double tmin, double tmax, int depth) {
    double t;
    Primitive *primitive = scene.FindIntersection(ray, tmin, tmax, t); // находим ближайший объект, с которым пересекается луч

    // если луч ни с чем не пересекается
    if (primitive == nullptr) {
        if (scene.envmap == nullptr)
            return scene.backgroundColor;

        double ax = atan2(ray.direction.z, ray.direction.x) / (2 * M_PI) + 0.5;
        double ay = acos(ray.direction.y) / M_PI;

        return scene.envmap->GetPixel(ax, ay); // возвращаем цвет фона
    }

    Vec point = ray.GetPoint(t); // находим точку перемесения луча с объектом
    Vec normal = primitive->GetNormal(point); // получаем нормаль в этой точке
    Material material = primitive->GetMaterial(point); // получаем материал ближайшего объекта
    Vec color = primitive->GetColor(point); // получаем цвет объекта в точке

    Vec rayColor = scene.Shading(point, ray.direction, normal, color, material);

    if (depth <= 0) // если достигли минимальной глубины
        return rayColor; // то возвращаем диффузный и зеркальный цвет 

    rayColor += GetReflectColor(scene, point, ray.direction, normal, material, depth - 1);
    rayColor += GetRefractColor(scene, point, ray.direction, normal, material, depth - 1);

    return rayColor;
}

// трассировка лучей
Picture RayTracer::Render(const Scene& scene, int antialiasing) {
    double size = std::max(scene.width, scene.height);
    double ds = 0.5 / size;

    Picture picture(scene.width, scene.height);
    Camera camera(scene.position, scene.rotation);

    #pragma omp parallel for collapse(2)
    for (int x = 0; x < scene.width; x++) {
        for (int y = 0; y < scene.height; y++) {
            double wx = x / size - 0.5;
            double wy = 0.5 - y / size;

            Vec color;

            if (antialiasing == 1) {
                color = CastRay(scene, camera.GetRay(wx, wy), 0, INF, scene.depth);
            }
            else if (antialiasing == 4) {
                Vec c1 = CastRay(scene, camera.GetRay(wx - ds, wy - ds), 0, INF, scene.depth);
                Vec c2 = CastRay(scene, camera.GetRay(wx + ds, wy - ds), 0, INF, scene.depth);
                Vec c3 = CastRay(scene, camera.GetRay(wx, wy), 0, INF, scene.depth);
                Vec c4 = CastRay(scene, camera.GetRay(wx, wy + ds), 0, INF, scene.depth);

                color = (c1 + c2 + c3 + c4) / 4;
            }
            else if (antialiasing == 9) {
                Vec c1 = CastRay(scene, camera.GetRay(wx - ds, wy - ds), 0, INF, scene.depth);
                Vec c2 = CastRay(scene, camera.GetRay(wx, wy - ds), 0, INF, scene.depth);
                Vec c3 = CastRay(scene, camera.GetRay(wx + ds, wy - ds), 0, INF, scene.depth);

                Vec c4 = CastRay(scene, camera.GetRay(wx - ds, wy), 0, INF, scene.depth);
                Vec c5 = CastRay(scene, camera.GetRay(wx, wy), 0, INF, scene.depth);
                Vec c6 = CastRay(scene, camera.GetRay(wx + ds, wy), 0, INF, scene.depth);

                Vec c7 = CastRay(scene, camera.GetRay(wx - ds, wy + ds), 0, INF, scene.depth);
                Vec c8 = CastRay(scene, camera.GetRay(wx, wy + ds), 0, INF, scene.depth);
                Vec c9 = CastRay(scene, camera.GetRay(wx + ds, wy + ds), 0, INF, scene.depth);

                color = (c1 + c2 + c3 + c4 + c5 + c6 + c7 + c8 + c9) / 9;
            }

            picture.SetPixel(x, y, color);
        }
    }

    return picture;
}