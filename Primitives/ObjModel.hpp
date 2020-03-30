#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

#include "../Geometry/Vec.hpp"
#include "Triangle.hpp"
#include "BoundingBox.hpp"

class ObjModel : public Primitive {
    struct FaceNode {
        int v; // индекс вершины
        int vt; // индекс текстурной координаты
        int vn; // индекс нормали
    };

    typedef std::vector<FaceNode> Face;

    std::vector<Vec> v; // вершины
    std::vector<Vec> vt; // текстурные координаты
    std::vector<Vec> vn; // нормали

    Vec center;
    std::vector<Triangle*> triangles;
    BoundingBox *box;
    int boxIndex;

    bool IsComment(const std::string &line); // проверка строки на комментарий
    FaceNode ParseVertex(const std::string& vertex); // парсинг вершини грани
    void ParseFace(std::stringstream &ss); // парсинг грани

    void ToTriangles(); // получение треугольников
    BoundingBox* Split(std::vector<Triangle *> &triangles, int level = 0);

public:
    ObjModel(std::istream &is, const Material& material); // создание модельки из файла

    Primitive* Intersect(const Ray &ray, double tmin, double tmax, double &t); // поиск пересечения
    Vec GetNormal(const Vec &point); // нормаль
    Material GetMaterial(const Vec &point); // нормаль
};

// проверка строки на комментарий
bool ObjModel::IsComment(const std::string &line) {
    return line.length() == 0 || line[0] == '#';
}

// парсинг вершини грани
ObjModel::FaceNode ObjModel::ParseVertex(const std::string& vertex) {
    std::stringstream ss(vertex);
    std::vector<std::string> elements;
    std::string elem;

    while (getline(ss, elem, '/'))
        elements.push_back(elem);

    FaceNode node;

    node.v = std::stoi(elements[0]);

    if (elements.size() == 2)
        node.vt = std::stoi(elements[1]);
    
    if (elements.size() == 3) {
        node.vt = elements[1] == "" ? -1 : std::stoi(elements[1]);
        node.vn = std::stoi(elements[2]);
    }
    
    return node;
}

// парсинг грани
void ObjModel::ParseFace(std::stringstream &ss) {
    std::string vertex; // информация об одной вершине
    Face face;

    while (ss >> vertex) 
        face.push_back(ParseVertex(vertex));

    for (size_t j = 2; j < face.size(); j++) {
        Vec v1 = v[face[0].v - 1];
        Vec v2 = v[face[j - 1].v - 1];
        Vec v3 = v[face[j].v - 1];

        triangles.push_back(new Triangle(v1, v2, v3, material));
    }
}

BoundingBox* ObjModel::Split(std::vector<Triangle *> &triangles, int level) {
    BoundingBox *box = new BoundingBox();

    if (triangles.size() < 8) {
        for (size_t i = 0; i < triangles.size(); i++)
            box->AddPrimitive(triangles[i]);

        return box;
    }

    Vec bboxMin(INF, INF, INF);
    Vec bboxMax(-INF, -INF, -INF);

    for (size_t i = 0; i < triangles.size(); i++)
        triangles[i]->UpdateBbox(bboxMin, bboxMax);

    Vec center = (bboxMin + bboxMax) * 0.5;

    std::vector<Triangle *> set1;
    std::vector<Triangle *> set2;

    int mode = level % 3;

    for (size_t i = triangles.size() - 1; i + 1 > 0; i--) {
        Vec min(INF, INF, INF);
        Vec max(-INF, -INF, -INF);
        triangles[i]->UpdateBbox(min, max);

        Vec delta = ((min + max) * 0.5 - center);

        if ((mode == 0 && delta.y < 0) || (mode == 1 && delta.x < 0) || (mode == 2 && delta.z < 0)) {
            set1.push_back(triangles[i]);
        }
        else {
            set2.push_back(triangles[i]);
        }

        triangles.pop_back();
    }

    if (set1.size() == 0 || set2.size() == 0) {
        for (size_t i = 0; i < set1.size(); i++)
            box->AddPrimitive(set1[i]);

        for (size_t i = 0; i < set2.size(); i++)
            box->AddPrimitive(set2[i]);

        return box;
    }

    box->AddBox(Split(set1, level + 1));
    box->AddBox(Split(set2, level + 1));
    return box;
}

// создание модельки из файла
ObjModel::ObjModel(std::istream &is, const Material& material) {
    std::string path;
    double scale;
    is >> path >> center >> scale;
    this->material = material;

    std::ifstream modelFile(path);

    if (!modelFile)
        throw std::runtime_error("invalid model");

    std::string line;

    while (std::getline(modelFile, line)) {
        if (IsComment(line))
            continue;

        std::stringstream ss(line);
        std::string type;
        ss >> type;
        
        if (type == "v") {
            v.push_back(Vec(ss) * scale + center);
        } else if (type == "vt") {
            vt.push_back(Vec(ss));
        } else if (type == "vn") {
            vn.push_back(Vec(ss));
        } else if (type == "f") {
            ParseFace(ss);
        }
    }

    modelFile.close();

    box = Split(triangles);
}

// поиск пересечения
Primitive* ObjModel::Intersect(const Ray &ray, double tmin, double tmax, double &t) {
    return box->Intersect(ray, tmin, tmax, t);
}
   
// нормаль
Vec ObjModel::GetNormal(const Vec &point) { 
    return box->GetNormal(point);
}

Material ObjModel::GetMaterial(const Vec &point) {
    return box->GetMaterial(point);
}
