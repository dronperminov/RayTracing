#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>

#include "Geometry/Constants.hpp"
#include "Geometry/Vec.hpp"
#include "Geometry/Ray.hpp"

#include "Primitives/Primitive.hpp"
#include "Primitives/Sphere.hpp"
#include "Primitives/Triangle.hpp"
#include "Primitives/InfinityFlatness.hpp"
#include "Primitives/Flatness.hpp"
#include "Primitives/ChessFlatness.hpp"
#include "Primitives/Disk.hpp"
#include "Primitives/Mandelbulb.hpp"
#include "Primitives/Torus.hpp"
#include "Primitives/Box.hpp"
#include "Primitives/ObjModel.hpp"
#include "Primitives/BoundingBox.hpp"

#include "Material.hpp"
#include "Light.hpp"
#include "Utils/Camera.hpp"
#include "Utils/Picture.hpp"

class RayTracer {
    Vec backgroundColor;
    std::map<std::string, Material> materials; // материалы
    std::vector<Light *> lights; // источники света
    std::vector<Primitive *> primitives; // примитивы
    Picture *envmap;

    Primitive* FindIntersection(const Ray &ray, double tmin, double tmax, double &t); // поиск ближайшего к лучу примитива
    bool HaveIntersection(const Ray &ray, double tmin, double tmax); // проверка наличия пересечения луча

    Vec Shading(const Vec &point, const Vec &direction, const Vec &normal, const Vec &color, const Material& material); // обсчёт освещения
    Vec GetReflectColor(const Vec &point, const Vec &direction, const Vec &normal, Material material, int depth);
    Vec GetRefractColor(const Vec &point, const Vec &direction, const Vec &normal, Material material, int depth);
    Vec CastRay(const Ray &ray, double tmin, double tmax, int depth); // трассировка луча
public:
    void ReadScene(const std::string& path); // считывание сцены
    Picture CastRays(int width, int height, const Camera &camera, int maxDepth, int antialiasing);

    ~RayTracer();
};

// поиск ближайшего к лучу примитива
Primitive* RayTracer::FindIntersection(const Ray &ray, double tmin, double tmax, double &t) {
    t = INF; // предполагаем, что нет пересечения
    Primitive *primitive = nullptr;

    // проходимся по всем фигурам
    for (size_t i = 0; i < primitives.size(); i++) {
        double ti;
        Primitive *pi = primitives[i]->Intersect(ray, ti);

        if (pi && ti >= tmin && ti < tmax && ti < t) {
            t = ti; // обновляем ближайшее расстоние
            primitive = pi; // и запоминаем ближайший примитив
        }
    }

    return primitive;
}

// проверка наличия пересечения луча
bool RayTracer::HaveIntersection(const Ray &ray, double tmin, double tmax) {
    for (size_t i = 0; i < primitives.size(); i++) {
        double ti;
        Primitive *pi = primitives[i]->Intersect(ray, ti);

        if (pi && ti >= tmin && ti < tmax)
            return true;
    }

    return false;
}

// обсчёт освещения
Vec RayTracer::Shading(const Vec &point, const Vec &direction, const Vec &normal, const Vec &color, const Material& material) {
    Vec diffuse(0, 0, 0);
    Vec specular(0, 0, 0);

    // проходимся по всем источникам освещения
    for (size_t i = 0; i < lights.size(); i++) {
        if (lights[i]->GetType() == LightType::Ambient) {
            diffuse += lights[i]->GetColor() * lights[i]->GetEnergy();
            continue;
        }

        Vec lightDir; // вектор источника света
        double tmax; // максимальное расстояние
        double norm = 1;

        if (lights[i]->GetType() == LightType::Point) {
            lightDir = ((PointLight*) lights[i])->GetPosition() - point;
            tmax = lightDir.Norm() - EPSILON;
            lightDir = lightDir.Normalized();
            norm = tmax;
        }
        else if (lights[i]->GetType() == LightType::Directional) {
            lightDir = ((DirectionalLight *) lights[i])->GetDirection();
            tmax = INF;
        }
        else if (lights[i]->GetType() == LightType::Spot) {
            SpotLight *light = (SpotLight *) lights[i];

            lightDir = light->GetPosition() - point;
            tmax = lightDir.Norm() - EPSILON;

            if (acos(light->GetDirection().Dot(lightDir.Normalized())) > light->GetAnlge())
                continue;

            lightDir = lightDir.Normalized();
            norm = tmax;
        }

        // если есть объекты на пути к источнику света, то объект в тени
        if (HaveIntersection(Ray(point, lightDir), EPSILON, tmax))
            continue;

        double diffuseIntensity = std::max(0.0, lightDir.Dot(normal));
        double specularIntensity = std::max(0.0, lightDir.Reflect(normal).Dot(direction));

        diffuse += lights[i]->GetColor() * (diffuseIntensity * lights[i]->GetEnergy() / norm);
        specular += lights[i]->GetColor() * (pow(specularIntensity, material.s) * lights[i]->GetEnergy() / norm);
    }

    diffuse *= material.diffuse;
    specular *= material.specular;

    return color * diffuse + specular;
}

Vec RayTracer::GetReflectColor(const Vec &point, const Vec &direction, const Vec &normal, Material material, int depth) {
    if (material.reflection == 0) 
        return Vec();

    Vec reflected = direction.Reflect(normal);
    Vec color = CastRay(Ray(point, reflected), EPSILON, INF, depth);

    return color * material.reflection;
}

Vec RayTracer::GetRefractColor(const Vec &point, const Vec &direction, const Vec &normal, Material material, int depth) {
    if (material.refraction == 0)
        return Vec();

    double eta = material.eta;
    int sign = 1;
    double cs = -direction.Dot(normal);

    if (cs > 0){
        eta = 1 / eta;
        sign = -1;
    }

    double k = 1 - eta * eta * (1 - cs * cs);

    if (k < 0)
        return Vec();

    Vec refracted = direction * eta + normal * (eta * cs + sign * sqrt(k));
    Vec color = CastRay(Ray(point, refracted), EPSILON, INF, depth);

    return color * material.refraction;
}

// трассировка луча
Vec RayTracer::CastRay(const Ray &ray, double tmin, double tmax, int depth) {
    double t;
    Primitive *primitive = FindIntersection(ray, tmin, tmax, t); // находим ближайший объект, с которым пересекается луч

    // если луч ни с чем не пересекается
    if (primitive == nullptr) {
        if (envmap == nullptr)
            return backgroundColor;

        double ax = atan2(ray.direction.z, ray.direction.x) / (2 * M_PI) + 0.5;
        double ay = acos(ray.direction.y) / M_PI;

        int x = std::max(0, std::min(envmap->Width() - 1, (int) (ax * envmap->Width())));
        int y = std::max(0, std::min(envmap->Height() - 1, (int) (ay * envmap->Height())));

        return envmap->GetPixel(x, y); // возвращаем цвет фона
    }

    Vec point = ray.GetPoint(t); // находим точку перемесения луча с объектом
    Vec normal = primitive->GetNormal(point); // получаем нормаль в этой точке
    Vec direction = ray.direction;
    Material material = primitive->GetMaterial(point); // получаем материал ближайшего объекта
    Vec color = primitive->GetColor(point); // получаем цвет объекта в точке

    Vec rayColor = Shading(point, direction, normal, color, material);

    if (depth <= 0) // если достигли минимальной глубины
        return rayColor; // то возвращаем диффузный и зеркальный цвет 

    rayColor = rayColor + GetReflectColor(point, direction, normal, material, depth - 1);
    rayColor = rayColor + GetRefractColor(point, direction, normal, material, depth - 1);

    return rayColor;
}

// считывание сцены
void RayTracer::ReadScene(const std::string& path) {
    std::ifstream sceneFile(path.c_str());

    if (!sceneFile) {
        std::cout << "Unable to read scene '" << path << "'" << std::endl;
        return;
    }

    envmap = nullptr;
    std::string line;
    // считываем построчно сцену
    while (std::getline(sceneFile, line)) {
        if (line == "" || line[0] == '#' || line == "\r") // пропускаем пустые строки и комментарии
            continue;

        std::string type; // тип объекта
        std::stringstream ss(line);
        ss >> type; // считываем тип объекта

        if (type == "background") {
            ss >> backgroundColor;
            backgroundColor = backgroundColor / 255;
            continue;
        }

        if (type == "envmap") {
            std::string filepath;
            ss >> filepath;
            envmap = new Picture(filepath);
            continue;
        }

        std::string name; // название объекта
        ss >> name; // считываем название материала

        if (type == "material") {
            materials[name] = Material(ss);
        }
        else if (type == "sphere") {
            primitives.push_back(new Sphere(ss, materials[name]));
        }
        else if (type == "textured_sphere") {
            primitives.push_back(new TexturedSphere(ss, materials[name]));
        }
        else if (type == "triangle") {
            primitives.push_back(new Triangle(ss, materials[name]));
        }
        else if (type == "infinityflatness") {
            primitives.push_back(new InfinityFlatness(ss, materials[name]));
        }
        else if (type == "flatness") {
            primitives.push_back(new Flatness(ss, materials[name]));
        }
        else if (type == "chessflatness") {
            std::string name2;
            ss >> name2;
            primitives.push_back(new ChessFlatness(ss, materials[name], materials[name2]));
        }
        else if (type == "disk") {
            primitives.push_back(new Disk(ss, materials[name]));
        }
        else if (type == "mandelbulb") {
            primitives.push_back(new Mandelbulb(ss, materials[name]));
        }
        else if (type == "torus") {
            primitives.push_back(new Torus(ss, materials[name]));
        }
        else if (type == "box") {
            primitives.push_back(new Box(ss, materials[name]));
        }
        else if (type == "model") {
            primitives.push_back(new ObjModel(ss, materials[name]));
        }
        else if (type == "light" && name == "ambient") {
            lights.push_back(new AmbientLight(ss));
        }
        else if (type == "light" && name == "point") {
            lights.push_back(new PointLight(ss));
        }
        else if (type == "light" && name == "directional") {
            lights.push_back(new DirectionalLight(ss));
        }
        else if (type == "light" && name == "spot") {
            lights.push_back(new SpotLight(ss));
        }
        else {
            std::cout << "Warning: unknown object '" << name << "'" << std::endl;
        }
    }

    sceneFile.close(); // закрываем файл
}

// трассировка лучей
Picture RayTracer::CastRays(int width, int height, const Camera &camera, int maxDepth, int antialiasing) {
    double size = width > height ? width : height;
    double ds = 0.5 / size;

    Picture picture(width, height);

    #pragma omp parallel for collapse(2)
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            double wx = x / size - 0.5;
            double wy = 0.5 - y / size;

            Vec color;

            if (antialiasing == 1) {
                color = CastRay(camera.GetRay(wx, wy), 0, INF, maxDepth);
            }
            else if (antialiasing == 4) {
                Vec c1 = CastRay(camera.GetRay(wx - ds, wy - ds), 0, INF, maxDepth);
                Vec c2 = CastRay(camera.GetRay(wx + ds, wy - ds), 0, INF, maxDepth);
                Vec c3 = CastRay(camera.GetRay(wx, wy), 0, INF, maxDepth);
                Vec c4 = CastRay(camera.GetRay(wx, wy + ds), 0, INF, maxDepth);

                color = (c1 + c2 + c3 + c4) / 4;
            }
            else if (antialiasing == 9) {
                Vec c1 = CastRay(camera.GetRay(wx - ds, wy - ds), 0, INF, maxDepth);
                Vec c2 = CastRay(camera.GetRay(wx, wy - ds), 0, INF, maxDepth);
                Vec c3 = CastRay(camera.GetRay(wx + ds, wy - ds), 0, INF, maxDepth);

                Vec c4 = CastRay(camera.GetRay(wx - ds, wy), 0, INF, maxDepth);
                Vec c5 = CastRay(camera.GetRay(wx, wy), 0, INF, maxDepth);
                Vec c6 = CastRay(camera.GetRay(wx + ds, wy), 0, INF, maxDepth);

                Vec c7 = CastRay(camera.GetRay(wx - ds, wy + ds), 0, INF, maxDepth);
                Vec c8 = CastRay(camera.GetRay(wx, wy + ds), 0, INF, maxDepth);
                Vec c9 = CastRay(camera.GetRay(wx + ds, wy + ds), 0, INF, maxDepth);

                color = (c1 + c2 + c3 + c4 + c5 + c6 + c7 + c8 + c9) / 9;
            }

            picture.SetPixel(x, y, color);
        }
    }

    return picture;
}

RayTracer::~RayTracer() {
    for (size_t i = 0; i < primitives.size(); i++)
        delete primitives[i];

    for (size_t i = 0; i < lights.size(); i++)
        delete lights[i];
}
