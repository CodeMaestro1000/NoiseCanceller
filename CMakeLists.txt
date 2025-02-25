cmake_minimum_required(VERSION 3.30)
project(NoiseCanceller)

set(CMAKE_CXX_STANDARD 23)

include_directories(/usr/local/include)

add_executable(NoiseCanceller main.cpp
        AudioRouter.cpp
        AudioRouter.h
        Filters.cpp
        Filters.h
)
target_link_libraries(NoiseCanceller /usr/local/lib/libportaudio.dylib)
target_link_libraries(NoiseCanceller /usr/local/lib/librnnoise.dylib)
