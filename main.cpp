#include <iostream>
#include <sstream>
#include <string>
#include <cstdint>
#include <chrono>
#include <cmath>

#include "Utils/ArgumentsParser.hpp"
#include "Utils/Picture.hpp"
#include "Utils/SceneReader.hpp"
#include "RayTracer.hpp"

typedef std::chrono::high_resolution_clock Time;
typedef std::chrono::time_point<Time> TimePoint; 
typedef std::chrono::milliseconds ms;

// обновление параметров сцены
void ChangeScene(Scene &scene, ArgumentsParser &parser) {
    if (parser.IsSet("-size")) {
        std::stringstream ss(parser.Get("-size"));
        ss >> scene.width >> scene.height;
    }

    // максимальная глубина трассировки
    if (parser.IsSet("-depth")) {
        scene.depth = parser.GetInteger("-depth");
    }

    // получаем положение камеры
    if (parser.IsSet("-position")) {
        scene.position = parser.GetVec("-position");
    }

    // получаем повороты камеры
    if (parser.IsSet("-rotation")) {
        scene.rotation = parser.GetVec("-rotation") * (M_PI / 180);
    }
}

int main(int argc, const char** argv) {
    ArgumentsParser parser;
    parser.AddArgument("-output", "Filename of output picture", "out.bmp", "-o");
    parser.AddArgument("-scene", "Path to scene file", "1", "-f");
    parser.AddArgument("-size", "Size of output picture", "1024 768", "-s");
    parser.AddArgument("-depth", "Max depth of tracing", "2", "-d");
    parser.AddArgument("-position", "Position of camera", "0 0 0", "-p");
    parser.AddArgument("-rotation", "Rotations of camera by OX and OY in degrees", "90 0", "-r");
    parser.AddArgument("-antialiasing", "Antialiasing mode, 1, 4 or 9 rays per pixel", "1", "-a");

    // если нет аргументов или один аргумент --help
    if (argc == 1 || (argc == 2 && std::string(argv[1]) == "--help")) {
        parser.Help(); // выводим сообщение о помощи
        return 0; // и выходим
    }

    // пытаемся распарсить аргументы
    if (!parser.ParseArguments(argc, argv))
        return 0; // выходим, если не получилось

    SceneReader reader; // создаём считыватель сцены
    Scene scene = reader.Read(parser.Get("-scene")); // считываем сцену

    ChangeScene(scene, parser); // обновляем параметры, заданые пользователем

    int antialiasing = parser.GetInteger("-antialiasing"); // режим сглаживания

    TimePoint t0 = Time::now();
    RayTracer tracer; // создаём трассировщик лучей
    Picture picture = tracer.Render(scene, antialiasing); // трассируем лучи
    TimePoint t1 = Time::now();
    
    picture.Save(parser.Get("-output")); // сохраняем изображение

    ms d = std::chrono::duration_cast<ms>(t1 - t0);
    std::cout << "end by " << d.count() << " ms" << std::endl;
    return 0;
}