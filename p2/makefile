EXECS=pt2
MPICC?=mpicc

all: ${EXECS}

pt2: pt2.c
	${MPICC} -o pt2 pt2.c -lm

clean:
	rm -f ${EXECS}
