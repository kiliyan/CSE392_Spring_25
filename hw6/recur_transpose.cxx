#include <iostream>
#include <vector>
#include <cmath>
#include <mpl/mpl.hpp>

using namespace std;


void recur_transpose(const mpl::communicator &comm, int &element) {
    int nprocs = comm.size();
    int procno = comm.rank();
    
    // base condition
    if (nprocs == 1) return;
     
    // remake process grid 
    int N = static_cast<int>(sqrt(nprocs));
    int n_row = procno / N;
    int n_col = procno - N*n_row ;
    
    // swap elements of the diagonal blocks
    int swap_procno,temp;
    mpl::tag_t tag;

    // upper right
    if (n_row < (N / 2) && n_col >= (N / 2)) {
        swap_procno = (n_row + (N / 2)) * N + (n_col - N / 2);
        comm.sendrecv(element, swap_procno,tag, temp, swap_procno,tag);
        element = temp;

    //lower left
    } else if (n_row >= (N / 2) && n_col < (N / 2)) {
        swap_procno = (n_row - (N / 2)) * N + (n_col + N / 2);
        comm.sendrecv(element, swap_procno,tag, temp, swap_procno,tag);
        element = temp;
    }

    // split into 4 blocks and call the function on them
    int block = (n_row >= (N / 2)) * 2 + (n_col >= (N / 2));
    mpl::communicator block_comm (mpl::communicator::split_tag(),comm, block);
  
    recur_transpose(block_comm, element);
}

int main() {

  const mpl::communicator &comm_world = mpl::environment::comm_world();
  int nprocs,procno;
  nprocs = comm_world.size();
  procno = comm_world.rank();


  // Each element of the matrix is the process
  int element = procno;
 
  int N= sqrt(nprocs);
  vector<vector<int>> matrix(N,vector<int>(N));
  vector<vector<int>> matrix_trans(N,vector<int>(N));

 // make the matrix using processgrid
 
 if(procno==0){
  vector<int> vector_og (nprocs);
  comm_world.gather(0, element, vector_og.data());
  int k = 0;
  for (int i=0; i<N; i++) 
    for (int j=0; j<N; j++) 
      matrix[i][j] = vector_og[k++];
        
 }   
 else { 
	 comm_world.gather(0,element);
 }

 // do transpose
  recur_transpose(comm_world, element);

  
// reconstruct transposed matrix
 if(procno==0)
 {
  vector<int> vector_trans (nprocs);
  comm_world.gather(0, element, vector_trans.data());
  int k = 0;
  for (int i=0; i<N; i++) 
    for (int j=0; j<N; j++) 
      matrix_trans[i][j] = vector_trans[k++];    
 }
 else{
	comm_world.gather(0,element);
 }	


// test for correctness
if(procno==0)
{
for(int i=0; i<N;i++){
 for(int j=0;j<N;j++)
  {
    if ((i!=j) && (matrix[i][j]!=matrix_trans[j][i]))
    {
    	cout<<"Error at index" <<i<<"\t"<<j<<endl;
    	exit(0);
    }
    if ((i==j) && (matrix[i][j]!=matrix_trans[i][j]))
    {
    	cout<<"Error at index" <<i<<"\t"<<j<<endl;
    	exit(0);
    }
  }
}
cout<<"Transpose done"<<endl;
}
 return 0;
}

