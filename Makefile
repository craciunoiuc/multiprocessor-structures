CC=gcc
MPICC=mpicc
OPENMP=-fopenmp
FLAGS=-O0 -Wall -Wextra -Wno-unknown-pragmas -Wno-unused-result

build: omp.exe thread.exe mpi.exe hibrid.exe

omp.exe: lodepng.o image_filtering_omp.c image_IO.c
	$(CC) $(FLAGS) $(OPENMP) $^ -o $@

thread.exe: lodepng.o image_IO.c
	$(CC) $(FLAGS) $^ -o $@

mpi.exe: lodepng.o image_IO.c
	$(MPICC) $(FLAGS) $^ -o $@

hibrid.exe: lodepng.o image_IO.c
	$(MPICC) $(FLAGS) $(OPENMP) $^ -o $@

lodepng.o: lode/lodepng.c image_IO.c
	$(CC) $(FLAGS) $^ -c

clean:
	rm -f *.o lode/*.o omp.exe thread.exe mpi.exe hibrid.exe output/*