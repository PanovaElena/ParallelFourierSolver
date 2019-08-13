function( add_element_to_global_property NAME_PROPERTY ELEMENT )
    get_property( PROP GLOBAL PROPERTY ${NAME_PROPERTY} )
    list( APPEND PROP ${ELEMENT} )
    set_property ( GLOBAL PROPERTY ${NAME_PROPERTY} ${PROP} )
    
endfunction()


function( create_library TARGET )
    file( GLOB TARGET_SRC "src/*.cpp" "src/*.cc" "include/*.h" )
    add_library( ${TARGET} STATIC ${TARGET_SRC} )
    
    target_include_directories( ${TARGET} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${FFT_INCLUDES}
        ${MPI_CXX_INCLUDE_DIRS})
    
    add_element_to_global_property( MY_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/include )
    add_element_to_global_property( MY_TEST_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/include )
    add_element_to_global_property( MY_LIBRARIES ${TARGET} )
    
endfunction()


function( create_executable TARGET)
    if ( EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src" )
        set( TARGET_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp )
    endif()
    if ( EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/include" )
        set( TARGET_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include )
        set( TARGET_INCLUDE_FILES ${TARGET_INCLUDE_FILES} ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h )
    endif()
    if ( EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../include" )
        set( TARGET_INCLUDE_DIRS ${TARGET_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/../include )
        set( TARGET_INCLUDE_FILES ${TARGET_INCLUDE_FILES} ${CMAKE_CURRENT_SOURCE_DIR}/../include/*.h )
    endif()
    if ( EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../../include" )
        set( TARGET_INCLUDE_DIRS ${TARGET_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/../../include )
        set( TARGET_INCLUDE_FILES ${TARGET_INCLUDE_FILES} ${CMAKE_CURRENT_SOURCE_DIR}/../../include/*.h )
    endif()
    file( GLOB TARGET_SRC ${TARGET_SRC_FILES} ${TARGET_INCLUDE_FILES} )
        
    add_executable( ${TARGET} ${TARGET_SRC} )
    
    if (USE_FFTW AND NOT FFTW_DIR) 
        add_dependencies( ${TARGET} project_fftw )
    endif()
     
    get_property( MY_LIB_INCLUDES GLOBAL PROPERTY MY_INCLUDES )
    target_include_directories( ${TARGET} PUBLIC
        ${TARGET_INCLUDE_DIRS}
        ${MY_LIB_INCLUDES}
        ${FFT_INCLUDES}
        ${MPI_CXX_INCLUDE_DIRS})
    
    get_property( MY_LIBS GLOBAL PROPERTY MY_LIBRARIES )
    if ( UNIX )
        target_link_libraries( ${TARGET} 
            ${MY_LIBS}
            -Wl,--start-group ${FFT_LIBRARIES} -Wl,--end-group
            ${MPI_CXX_LIBRARIES})
    elseif( WIN32 )
        target_link_libraries( ${TARGET} 
            ${MY_LIBS}
            ${FFT_LIBRARIES}
            ${MPI_CXX_LIBRARIES})
    endif()
    target_link_libraries( ${TARGET} ${ALL_TARGET_LIBS} )
    
    add_element_to_global_property( MY_TEST_INCLUDES ${TARGET_INCLUDE_DIRS} )

endfunction()

function( create_tests )
    set( TARGET tests )
    if ( EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src" )
        set( TARGET_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp )
    endif()
    file( GLOB TARGET_SRC ${TARGET_SRC_FILES} )
        
    add_executable( ${TARGET} ${TARGET_SRC} )
    
    if (USE_FFTW AND NOT FFTW_DIR) 
        add_dependencies( ${TARGET} project_fftw )
    endif()
    
    get_property( MY_ALL_TEST_INCLUDES GLOBAL PROPERTY MY_TEST_INCLUDES )
    target_include_directories( ${TARGET} PUBLIC
        ${MY_ALL_TEST_INCLUDES}
        ${FFT_INCLUDES}
        ${MPI_CXX_INCLUDE_DIRS})
    
    get_property( MY_LIBS GLOBAL PROPERTY MY_LIBRARIES )
    if ( UNIX )
        target_link_libraries( ${TARGET}
            ${MY_LIBS}
            -Wl,--start-group ${FFT_LIBRARIES} -Wl,--end-group
            ${MPI_CXX_LIBRARIES})
    elseif( WIN32 )
        target_link_libraries( ${TARGET} 
            ${MY_LIBS}
            ${FFT_LIBRARIES}
            ${MPI_CXX_LIBRARIES})
    endif()
    target_link_libraries( ${TARGET} ${ALL_TARGET_LIBS} )
    

endfunction()

