cenariomake: cenario.cpp
	g++ -o cenario Sphere.cpp Bmp.cpp cenario.cpp -lGL -lGLU -lglut
