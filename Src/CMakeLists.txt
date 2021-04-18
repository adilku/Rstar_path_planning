
set(HEADERS         config.h
                    environmentoptions.h
                    gl_const.h
                    ilogger.h
                    map.h
                    mission.h
                    node.h
                    search.h
                    searchresult.h
                    tinyxml2.h
                    xmllogger.h
                    open_container.h
                    search_rstar.h
                    draw_path.h
                    open_rstar.h
                    node_rstar.h)

set(SOURCES         config.cpp
                    environmentoptions.cpp
                    map.cpp
                    mission.cpp
                    search.cpp
                    tinyxml2.cpp
                    xmllogger.cpp
                    open_container.cpp
                    search_rstar.cpp
                    draw_path.cpp
                    open_rstar.cpp
                    node_rstar.cpp)


#set(SFML_DIR "${PROJECT_SOURCE_DIR} /src/SFML/lib/cmake/SFML")

find_package(SFML 2.5 COMPONENTS graphics REQUIRED CONFIG)

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES} main.cpp)

target_link_libraries(${PROJECT_NAME} sfml-graphics)

add_library(PPLIB ${HEADERS} ${SOURCES})

install(
        TARGETS ${PROJECT_NAME}
        CONFIGURATIONS Debug
        DESTINATION Bin/Debug
        )

install(
        TARGETS ${PROJECT_NAME}
        CONFIGURATIONS Release
        DESTINATION Bin/Release
        )



set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})

