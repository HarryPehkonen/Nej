find_package(GTest CONFIG REQUIRED)

add_executable(nej_tests test_main.cpp)
target_link_libraries(nej_tests PRIVATE GTest::gtest_main nej_core)

include(GoogleTest)
gtest_discover_tests(nej_tests)

# Add integration tests
add_test(NAME IntegrationTest_LineByLine COMMAND ${CMAKE_SOURCE_DIR}/tests/integration/test_line_by_line.sh)
