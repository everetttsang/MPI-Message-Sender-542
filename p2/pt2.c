#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
  // Initialize the MPI environment. The two arguments to MPI Init are not
  //   // currently used by MPI implementations, but are there in case future
  //     // implementations might need the arguments.
  MPI_Init(NULL, NULL);
  //
  //         // Get the number of processes
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  //
  // Get the rank of the process
  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  //
  // Get the name of the processor
  char processor_name[MPI_MAX_PROCESSOR_NAME];
  int name_len;
  MPI_Get_processor_name(processor_name, &name_len);
  //
  //
  int number1=42;
  if(world_rank==0){
    printf("Hello I'm processor %s with rank %d\n", processor_name, world_rank);
    MPI_SEND(&number1, 2, MPI_INT, 1, 0, MPI_COMM_WORLD);
  }

  if (world_rank ==1){
    printf("Hello I'm processor %s with rank %d\n",processor_name, world_rank);
    MPI_Recv(&number2, 2, MP_INT, 0, 0, MPI_COM_WORLD,, MPI_STATUS_IGNORE);
    printf("Received %d from node0\n",number2);
  }

  if(world_rank ==2){
    printf("Hello I'm processor %s with rank %d\n", processor_name, world_rank);
  }
  //
  //                                       // Finalize the MPI environment. No more MPI calls can be made after this
  MPI_Finalize();
}
//
