get_property( PROP GLOBAL PROPERTY ALL_INCLUDE )
add_element_to_global_property( TASKS_INCLUDE "${PROP};${CMAKE_CURRENT_SOURCE_DIR}/include" )

add_subdirectory( running_wave )
add_subdirectory( spherical_wave )
