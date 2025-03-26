cmake_minimum_required(VERSION 3.12)
project(BluetoothAttackTool LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

find_library(BLUETOOTH_LIB bluetooth)
find_package(Threads REQUIRED)

if(NOT BLUETOOTH_LIB)
    message(FATAL_ERROR "Bluetooth development libraries not found!")
endif()

add_library(packet_manipulator
    mal.cc
    mal.hpp
)
add_executable(bt_attack
    main.c
    attack.c
    threads.c
    scan.c  
)
target_link_libraries(bt_attack
    PRIVATE
    ${BLUETOOTH_LIB}
    Threads::Threads
    packet_manipulator
)

target_compile_definitions(bt_attack PRIVATE USING_CXX=1)
target_include_directories(bt_attack PRIVATE include)
