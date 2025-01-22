cmake_minimum_required( VERSION 3.13 )
project( ex5 VERSION 1.0 )

add_executable( ex5 ex5.cxx )
find_package( MPI )
target_include_directories(
	ex5 PUBLIC
	${MPI_C_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR} )
target_link_libraries(
	ex5 PUBLIC 
	${MPI_C_LIBRARIES} )

install( TARGETS ex5 DESTINATION . )
