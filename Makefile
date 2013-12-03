OBJ = main.o Camera.o Terrain.o GeomTools.o VerletSystem.o Vehicle.o
LIBS = -lglfw -lglew32 -lopengl32 -lglu32 -lglut32

all: run

run: $(OBJ)
	g++ $(OBJ) -o run $(LIBS)

main.o: main.cpp VerletSystem.h ObjLoader.h Camera.h Terrain.h GeomTools.h Vehicle.h
	g++ -c main.cpp -o main.o $(LIBS)

VerletSystem.o: VerletSystem.h VerletSystem.cpp
	g++ -c VerletSystem.cpp -o VerletSystem.o ${LIBS}

Camera.o: Camera.cpp Camera.h aabb.h
	g++ -c Camera.cpp -o Camera.o $(LIBS)

Terrain.o: Terrain.cpp Terrain.h
	g++ -c Terrain.cpp -o Terrain.o ${LIBS}

GeomTools.o: GeomTools.cpp GeomTools.h
	g++ -c GeomTools.cpp -o GeomTools.o ${LIBS}

Vehicle.o: Vehicle.cpp Vehicle.h
	g++ -c Vehicle.cpp -o Vehicle.o ${LIBS}

execute: run
	./run

clean:
	rm *.o