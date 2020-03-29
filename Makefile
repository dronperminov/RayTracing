COMPILER=g++
FLAGS=-O3 -fopenmp

all:
	$(COMPILER) $(FLAGS) main.cpp -o rt

example:
	rt -scene Scenes/scene1.txt -output Examples/scene1.bmp
	rt -scene Scenes/scene2.txt -output Examples/scene2.bmp
	rt -scene Scenes/scene3.txt -output Examples/scene3.bmp
	rt -scene Scenes/duck.txt -output Examples/duck.bmp
	rt -scene Scenes/fractal.txt -output Examples/fractal.bmp
	rt -scene Scenes/spheres.txt -output Examples/spheres.bmp
	rt -scene Scenes/torus.txt -output Examples/torus.bmp