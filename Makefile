CC=gcc
MPICC=mpicc
OPENMP=-fopenmp
FLAGS=-O0 -Wall -Wextra -Wno-unknown-pragmas -Wno-unused-result

build: omp.exe thread.exe mpi.exe hibrid.exe

omp.exe: lodepng.o image_filtering_omp.c image_IO_omp.c
	$(CC) $(FLAGS) $(OPENMP) $^ -o $@

thread.exe: lodepng.o image_IO_threads.c image_filtering_threads.c
	$(CC) $(FLAGS) $^ -o $@ -lpthread

mpi.exe: lodepng.o image_IO_MPI.c image_filtering_mpi.c
	$(MPICC) $(FLAGS) $^ -o $@

hibrid.exe: lodepng.o image_IO_omp.c image_filtering_hibrid.c
	$(MPICC) $(FLAGS) $(OPENMP) $^ -o $@

lodepng.o: lode/lodepng.c
	$(CC) $(FLAGS) $^ -c

clean:
	rm -f *.o lode/*.o omp.exe thread.exe mpi.exe hibrid.exe output/*