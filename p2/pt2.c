#include <mpi.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>

int main(int argc, char** argv) {
  struct timeval ts;
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
  int i;
  short int number1=1;
  printf("Size of number1: %d\n", sizeof(number1));
  int number2;

  for (i=0; i<31; i++){
    long int timesToSend = pow ( 2, i);
    printf("Sending number size %d %ld times.\n", sizeof(number1), timesToSend);
    int j;
    long int rttSum=0;
    double rttAvg;
    // for (j=0; j<1000; j++){
      // int k;
      // long int totalDataSent=0;
      for (k=0; k< timesToSend; k++){
        int time1;
        int time2;
        int rtt;
        if(world_rank==0){
          //printf("Hello I'm processor %s with rank %d\n", processor_name, world_rank);
          gettimeofday(&ts, NULL);
          time1=ts.tv_usec;
          MPI_Send(&number1, sizeof(number1), MPI_INT, 1, 0, MPI_COMM_WORLD);
          MPI_Recv(&number1, sizeof(number1), MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
          gettimeofday(&ts, NULL);
          time2= ts.tv_usec;
          rtt = time2-time1;
          rttSum= rttSum + rtt;
          totalDataSent += sizeof(number1);
          //printf("Received %d from node1\tRTT: %d microseconds.\n",number1,rtt);
        }

        if (world_rank ==1){

          MPI_Recv(&number2, sizeof(number2), MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
          //printf("Received %d from node0\n",number2);
          MPI_Send(&number2, sizeof(number2), MPI_INT, 0, 0, MPI_COMM_WORLD);
        }
      }
      // if(j==0){
        // printf("Total data sent %ld\n", totalDataSent);
      // }
    }

    rttAvg = rttSum/ 1000.0;
    printf("Sent %ld 1000x. Average RTT = %d.\n", timesToSend*2, rttAvg);

  }

  // Finalize the MPI environment. No more MPI calls can be made after this
  MPI_Finalize();
}
//
