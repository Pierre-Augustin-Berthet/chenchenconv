cmake_minimum_required(VERSION 2.8)

#test projet 
project(gadget)

set(GADGETS_SOURCES src/gadgets.c src/utils.c src/secFPR.c src/floorFPR.c)

include_directories(include)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}")

add_library(toto SHARED ${GADGETS_SOURCES})

add_executable(test_gadgets apps/test_gadgets.c)
target_link_libraries(test_gadgets toto m)

add_executable(test_secfpr apps/test_secfpr.c)
target_link_libraries(test_secfpr toto m)


add_executable(test_utils apps/test_utils.c)
target_link_libraries(test_utils toto m)

add_executable(test_secfloor apps/test_secfloor.c)
target_link_libraries(test_secfloor toto m)