module load cxxopts
mkdir build
cd build
cmake ..
make 
ibrun -n 16 ./project -o 0.00125

