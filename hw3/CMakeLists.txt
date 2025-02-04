cmake_minimum_required( VERSION 3.13 )
project( scangather VERSION 1.0 )

add_executable( scangather )
target_sources( scangather PRIVATE scangather.cxx )

# find_package( MPI )
# target_include_directories(
# 	scangather PUBLIC
# 	${MPI_C_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR} )
# target_link_libraries(
# 	scangather PUBLIC 
# 	${MPI_C_LIBRARIES} )

find_package( mpl REQUIRED )
target_include_directories(
        scangather PUBLIC
        mpl::mpl )
target_link_libraries(
        scangather PUBLIC
        mpl::mpl )

install( TARGETS scangather DESTINATION . )

