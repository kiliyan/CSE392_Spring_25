/****************************************************************
 ****
 **** This program file is part of the book 
 **** `Parallel programming for Science and Engineering'
 **** by Victor Eijkhout, eijkhout@tacc.utexas.edu
 ****
 **** copyright Victor Eijkhout 2012-6
 ****
 **** MPI Exercise
 ****
 ****************************************************************/

#include <iostream>
#include <mpi.h>
using namespace std;



int main(int argc,char **argv) {

/**** your code here ****/
//std::cout << "before\n";
MPI_Init(0,0);
MPI_Comm comm=MPI_COMM_WORLD;
int nprocs,procno;
MPI_Comm_size(comm,&nprocs);
MPI_Comm_rank(comm,&procno);
if(procno ==0) { 
std::cout << "size from the process number "<<procno<<" is = "<<nprocs <<"\n";
}
MPI_Finalize();	 
//cout<< "after\n";
  return 0;
}
