cmake_minimum_required(VERSION 3.2)
set(PROJECT_NAME "RstarSearch")
project(${PROJECT_NAME})

set(VISUAL_MODE 1)

if (${MINGW})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -O2 -Wall -Wextra -static -static-libgcc -static-libstdc++")
else ()
    if (${VISUAL_MODE})
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVISUAL_MODE=1 -std=c++17 -O0 -Wall -Wextra")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -O0 -Wall -Wextra")
    endif()
endif()


set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})

enable_testing()

add_subdirectory(Src)
add_subdirectory(Tests)

