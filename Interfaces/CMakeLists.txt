
# Libraries

if( USE_CUBLAS )
    add_definitions( -DUSE_CUBLAS )
endif()

add_library( GpufitInterface SHARED
    PulseFitInterface.cpp
    ${PROJECT_SOURCE_DIR}/tests/utils.h
    ${PROJECT_SOURCE_DIR}/tests/utils.cpp
)

target_include_directories( GpufitInterface PRIVATE ${PROJECT_SOURCE_DIR} )

target_link_libraries( GpufitInterface Gpufit Cpufit )
