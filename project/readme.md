NOT FINAL SUBMISSION. WILL SUBMIT SOON THE WRITEUP SOON> APOLOGIES FOR DELAY

module load cxxopts
mkdir build
cd build
cmake ..
make 
ibrun -n 16 ./mpm_2d -o 0.00125

