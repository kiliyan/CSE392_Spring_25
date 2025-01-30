cmake_minimum_required( VERSION 3.13 )
project( randommax VERSION 1.0 )

add_executable( randommax )
target_sources( randommax PRIVATE randommax.cxx )

# find_package( MPI )
# target_include_directories(
# 	randommax PUBLIC
# 	${MPI_C_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR} )
# target_link_libraries(
# 	randommax PUBLIC 
# 	${MPI_C_LIBRARIES} )

find_package( mpl REQUIRED )
target_include_directories(
        randommax PUBLIC
        mpl::mpl )
target_link_libraries(
        randommax PUBLIC
        mpl::mpl )

install( TARGETS randommax DESTINATION . )

