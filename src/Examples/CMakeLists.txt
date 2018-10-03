get_property( PROP GLOBAL PROPERTY ALL_INCLUDE)
add_element_to_global_property(EXAMPLES_INCLUDE "${PROP};${CMAKE_CURRENT_SOURCE_DIR}/include")

add_subdirectory(RunningWave)
add_subdirectory(SphericalWave)
