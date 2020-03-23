# Raytracing
Implementation of raytracing algorithms with different primitives

## Примитивы:
* сфера
* треугольник
* диск
* бесконечная плоскость
* конечная плоскость
* шахматная плоскость

Каждая из сцен считывается из файла. Сцена - текстовый документ в следующем формате:
* ### материалы:
  ```material [name] [color] [s] [eta] [diffuse] [specular] [reflection] [refraction]```
* ### цвет фона:
  ```background [red green blue]```

* ### источники освещения:
	- ```light ambient [energy]```
	- ```light point [energy] [position]```
	- ```light direction [energy] [direction]```

* ### объекты:
	- сфера: ```sphere [material] [center] [radius]```
	- треугольник: ```triangle [material] [v1] [v2] [v3]```
	- бесконечная плоскость: ```infinityflatness [material] [normal] [bias]```
	- конечная плоскость: ```flatness [material] [normal] [center] [size]```
	- шахматная плоскость: ```chessflatness [material] [normal] [center] [size] [cells count]```
	- диск: ```disk [normal] [center] [radius]```

Программа позволяет трассировать как три основные сцены, так и любые другие из файлов (для этого в качестве параметра scene должно быть задано путь к файлу со сценой)