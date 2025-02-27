cmake_minimum_required( VERSION 3.13 )
project( recur_transpose VERSION 1.0 )

set(CMAKE_CXX_FLAGS "-O3 -flto -march=native")

add_executable( recur_transpose )
target_sources( recur_transpose PRIVATE recur_transpose.cxx )

# find_package( MPI )
# target_include_directories(
# 	recur_transpose PUBLIC
# 	${MPI_C_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR} )
# target_link_libraries(
# 	recur_transpose PUBLIC 
# 	${MPI_C_LIBRARIES} )

find_package( mpl REQUIRED )
target_include_directories(
        recur_transpose PUBLIC
        mpl::mpl )
target_link_libraries(
        recur_transpose PUBLIC
        mpl::mpl )

install( TARGETS recur_transpose DESTINATION . )

