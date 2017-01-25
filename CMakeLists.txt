cmake_minimum_required(VERSION 3.0)
project(recurstats VERSION 0.0.1 LANGUAGES C)

# CMAKE_C_FLAGS
execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpmachine OUTPUT_VARIABLE TARGET_PLATFORM) #put into TARGET_PLATFORM result of $ gcc -dumpmachine
string(REGEX MATCH i586-poky-linux IS_POKY_LINUX ${TARGET_PLATFORM})
if(IS_POKY_LINUX)
  message(STATUS "Building for i586-poky-linux (Edison) platform")
  add_definitions(-DPOKY_LINUX=1)
endif()

# FILES
set(SOURCE_DIR src) #set a variable
set(EXEC_DIR exe) #set a variable
file(GLOB LIB_SOURCES "${SOURCE_DIR}/rs_*.c") #put into LIB_SOURCES the globbed list

# TARGETS
add_compile_options(-std=c11)

add_library(recurstats_static STATIC ${LIB_SOURCES})
add_library(recurstats SHARED ${LIB_SOURCES})
add_executable(rs_test ${EXEC_DIR}/rs_test.c)
target_link_libraries(rs_test recurstats_static) # link to mm_gps when building rs_test
add_executable(cdf_main ${EXEC_DIR}/cdf_main.c ${SOURCE_DIR}/rs_cdf_t.c)


