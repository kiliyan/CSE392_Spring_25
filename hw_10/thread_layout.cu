#include <iostream>
#include <math.h>
#include <chrono>
using namespace std::chrono;
using myclock = steady_clock;
#include "cxxopts.hpp"


__global__ void fillThreadIDs(int* thread_ids, int m, int n, int M, int N) {
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;
    int t_id = row * n * N + col;

    int grid_x = t_id / (m*n*M);
    int grid_y = (t_id % (m*n*M)) / (m*n);

    int block_x = ((t_id % (m*n*M)) / m) % n;
    int block_y = (t_id % (m*n*M)) % m;

    int i = grid_x*n + block_x;
    int j = grid_y*m + block_y;

    thread_ids[i*m*M + j] = t_id;
}

#define CU_ASSERT( code ) { \
  cudaError_t err = code ; \
  if (err!=cudaSuccess) { \
    printf("error <<%s>> on line %d\n",cudaGetErrorString(err),__LINE__); \
    return 1; } }

int main(int argc,char **argv) {


  cxxopts::Options options
    ( "cxxopts","Thread index figure" );
    options.add_options()
        ("h,help", "Usage information")
        ("n,block_row", "Rows in each block", cxxopts::value<int>()->default_value("2"))
        ("m,block_col", "Columns in each block", cxxopts::value<int>()->default_value("3"))
        ("N,grid_row", "Number of blocks in  ", cxxopts::value<int>()->default_value("3"))
        ("M,grid_col", "Columns in each grid", cxxopts::value<int>()->default_value("2"));

  auto result = options.parse(argc, argv);
  if (result.count("help")>0) {
    std::cout << options.help() << '\n';
    return 0;
  }


  auto n = result["block_row"].as<int>();
  auto m = result["block_col"].as<int>();
  auto N = result["grid_row"].as<int>();
  auto M = result["grid_col"].as<int>();

  myclock::time_point before =
    myclock::now();

  dim3 blocksize(n,m);
  dim3 gridsize(N,M);

  int *thread_ids;
  size_t nbytes = m*n*N*M*sizeof(int);
  CU_ASSERT( cudaMallocManaged( &thread_ids,nbytes ) );

  fillThreadIDs<<<gridsize,blocksize>>>(thread_ids,m,n,M,N);



  //add1<<<gridsize,blocksize>>>(N,x,y,z);
  // Wait for GPU to finish before accessing data on host
  CU_ASSERT( cudaDeviceSynchronize() );


  for(int i=0; i<n*N; i++){
    std::cout<<"\n";
    for(int j=0; j<m*M;j++)
    {

        (thread_ids[i*m*M+j]>9) ? std::cout<<thread_ids[i*m*M + j]<<" " : std::cout<<thread_ids[i*m*M + j]<<"  ";
        if(j%m==m-1) std::cout<<"\t";
    }
    if(i%n==n-1) std::cout<<"\n";
  }

  std::cout <<"\n";

  //for(int i=0; i<n*m*N*M;i++)
  // std::cout<<thread_ids[i]<<" ";


  auto after = myclock::now();
  std::cout << "\nAction took: "
       << duration_cast<milliseconds>
            (after-before).count()
       << "ms\n";

  cudaFree(thread_ids);

  return 0;
}
