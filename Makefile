cenariomake: cenario.cpp
	g++ -o cenario Geometry.cpp Bmp.cpp cenario.cpp  -lm -lpthread -lX11 -lGL -lGLU -lglut
