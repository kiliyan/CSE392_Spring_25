cmake_minimum_required( VERSION 3.13 )
project( stridescatter VERSION 1.0 )

add_executable( stridescatter )
target_sources( stridescatter PRIVATE stridescatter.cxx )

# find_package( MPI )
# target_include_directories(
# 	stridescatter PUBLIC
# 	${MPI_C_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR} )
# target_link_libraries(
# 	stridescatter PUBLIC 
# 	${MPI_C_LIBRARIES} )

find_package( mpl REQUIRED )
target_include_directories(
        stridescatter PUBLIC
        mpl::mpl )
target_link_libraries(
        stridescatter PUBLIC
        mpl::mpl )

install( TARGETS stridescatter DESTINATION . )

