/*
 * File:     lab1part1.c
 *
 * Purpose:  Estimate pi using a monte carlo method
 *  
 * Compile:  mpicc ...
 * Run:      mpiexec ...
 *
 * Input:    Number of "dart tosses"
 * Output:   Estimate of pi.  If DEBUG flag is set, results of processes' tosses.
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

void Get_input(long long int* number_of_tosses, int my_rank, int comm_sz, MPI_Comm comm);
long long int Monte_carlo(long long number_of_tosses, int my_rank);

/*-------------------------------------------------------------------*/
int main(void) {
   long long int number_of_tosses;
   long long int local_number_of_tosses;
   long long int number_in_circle;
   long long int local_number_in_circle;
   
   double pi_estimate;
   int my_rank, comm_sz;
   MPI_Comm comm;
   
   MPI_Init(NULL, NULL);
   comm = MPI_COMM_WORLD;
   MPI_Comm_size(comm, &comm_sz);
   MPI_Comm_rank(comm, &my_rank);
   
   Get_input(&number_of_tosses, my_rank, comm_sz, comm);
   local_number_of_tosses = number_of_tosses/comm_sz;

#  ifdef DEBUG
   printf("Proc %d > number of tosses = %lld, local number = %lld\n", my_rank, number_of_tosses, local_number_of_tosses);
#  endif

   local_number_in_circle = Monte_carlo(local_number_of_tosses, my_rank);
   
   /* Compute global sum of local_number_in_circle and store total in variable number_in_circle in process 0*/
   
   if ( my_rank == 0 ){
      number_in_circle += local_number_in_circle;
      for ( int i = 1; i < comm_sz; i++)
      {
         MPI_Recv((void*)&local_number_of_tosses, 1, MPI_LONG_LONG_INT, i, MPI_ANY_TAG, comm, NULL);
         number_in_circle += local_number_in_circle;
      }
      pi_estimate = 4*number_in_circle/((double)number_of_tosses);
      printf("pi estimate = %f\n", pi_estimate);
   }
   else
   {
      MPI_Send((void*)&local_number_of_tosses, 1, MPI_LONG_LONG_INT, 0, 0, comm);
   }
   
   MPI_Finalize();
   return 0;
}  /* main */


/*-------------------------------------------------------------------*/
void Get_input(
      long long int* number_of_tosses  /* out */,
      int my_rank                      /* in  */, 
      int comm_sz                      /* in  */,
      MPI_Comm comm                    /* in  */) {
      
   if (my_rank == 0 ) {
      printf("Enter the total number of tosses\n");
      scanf("%lld", number_of_tosses);

      for (int i = 1; i < comm_sz; i++)
          MPI_Send((void*)number_of_tosses, 1, MPI_LONG_LONG_INT, i, 0, comm);
   }
   else
   {
      /* Broadcast number_of_tosses to all communicators */
      MPI_Recv((void*)number_of_tosses, 1, MPI_LONG_LONG_INT, 0, MPI_ANY_TAG, comm, NULL);
   }
}  /* Get_input */


/*-------------------------------------------------------------------*/
long long int Monte_carlo(long long local_number_of_tosses, int my_rank) {
   long long int i;
   double x,y;
   double distance_squared;
   long long int number_in_circle = 0;
      
   srandom(my_rank+1);
   for ( i=0 ; i< local_number_of_tosses ; i++) {
      x = ((float)rand()/(float)RAND_MAX)*2 -1;
      y = ((float)rand()/(float)RAND_MAX)*2 -1;
      
      distance_squared = x*x+y*y;
#     ifdef DEBUG
      printf("Proc %d > x = %f, y = %f, dist squared = %f\n", my_rank, x, y, distance_squared);
#     endif
      /* if dart falls in unit circle, increment the count number_in_circle */
      if (distance_squared <=1)
          number_in_circle++;
   }
   
   return number_in_circle;
}  /* Monte_carlo */

