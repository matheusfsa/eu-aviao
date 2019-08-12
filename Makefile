cenariomake: cenario.cpp
	g++ -o cenario Geometry.cpp Bmp.cpp cenario.cpp -lGL -lGLU -lglut
