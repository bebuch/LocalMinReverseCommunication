cmake_minimum_required(VERSION 3.20)

project("LocalMinReverseCommunication")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Create library target
add_library(LocalMinReverseCommunication INTERFACE)
target_include_directories(LocalMinReverseCommunication INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>>")

# Setup GoogleTest
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY "https://github.com/google/googletest"
    GIT_TAG        "v1.15.2")
set(FETCHCONTENT_QUIET OFF)
FetchContent_MakeAvailable(googletest)
set(INSTALL_GTEST OFF)

# Create tests executable
enable_testing()

add_executable(tests "test/tests.cpp")
target_link_libraries(tests LocalMinReverseCommunication gtest_main)

include(GoogleTest)
gtest_discover_tests(tests)

# Install
install(DIRECTORY "include/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
install(TARGETS LocalMinReverseCommunication
    EXPORT LocalMinReverseCommunicationTargets
    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
install(EXPORT LocalMinReverseCommunicationTargets
    FILE "LocalMinReverseCommunicationConfig.cmake"
    NAMESPACE "LocalMinReverseCommunication::"
    DESTINATION "lib/cmake/LocalMinReverseCommunication")
