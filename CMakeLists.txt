cmake_minimum_required(VERSION 3.2)
set(PROJECT_NAME "Adilkhan_Kussidenov_ASearch")
project(${PROJECT_NAME})

execute_process(
        COMMAND bash "echo DDSFMLEFKEFLEMRKFEMERFLEKRMFELRFKL"
        OUTPUT_VARIABLE FOO
)

if (${MINGW})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -O2 -Wall -Wextra -static -static-libgcc -static-libstdc++")
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -O2 -Wall -Wextra")
endif()

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})

enable_testing()


add_subdirectory(Src)
add_subdirectory(Tests)

