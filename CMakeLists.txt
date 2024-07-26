cmake_minimum_required(VERSION 3.25.1) #Po prostu wersja ktora mam

project(podlewanie VERSION "1.1.0")

SET(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -g -O3 -Wall")

set(MAIN_SOURCE main.cpp)
set(MAIN_TARGET program_podlewajacy)
set(wiringPiLib ~/Desktop/wiringPi/WiringPi/wiringPi/libwiringPi.so.3.6)

add_executable(${MAIN_TARGET} ${MAIN_SOURCE})

target_link_libraries(${MAIN_TARGET} ${wiringPiLib})
