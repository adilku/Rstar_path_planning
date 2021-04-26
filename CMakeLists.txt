cmake_minimum_required(VERSION 3.2)

set(VISUAL_MODE 1)

if (${VISUAL_MODE})
    set(PROJECT_NAME "RstarSearchVisualOn")
else()
    set(PROJECT_NAME "RstarSearch")
endif()

project(${PROJECT_NAME})



if (${MINGW})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -O2 -Wall -Wextra -static -static-libgcc -static-libstdc++")
else ()
    if (${VISUAL_MODE})
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVISUAL_MODE=1 -std=c++17 -O3 -Wall -Wextra")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -O3 -Wall -Wextra")
    endif()
endif()


set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})

enable_testing()

add_subdirectory(Src)
add_subdirectory(Tests)

