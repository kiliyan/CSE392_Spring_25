cmake_minimum_required(VERSION 3.18)
project(shared_ops LANGUAGES CXX CUDA)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CUDA_STANDARD 17)

# Set executable name using PROJECT_NAME (auto set by CMake)
add_executable(${PROJECT_NAME} ${PROJECT_NAME}.cu)

# Use the include path from the TACC environment
if(DEFINED ENV{TACC_CXXOPTS_INC})
    message(STATUS "Using cxxopts from $ENV{TACC_CXXOPTS_INC}")
    target_include_directories(${PROJECT_NAME} PRIVATE $ENV{TACC_CXXOPTS_INC})
else()
    message(FATAL_ERROR "Make sure to module load cxxopts.")
endif()
