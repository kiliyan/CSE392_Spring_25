cmake_minimum_required(VERSION 3.12)
project(${PROJECT_NAME} VERSION 1.0)

find_package(MPI REQUIRED)

# Check if cxxopts is available via the TACC_CXXOPTS_INC environment variable
if(DEFINED ENV{TACC_CXXOPTS_INC})
    message(STATUS "Using cxxopts from $ENV{TACC_CXXOPTS_INC}")
    target_include_directories(${PROJECT_NAME} PRIVATE $ENV{TACC_CXXOPTS_INC})
else()
    message(FATAL_ERROR "Make sure to module load cxxopts.")
endif()


set(SOURCES src/main.cpp src/grid.cpp src/material.cpp src/basis.cpp)

set(HEADERS src/grid.h src/material.h src/basis.h)


add_executable(${PROJECT_NAME} ${SOURCES})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)


target_include_directories(${PROJECT_NAME} PUBLIC ${MPI_CXX_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR} )

target_link_libraries(${PROJECT_NAME} PUBLIC ${MPI_CXX_LIBRARIES})

install(TARGETS ${PROJECT_NAME} DESTINATION .)

