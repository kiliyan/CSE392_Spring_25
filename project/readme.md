
module load cxxopts 

mkdir build 

cd build 

cmake .. 

make 

ibrun -n 48 ./mpm_2d -o 0.00125 -g 40







