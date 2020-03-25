#include <iostream>
#include <sstream>
#include <string>
#include <cstdint>
#include <chrono>
#include <cmath>

#include "Utils/ArgumentsParser.hpp"
#include "RayTracer.hpp"

typedef std::chrono::high_resolution_clock Time;
typedef std::chrono::time_point<Time> TimePoint; 
typedef std::chrono::milliseconds ms;

// получение камеры
void SetCamera(Camera &camera, ArgumentsParser &parser, Vec position, Vec rotation) {
    if (!parser.IsSet("-position")) {
        camera.SetPosition(position);
    }

    if (!parser.IsSet("-rotation")) {
        camera.SetRotation(rotation);
    }
}

int main(int argc, const char** argv) {
    ArgumentsParser parser;
    parser.AddArgument("-o", "filename of output picture", "out.bmp");
    parser.AddArgument("-scene", "number of scene or path to scene file", "1");
    parser.AddArgument("-size", "size of output picture", "1024 768");
    parser.AddArgument("-depth", "max depth of tracing", "2");
    parser.AddArgument("-position", "position of camera", "0 0 0");
    parser.AddArgument("-rotation", "rotations of camera by OX and OY in degrees", "90 0");
    parser.AddArgument("-antialiasing", "antialiasing mode, 1, 4 or 9 rays per pixel", "1");

    // если нет аргументов или один аргумент --help
    if (argc == 1 || (argc == 2 && std::string(argv[1]) == "--help")) {
        parser.Help(); // выводим сообщение о помощи
        return 0; // и выходим
    }

    // пытаемся распарсить аргументы
    if (!parser.ParseArguments(argc, argv))
        return 0; // выходим, если не получилось

    RayTracer tracer; // создаём трассировщик лучей

    int width, height; // размеры изображения
    std::stringstream ss(parser.Get("-size"));
    ss >> width >> height;

    Vec position = parser.GetVec("-position"); // получаем положение камеры
    Vec rotation = parser.GetVec("-rotation") * (M_PI / 180); // переводим в радианы
    Camera camera(position, rotation); // создаём камеру
    
    std::string scene = parser.Get("-scene"); // номер сцены
    int depth = parser.GetInteger("-depth"); // максимальная глубина трассировки
    int antialiasing = parser.GetInteger("-antialiasing"); // режим сглаживания

    if (scene == "1") {
        SetCamera(camera, parser, Vec(0, 2, -3), Vec(M_PI / 2.25, 0, 0));
        tracer.ReadScene("Scenes/scene1.txt");
    }
    else if (scene == "2") {
        if (!parser.IsSet("-depth"))
            depth = 3;

        SetCamera(camera, parser, Vec(0, 6.5, -7), Vec(M_PI / 2.25, 0, 0));
        tracer.ReadScene("Scenes/scene2.txt");
    }
    else if (scene == "3") {
        SetCamera(camera, parser, Vec(0, 4, -7.99), Vec(M_PI / 2.57, 0, 0));

        if (!parser.IsSet("-depth"))
            depth = 4;

        tracer.ReadScene("Scenes/scene3.txt");
    }
    else {
        tracer.ReadScene(scene);    
    }

    TimePoint t0 = Time::now();
    Picture picture = tracer.CastRays(width, height, camera, depth, antialiasing); // трассируем лучи
    TimePoint t1 = Time::now();
    picture.Save(parser.Get("-o")); // сохраняем изображение

    ms d = std::chrono::duration_cast<ms>(t1 - t0);
    std::cout << "end by " << d.count() << " ms" << std::endl;
    return 0;
}