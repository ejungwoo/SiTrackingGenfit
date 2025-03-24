cmake_minimum_required(VERSION 3.10)
project(SITRACKING)

find_package(ROOT REQUIRED COMPONENTS Geom)
include(${ROOT_USE_FILE})

set(GENFIT_ROOT "$ENV{GENFIT}")
find_path(GENFIT_INCLUDE_DIR Track.h PATHS ${GENFIT_ROOT}/include NO_DEFAULT_PATH)
find_library(GENFIT_LIB genfit2 PATHS ${GENFIT_ROOT}/lib NO_DEFAULT_PATH)
message(STATUS "GENFIT_ROOT: ${GENFIT_ROOT}")
message(STATUS "GENFIT_INCLUDE_DIR: ${GENFIT_INCLUDE_DIR}")
message(STATUS "GENFIT_LIB: ${GENFIT_LIB}")

foreach(program
        minimalFittingExample
        measurementFactoryExample
        siTracking
    )
    message(STATUS ${program})
    add_executable(${program} ${program}.cc source/MyField.cc)
    target_include_directories(${program} PRIVATE ${GENFIT_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/source)
    target_link_libraries(${program} ${ROOT_LIBRARIES} ${GENFIT_LIB} ROOT::Core ROOT::Physics ROOT::Eve)
ENDFOREACH(program)

set(COPY_FILE_LIST)
file(GLOB COPY_FILE_LIST "${CMAKE_SOURCE_DIR}/fieldMap.root" "${CMAKE_SOURCE_DIR}/detectorGeometry.root")
foreach(copy_file ${COPY_FILE_LIST})
    message(STATUS "copying ${copy_file}")
    get_filename_component(just_name ${copy_file} NAME)
    configure_file(${copy_file} ${CMAKE_BINARY_DIR}/${just_name} COPYONLY)
endforeach()
