function( add_element_to_global_property NAME_PROPERTY ELEMENT )
    get_property( PROP GLOBAL PROPERTY ${NAME_PROPERTY} )
    list( APPEND PROP ${ELEMENT} )
    set_property ( GLOBAL PROPERTY ${NAME_PROPERTY} ${PROP} )
    
endfunction()


function( create_library TARGET )
    file( GLOB TARGET_SRC "src/*.cpp" "src/*.cc" "include/*.h" )
    add_library( ${TARGET} STATIC ${TARGET_SRC} )
    
    target_include_directories( ${TARGET} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include )
    
    add_element_to_global_property( ALL_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include )
    add_element_to_global_property( ALL_LIBS ${TARGET} )
	
endfunction()


function( create_executable TARGET PROP_NAME )
    file( GLOB TARGET_SRC "src/*.cpp" "include/*.h" "../include/*.h" )
    add_executable( ${TARGET} ${TARGET_SRC} )
    if (USE_FFTW) 
	    add_dependencies( ${TARGET} project_fftw )
	endif()
	 
    get_property( INCLUDE_DIRS GLOBAL PROPERTY ${PROP_NAME} )
	#message("${TARGET}: ${INCLUDE_DIRS}")
    target_include_directories( ${TARGET} PUBLIC ${INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/include )
    
    get_property( LIB_LIST GLOBAL PROPERTY ALL_LIBS )
    target_link_libraries( ${TARGET} ${LIB_LIST} )

endfunction()

