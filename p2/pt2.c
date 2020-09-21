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
  int *data = malloc(268435456 * sizeof(int)); // Allocate 1GB of ints to send
  int maxSize = pow(2, 30);
  int byteSize = sizeof(int);
  int i;
  int numElements;
  long int rttSum;
  double rttAvg;
  double throughput;
  

  if (world_rank == 0){
    //system("ifconfig");
    int fd;
    char ipv4[256];
    char command[256];
    struct ifreq ifr;
    fd=socket(AF_INET,SOCK_DGRAM,0);
    ifr.ifr_addr.sa_family=AF_INET;
    strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);
    ioctl(fd, SIOCGIFADDR, &ifr);
    close(fd);

    printf("My ip address: %s\n", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
    strcpy(ipv4, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

    MPI_Send(ipv4, sizeof(ipv4), MPI_CHAR, 1, 0, MPI_COMM_WORLD);
    system("ib_read_lat -a");
    MPI_Recv(ipv4, sizeof(ipv4), MPI_CHAR, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    system("ib_read_bw -a");
    while(byteSize <= maxSize){
      rttSum = 0;
      numElements = byteSize / sizeof(int);

      for(i = 0; i < 1000; i++){
        gettimeofday(&start, NULL);

        MPI_Send(data, numElements, MPI_INT, 1, 0, MPI_COMM_WORLD);
        MPI_Recv(data, numElements, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        gettimeofday(&end, NULL);

        rttSum += (end.tv_sec*1e6 + end.tv_usec) - (start.tv_sec*1e6 + start.tv_usec);
      }

      rttAvg = (double) rttSum / 2000;

     
      printf("Buffer Size: %d\n", byteSize);
      printf("Number of Int Elements: %d\n", numElements);
      printf("Average Round Trip Time (Latency): %f\n", rttAvg);
      throughput = byteSize / rttAvg; //MBps
      printf("Throughput: %f\n", throughput);
      byteSize = byteSize * 2;
    }
  }

  if (world_rank == 1){
    char ipv4[256];
    char command1[256];
    char command2[256];
    strcpy(command1, "ib_read_lat ");
    strcpy(command2, "ib_read_bw ");
    printf("Command2 %s\n", command2);
    usleep(3000);
    MPI_Recv(ipv4, sizeof(ipv4), MPI_CHAR, 0,0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    printf("IPV4: %s\n", ipv4);
    strcat(command1, ipv4);
    strcat(command1, " -a -F");
    printf("Changed ipv4 %s\n", ipv4);
    strcat(command2, ipv4);
    printf("Command2 %s\n", command2);
    strcat(command2, " -a -F");
    printf("Command2 %s\n", command2);
    //command1[sizeof(command1)]='\0';
    //command2[sizeof(command2)]='\0';
    printf("Command1 %s\n", command1);
    printf("Command2 %s\n", command2);
    system(command1);
    //block send
    MPI_Send(ipv4, sizeof(ipv4), MPI_CHAR, 0,0, MPI_COMM_WORLD);
    usleep(30000);
    system(command2);
    while(byteSize <= maxSize){
      numElements = byteSize / sizeof(MPI_INT);

      for(i = 0; i < 1000; i++){
        MPI_Recv(data, numElements, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Send(data, numElements, MPI_INT, 0, 0, MPI_COMM_WORLD);
      }

      byteSize = byteSize * 2;
    }
  }

  // Finalize the MPI environment. No more MPI calls can be made after this
  MPI_Finalize();
}
