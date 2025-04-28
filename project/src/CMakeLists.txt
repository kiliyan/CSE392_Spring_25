cmake_minimum_required(VERSION 3.12)
project(${PROJECT_NAME} VERSION 1.0)

# Find MPI package
find_package(MPI REQUIRED)

# Add source files (all .cpp files in your project)
set(SOURCES
    main.cpp
    grid.cpp
    material.cpp
    basis.cpp
)

# Add header files (although headers aren't compiled directly, they are included in the compilation)
set(HEADERS
    grid.h
    material.h
    basis.h
)

# Define the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Set C++20 standard
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

# Include MPI directories and current source directory
target_include_directories(${PROJECT_NAME} PUBLIC
    ${MPI_CXX_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link MPI libraries
target_link_libraries(${PROJECT_NAME} PUBLIC
    ${MPI_CXX_LIBRARIES}
)

# Install target to the current directory
install(TARGETS ${PROJECT_NAME} DESTINATION .)

