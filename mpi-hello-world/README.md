EE 542_4
Lab Assignment #4 - Part 1
Amjad Aryan and Everett Tsang
-----------------------
In the tutorial, 32 tasks are allocated in the batch job and each task will print out Hello Worold along with it's rank in the MPI system.

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
