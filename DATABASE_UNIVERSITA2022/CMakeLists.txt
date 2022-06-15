cmake_minimum_required(VERSION 3.21)
project(DATABASE_UNIVERSITA2022)

set(CMAKE_CXX_STANDARD 17)
set(SOURCE_FILES main.cpp uni_database.h utility.h dates.h mmath.h exam_scheduler.h university.h)

set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} "-static")
add_executable(DATABASE_UNIVERSITA2022 ${SOURCE_FILES})