include_directories(../Src)


add_executable(testbench testbench.cpp)
#find_package(SFML 2.5 COMPONENTS graphics REQUIRED)
#target_link_libraries(testbench sfml-graphics)


target_link_libraries(testbench PPLIB)

set(validator ${CMAKE_CURRENT_SOURCE_DIR}/testbench)

file(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/../TestData/street-map_xml/*.xml")
set(i 0)

message(STATUS ${CMAKE_CURRENT_SOURCE_DIR})

foreach(filename ${files})
    add_test(NAME "validate_${i}"
            COMMAND testbench ${filename})
    math(EXPR i "${i} + 1")
endforeach()


add_test(NAME Test1 COMMAND testbench ../../../TestData/0_test.xml)
add_test(NAME Test2 COMMAND testbench ../../../TestData/1_test.xml)
add_test(NAME Test3 COMMAND testbench ../../../TestData/2_test.xml)
add_test(NAME Test4 COMMAND testbench ../../../TestData/3_test.xml)
add_test(NAME Test5 COMMAND testbench ../../../TestData/4_test.xml)
add_test(NAME Test6 COMMAND testbench ../../../TestData/5_test.xml)
add_test(NAME Test7 COMMAND testbench ../../../TestData/6_test.xml)
add_test(NAME Test8 COMMAND testbench ../../../TestData/7_test.xml)
add_test(NAME Test9 COMMAND testbench ../../../TestData/8_test.xml)
add_test(NAME Test10 COMMAND testbench ../../../TestData/9_test.xml)
add_test(NAME Test11 COMMAND testbench ../../../TestData/A_test.xml)
add_test(NAME Test12 COMMAND testbench ../../../TestData/B_test.xml)