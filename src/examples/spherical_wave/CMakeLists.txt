get_property( PROP GLOBAL PROPERTY EXAMPLES_INCLUDE )
add_element_to_global_property( SPHERICAL_WAVE_INCLUDE "${PROP};${CMAKE_CURRENT_SOURCE_DIR}/include" )

add_subdirectory( spherical_wave_sequential )
add_subdirectory( spherical_wave_parallel )