#include <mpi.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
int main(int argc, char** argv) {
  // Initialize the MPI environment. The two arguments to MPI Init are not
  // currently used by MPI implementations, but are there in case future
  // implementations might need the arguments.
  MPI_Init(NULL, NULL);

  // Get the number of processes
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  // Get the rank of the process
  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  // Get the name of the processor
  char processor_name[MPI_MAX_PROCESSOR_NAME];
  int name_len;
  MPI_Get_processor_name(processor_name, &name_len);

  struct timeval start;
  struct timeval end;
  struct ifreq ifr;
  int maxSize = pow(2, 30);
  int *data = malloc(maxSize); // Allocate 1GB of ints to send
  int bufferSize = sizeof(int);
  int numElements;
  int i;
  int fd;
  long int rttSum;
  double rttAvg;
  double throughput;
  char ipv4[256];
  char command1[256];
  char command2[256];
  
  if (world_rank == 0){
    fd=socket(AF_INET,SOCK_DGRAM,0);
    ifr.ifr_addr.sa_family=AF_INET;
    strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);
    ioctl(fd, SIOCGIFADDR, &ifr);
    close(fd);

    strcpy(ipv4, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
    printf("My ip address: %s\n", ipv4);
    
    MPI_Send(ipv4, sizeof(ipv4), MPI_CHAR, 1, 0, MPI_COMM_WORLD);
    system("ib_read_lat -a");

    MPI_Recv(ipv4, sizeof(ipv4), MPI_CHAR, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    system("ib_read_bw -a");

    while(bufferSize <= maxSize){
      rttSum = 0;
      numElements = bufferSize / sizeof(int);

      for(i = 0; i < 1000; i++){
        gettimeofday(&start, NULL);

        MPI_Send(data, numElements, MPI_INT, 1, 0, MPI_COMM_WORLD);
        MPI_Recv(data, numElements, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        gettimeofday(&end, NULL);

        rttSum += (end.tv_sec*1e6 + end.tv_usec) - (start.tv_sec*1e6 + start.tv_usec);
      }

      rttAvg = (double) rttSum / 2000;
 
      printf("Buffer Size: %d Bytes\n", bufferSize);
      printf("Average Round Trip Time (Latency): %f usec\n", rttAvg);
      printf("Throughput: %f MBps\n\n", (double) bufferSize / rttAvg);

      bufferSize = bufferSize * 2;
    }
  }

  if (world_rank == 1){
    usleep(30000);
    MPI_Recv(ipv4, sizeof(ipv4), MPI_CHAR, 0,0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    
    strcpy(command1, "ib_read_lat ");
    strcat(command1, ipv4);
    strcat(command1, " -a -F");

    strcpy(command2, "ib_read_bw ");
    strcat(command2, ipv4);
    strcat(command2, " -a -F");

    system(command1);

    MPI_Send(ipv4, sizeof(ipv4), MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    usleep(30000);
    system(command2);

    while(bufferSize <= maxSize){
      numElements = bufferSize / sizeof(int);

      for(i = 0; i < 1000; i++){
        MPI_Recv(data, numElements, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Send(data, numElements, MPI_INT, 0, 0, MPI_COMM_WORLD);
      }

      bufferSize = bufferSize * 2;
    }
  }

  // Finalize the MPI environment. No more MPI calls can be made after this
  MPI_Finalize();
}
