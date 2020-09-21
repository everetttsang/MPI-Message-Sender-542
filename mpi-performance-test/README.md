EE 542_4
Lab Assignment #4 - Parts 2 and 3
Amjad Aryan and Everett Tsang
-----------------------
In this tutorial, 2 tasks are allocated in the batch job and the following tests are performed:
1. Throughput testing (Latency and BW) using the OpenMPI library calls to send an array of ints in increments of up to 1 GB.
2. Mellanox benchmark utility ib_read_bw to measure bandwidth in increments of up to 8 MB of data.
3. Mellanox benchmark utility ib_read_lat to measure latency in increments of up to 8 MB of data.

Compilation Instructions
1. Before compiling, execute the following command
    module purge
    module load gcc/8.3.0
    module load openmpi/4.0.2
    module load pmix
2. To compile, execute the command: make
3. To clean the directory, execute the command: make clean

Run Instructions
1. Execute the command: sbatch --exlusive run.job
NOTE: The --exclusive flag is optional and reserves a node completely for this task
2. Observe the output at the "slurm-{job_id}.out" file.
