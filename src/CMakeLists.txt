add_library(nej_core STATIC core.cpp)

target_include_directories(nej_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(nej_core PUBLIC utf8cpp)

add_executable(nej main.cpp)
target_link_libraries(nej PRIVATE nej_core)

install(TARGETS nej DESTINATION bin)