#pragma once

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

#include "Scene.hpp"

#include "../Primitives/Primitive.hpp"
#include "../Primitives/Sphere.hpp"
#include "../Primitives/Triangle.hpp"
#include "../Primitives/InfinityFlatness.hpp"
#include "../Primitives/Flatness.hpp"
#include "../Primitives/ChessFlatness.hpp"
#include "../Primitives/Disk.hpp"
#include "../Primitives/Mandelbulb.hpp"
#include "../Primitives/Torus.hpp"
#include "../Primitives/Box.hpp"
#include "../Primitives/ObjModel.hpp"
#include "../Primitives/BoundingBox.hpp"

class SceneReader {
    Scene InitScene(); // задание параметров по умолчанию
    void ReadParams(Scene &scene, const std::string& name, std::stringstream &ss);
    void ReadLight(Scene &scene, std::stringstream& ss); // считывание света
    void ReadPrimitive(Scene &scene, const std::string& type, std::stringstream& ss); // считывание примитива
public:
    Scene Read(const std::string& path); // считывание сцены
};

// задание параметров по умолчанию
Scene SceneReader::InitScene() {
    Scene scene;
    
    scene.backgroundColor = Vec(0, 0, 0); // чёрный фон
    scene.position = Vec(0, 0, 0); // точка просмотра
    scene.rotation = Vec(M_PI / 2, 0, 0); // без вращения по x

    scene.width = 1024; // ширина изображения
    scene.height = 768; // высота изображения
    scene.depth = 2; // три итерации луча

    scene.envmap = nullptr; // нет карты окружения

    return scene;
}

void SceneReader::ReadParams(Scene &scene, const std::string& name, std::stringstream &ss) {
    if (name == "background") {
        ss >> scene.backgroundColor; // обновляем цвет фона
        scene.backgroundColor = scene.backgroundColor / 255;
    }
    else if (name == "position") {
        ss >> scene.position; // обновляем цвет фона
    }
    else if (name == "rotation") {
        ss >> scene.rotation; // обновляем углы поворота
        scene.rotation = scene.rotation * M_PI / 180;
    }
    else if (name == "width") {
        ss >> scene.width; // считываем ширину
    }
    else if (name == "height") {
        ss >> scene.height; // считываем высоту
    }
    else if (name == "depth") {
        ss >> scene.depth; // считываем глубину трассировки
    }
    else if (name == "envmap") {
        std::string filepath;
        ss >> filepath;
        scene.envmap = new Picture(filepath);
    }
}

// считывание света
void SceneReader::ReadLight(Scene &scene, std::stringstream& ss) {
    std::string type; // тип
    ss >> type;

    if (type == "ambient") {
        scene.lights.push_back(new AmbientLight(ss));
    }
    else if (type == "point") {
        scene.lights.push_back(new PointLight(ss));
    }
    else if (type == "directional") {
        scene.lights.push_back(new DirectionalLight(ss));
    }
    else if (type == "spot") {
        scene.lights.push_back(new SpotLight(ss));
    }
    else
        throw std::runtime_error("unknown light type '" + type + "'");
}

// считывание примитива
void SceneReader::ReadPrimitive(Scene &scene, const std::string& type, std::stringstream& ss) {
    std::string name;
    ss >> name; // считываем название материала

    if (scene.materials.find(name) == scene.materials.end())
        throw std::runtime_error("unknown material '" + name + "'");

    if (type == "sphere") {
        scene.primitives.push_back(new Sphere(ss, scene.materials[name]));
    }
    else if (type == "textured_sphere") {
        scene.primitives.push_back(new TexturedSphere(ss, scene.materials[name]));
    }
    else if (type == "triangle") {
        scene.primitives.push_back(new Triangle(ss, scene.materials[name]));
    }
    else if (type == "infinityflatness") {
        scene.primitives.push_back(new InfinityFlatness(ss, scene.materials[name]));
    }
    else if (type == "flatness") {
        scene.primitives.push_back(new Flatness(ss, scene.materials[name]));
    }
    else if (type == "chessflatness") {
        std::string name2;
        ss >> name2;
        scene.primitives.push_back(new ChessFlatness(ss, scene.materials[name], scene.materials[name2]));
    }
    else if (type == "disk") {
        scene.primitives.push_back(new Disk(ss, scene.materials[name]));
    }
    else if (type == "mandelbulb") {
        scene.primitives.push_back(new Mandelbulb(ss, scene.materials[name]));
    }
    else if (type == "torus") {
        scene.primitives.push_back(new Torus(ss, scene.materials[name]));
    }
    else if (type == "box") {
        scene.primitives.push_back(new Box(ss, scene.materials[name]));
    }
    else if (type == "model") {
        scene.primitives.push_back(new ObjModel(ss, scene.materials[name]));
    }
    else
        throw std::runtime_error("unknown object '" + type + "'");
}

Scene SceneReader::Read(const std::string& path) {
    std::ifstream f(path.c_str());

    if (!f)
        throw std::runtime_error("invalid scene file");

    std::string line; // строка файла
    Scene scene = InitScene(); // создаём сцену

    while (getline(f, line)) {
        std::stringstream ss(line); // создаём поток

        if (line == "" || line[0] == '#' || line == "\r")
            continue; // игнорируем пустые строки и комментарии

        std::string object; // объект сцены (фон/свет/примитив)
        ss >> object; // считываем объект

        if (object == "background" || object == "position" || object == "rotation" || object == "width" || object == "height" || object == "depth" || object == "envmap") {
            ReadParams(scene, object, ss); // считываем параметры
        }
        else if (object == "material") {
            std::string name;
            ss >> name;
            scene.materials[name] = Material(ss); // считываем материал
        }
        else if (object == "light") {
            ReadLight(scene, ss); // добавляем источник света
        }
        else {    
            ReadPrimitive(scene, object, ss); // считываем примитив
        }
    }

    f.close(); // закрываем файл
    return scene; // возвращаем сцену
}