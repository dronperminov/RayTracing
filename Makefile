COMPILER=g++
FLAGS=-O3 -fopenmp

all:
	$(COMPILER) $(FLAGS) main.cpp -o rt