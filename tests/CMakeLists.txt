# Test CMakeLists.txt
add_executable(interlaced_core_tests main.cpp)

add_executable(logging_test logging_test.cpp)

add_executable(filesystem_test filesystem_test.cpp)

add_executable(network_test network_test.cpp)

target_link_libraries(interlaced_core_tests interlaced_core)
target_link_libraries(logging_test interlaced_core)
target_link_libraries(filesystem_test interlaced_core)
target_link_libraries(network_test interlaced_core)

# Add Windows socket library for network tests
if(WIN32)
    target_link_libraries(network_test ws2_32)
endif()

# Add tests to ctest
enable_testing()

add_test(NAME interlaced_core_tests COMMAND interlaced_core_tests)
add_test(NAME logging_test COMMAND logging_test)
add_test(NAME filesystem_test COMMAND filesystem_test)
add_test(NAME network_test COMMAND network_test)