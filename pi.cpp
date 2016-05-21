#include "mpi.h"
#include <time.h>
#include <math.h>

long NBR_SAMPLES = 1000000;

void    master(int world_size) {
  double pi;
  long count, nbr_points_on_circle = 0;

  for (long i = 0; i < NBR_SAMPLES; i++) {
    int sign = rand() % 2;
    double x = (double)(rand() % 1001) / 1000;

    if (sign == 0)
      x *= -1;

    sign = rand() % 2;
    double y = (double)(rand() % 1001) / 1000;
    if (sign == 0)
      y *= -1;

    if (sqrt(x * x + y * y) <= 1)
      nbr_points_on_circle++;
  }

  count = nbr_points_on_circle;
  for (int proc = 1; proc < world_size; proc++) {
    MPI_Recv(&nbr_points_on_circle, 1,
	     MPI_LONG, proc, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    count += nbr_points_on_circle;
  }

  pi = (double) count / (NBR_SAMPLES * world_size) * 4;

  std::cout << std::endl << "Total number of samples : \e[32m"
	    << NBR_SAMPLES * world_size << std::endl
	    << "\e[0mEstimated value of π is : \e[36m"
	    << pi << "\e[0m" << std::endl;
}

void    slave(int world_rank) {
  long nbr_points_on_circle = 0;

  for (long i = 0; i < NBR_SAMPLES; i++) {

    int sign = rand() % 2;
    double x = (double)(rand() % 1000001) / 1000000;

    if (sign == 0)
      x *= -1;

    sign = rand() % 2;
    double y = (double)(rand() % 1000001) / 1000000;
    if (sign == 0)
      y *= -1;

    if (sqrt(x * x + y * y) <= 1)
      nbr_points_on_circle++;
  }

  std::cout << "Slave \e[33m" << world_rank
	    << "\e[0m is sending the results to the master"
	    << " process : \e[33m"
	    << nbr_points_on_circle << "\e[0m" << std::endl;

  MPI_Send(&nbr_points_on_circle, 1, MPI_LONG,
	   0, 0, MPI_COMM_WORLD);
}

int     main(int argc, char **argv) {
  int world_size, world_rank;

  if (argc == 2)
    NBR_SAMPLES = atol(argv[1]);
                
  MPI_Init(&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  if (world_rank == 0) {
    srand(time(NULL));
    master(world_size);
  } else {
    srand(time(NULL) % world_rank * 1000 + 1);
    slave(world_rank);
  }

  MPI_Finalize();
  return 0;
}
