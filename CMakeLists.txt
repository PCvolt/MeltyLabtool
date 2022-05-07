cmake_minimum_required(VERSION 3.19)
project(MeltyLabtool)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(MeltyLib)

add_library(Labtool SHARED
        src/DLLmain.cpp
        src/Memory.cpp src/Memory.h
        src/IsInAction.cpp src/IsInAction.h
        src/Reversal.cpp src/Reversal.h
        src/Guard.cpp src/Guard.h
        src/Position.cpp src/Position.h
        src/Framedata.cpp src/Framedata.h
        src/FunctionCallToggler.cpp src/FunctionCallToggler.h src/Menu.cpp src/Menu.h)

add_executable(MeltyLabtool src/DLLInjector.cpp)
target_link_libraries(Labtool MeltyLib)