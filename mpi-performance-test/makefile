EXECS=mpi_performance_test
MPICC?=mpicc

all: ${EXECS}

mpi_performance_test: mpi_performance_test.c
	${MPICC} -o mpi_performance_test mpi_performance_test.c -lm

clean:
	rm -f ${EXECS}
