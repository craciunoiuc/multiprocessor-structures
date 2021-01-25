build: next_prime

next_prime: next_prime.c
	mpicc next_prime.c -o next_prime -lm

clean:
	rm -f next_prime

cluster: build cluster_run.sh
	qsub -q ibm-dp.q -cwd ./cluster_run.sh