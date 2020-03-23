#ifndef BOUNDING_BOX_HPP
#define BOUNDING_BOX_HPP

#include <iostream>
#include "Primitive.hpp"

class BoundingBox : public Primitive {
	std::vector<Primitive *> primitives;
	Vec min;
	Vec max;
	Primitive *nearest;
public:
	BoundingBox();
    
    double Intersect(const Ray &ray); // поиск пересечения
    Vec GetNormal(const Vec &point); // нормаль
    Material GetMaterial(const Vec &point); // получение цвета в точке

    void AddPrimitive(Primitive *primitive); // добавление примитива
    void AddBox(BoundingBox *box);
};

BoundingBox::BoundingBox() {
	min = Vec(INF, INF, INF);
	max = Vec(-INF, -INF, -INF);
	nearest = nullptr;
}

// поиск пересечения
double BoundingBox::Intersect(const Ray &ray) {
    double dx = ray.GetInvDirection().GetX();
    double dy = ray.GetInvDirection().GetY();
    double dz = ray.GetInvDirection().GetZ();

	double tmin = (min.GetX() - ray.GetOrigin().GetX()) * dx; 
    double tmax = (max.GetX() - ray.GetOrigin().GetX()) * dx; 
 
    if (tmin > tmax)
    	std::swap(tmin, tmax); 
 
    double tymin = (min.GetY() - ray.GetOrigin().GetY()) * dy; 
    double tymax = (max.GetY() - ray.GetOrigin().GetY()) * dy; 
 
    if (tymin > tymax)
    	std::swap(tymin, tymax); 
 
    if ((tmin > tymax) || (tymin > tmax)) 
        return INF; 
 
    if (tymin > tmin) 
        tmin = tymin; 
 
    if (tymax < tmax) 
        tmax = tymax; 
 
    double tzmin = (min.GetZ() - ray.GetOrigin().GetZ()) * dz; 
    double tzmax = (max.GetZ() - ray.GetOrigin().GetZ()) * dz; 
 
    if (tzmin > tzmax) 
    	std::swap(tzmin, tzmax); 
 
    if ((tmin > tzmax) || (tzmin > tmax)) 
        return INF; 
 
    if (tzmin > tmin) 
        tmin = tzmin; 
 
    if (tzmax < tmax) 
        tmax = tzmax;


    double t = INF;
 	nearest = nullptr;

 	for (size_t i = 0; i < primitives.size(); i++) {
		double ti = primitives[i]->Intersect(ray);

		if (ti < t) {
			t = ti; // обновляем ближайшее расстоние
			nearest = primitives[i];
		}
	}
    
    return t;
}

// нормаль
Vec BoundingBox::GetNormal(const Vec &point) {
	return nearest->GetNormal(point);
}

// получение цвета в точке
Material BoundingBox::GetMaterial(const Vec &point) {
	return nearest->GetMaterial(point);
}

// добавление примитива
void BoundingBox::AddPrimitive(Primitive *primitive) {
	primitives.push_back(primitive);
	primitive->UpdateBbox(min, max);
}

void BoundingBox::AddBox(BoundingBox *primitive) {
    primitives.push_back(primitive);
    this->min = this->min.Min(primitive->min);
    this->max = this->max.Max(primitive->max);
}

#endif