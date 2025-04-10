#include <iostream>
#include <math.h>
#include <chrono>
using namespace std::chrono;
using myclock = steady_clock;
#include "cxxopts.hpp"

// function to add the elements of two arrays
__global__
void add1(size_t n, float *x, float *y, float *z, float *coeff,int ntimes)
{
  //size_t t_id = blockIdx.x * blockDim.x + threadIdx.x;
  size_t row = blockIdx.y * blockDim.y + threadIdx.y;
  size_t col = blockIdx.x * blockDim.x + threadIdx.x;
  size_t t_id = row * n + col;

  if (row < n && col < n){

   for(int i=0; i < ntimes; i++)
     z[t_id] += coeff[i]*(x[t_id] + y[t_id]);
}
}

__global__
void add2(size_t n, float *x, float *y, float *z,float *coeff, int ntimes)
{
  //size_t t_id = blockIdx.x * blockDim.x + threadIdx.x;
  size_t row = blockIdx.y * blockDim.y + threadIdx.y;
  size_t col = blockIdx.x * blockDim.x + threadIdx.x;
  size_t t_id = row * n + col;

  //int thrd_idx = threadIdx.x;
   extern __shared__ float c[];
 
  for (int i = threadIdx.y * blockDim.x + threadIdx.x; i < ntimes; i += blockDim.x * blockDim.y)
    c[i] = coeff[i];

  __syncthreads();

  if (row < n && col < n){

    for(int i=0; i < ntimes; i++)
      z[t_id] += c[i]*(x[t_id] + y[t_id]);

}
}


#define CU_ASSERT( code ) { \
  cudaError_t err = code ; \
  if (err!=cudaSuccess) { \
    printf("error <<%s>> on line %d\n",cudaGetErrorString(err),__LINE__); \
    return 1; } }

int main(int argc,char **argv) {

  cxxopts::Options options
    ( "cxxopts","Weighted addition of 2 vectors " );
  options.add_options()
    ( "h,help","usage information" )
    ( "n,nsize","size of vector", cxxopts::value<size_t>()->default_value("100") )
    ( "b,blocksize","blocksize (less than 32 please)", cxxopts::value<size_t>()->default_value("32") )

    ( "t,ntimes","number of times vector addition to be done", cxxopts::value<int>()->default_value("32") );
  auto result = options.parse(argc, argv);
  if (result.count("help")>0) {
    std::cout << options.help() << '\n';
    return 0;
  }
  /* } catch ( cxxopts::exceptions::option_has_no_value ) { */
  /*   std::cout << "Option has no value" << '\n'; */
  /* } */


  auto N = result["nsize"].as<size_t>();
  auto Nb = result["blocksize"].as<size_t>();
  if(Nb>32) {
   Nb=32;
   std::cout<<"Block size can't be greater than 32";
  }
  auto ntimes = result["ntimes"].as<int>();

  printf("Running with N=%ld, blocksize=%ld and # vector additions =%d \n",N,Nb,ntimes);

  float
    *x, *y, *z , *coeff;

  size_t nbytes = N*N*sizeof(float);

  CU_ASSERT( cudaMallocManaged( &coeff,ntimes*sizeof(float)));
  CU_ASSERT( cudaMallocManaged( &x,nbytes ) );
  CU_ASSERT( cudaMallocManaged( &y,nbytes ) );
  CU_ASSERT( cudaMallocManaged( &z,nbytes ) );

//do the same operation twice one with coeffcients in shared memory and other without
for(int k =0; k<2; k++){

  // initialize x and y arrays on the host
  for (size_t i = 0; i < N*N; i++) {
    x[i] = 1.0f;
    y[i] = 2.0f;
    z[i] = 0.0f;
  }

  // Run kernel on 1M elements on the CPU
  dim3 blocksize(Nb,Nb);
  dim3 gridsize((N+Nb-1)/Nb,(N+Nb-1)/Nb);

  for (int i =0; i < ntimes; i++)
    coeff[i]=1.0f;


  myclock::time_point before =
    myclock::now();


   k==0  ? add1<<<gridsize,blocksize>>>(N,x,y,z,coeff,ntimes) : add2<<<gridsize,blocksize,ntimes * sizeof(float)>>>(N,x,y,z,coeff,ntimes) ;

  // Wait for GPU to finish before accessing data on host
  CU_ASSERT( cudaDeviceSynchronize() );

  auto after = myclock::now();

  if (k==0){
  std::cout << "Without shared memory took: "
       << duration_cast<microseconds>
            (after-before).count()
       << "micro sec\n";
   }
   else{
    std::cout << "With shared memory took: "
    << duration_cast<microseconds>
         (after-before).count()
    << "micro sec\n";
   }
  // Check for errors (all values should be 3.0f)
  float maxError = 0.0f;
  float sum=0.0f;
  for (int i = 0; i < N*N; i++) {
    //printf("%4.1f",z[i]);
     sum+=z[i];
     if(fabs (z[i]-ntimes*3.0f)>1e-3) {
      maxError = fabs(z[i]-ntimes*3.0f);
      printf("error at %d\n",i);
      break;
    }
  }
  std::cout << "Max error: " << maxError <<std::endl;


}

cudaFree(x);
cudaFree(y);
cudaFree(z);
cudaFree(coeff);
  return 0;
}
