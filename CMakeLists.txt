cmake_minimum_required(VERSION 3.4.1)
project(jndcrash)
set(SOURCE_ROOT ${CMAKE_SOURCE_DIR}/src/main/jni)

add_subdirectory(libndcrash)

if (${ENABLE_INPROCESS})
    add_definitions(-DENABLE_INPROCESS)
endif()
if (${ENABLE_OUTOFPROCESS})
    add_definitions(-DENABLE_OUTOFPROCESS)
endif()

include_directories(${CMAKE_SOURCE_DIR}/libndcrash/include)
file(GLOB JNDCRASH_SOURCES ${SOURCE_ROOT}/*.c)
add_library(jndcrash SHARED ${JNDCRASH_SOURCES})
find_library(LOG_LIB log)
target_link_libraries(jndcrash ndcrash ${LOG_LIB})
