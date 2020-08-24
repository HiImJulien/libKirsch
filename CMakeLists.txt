cmake_minimum_required(VERSION 3.4)
project(libKirsch)

add_library(libKirsch INTERFACE)
target_include_directories(
    libKirsch
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(
    libKirsch
    INTERFACE
        cxx_std_11
)

add_library(kirsch::kirsch ALIAS libKirsch)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include (CTest)

    add_executable(
        delegate_test
        ${CMAKE_CURRENT_SOURCE_DIR}/test/test_delegate.cpp
    )

    target_link_libraries(
        delegate_test
        PRIVATE
            kirsch::kirsch
    )

    add_executable(
        signal_test
        ${CMAKE_CURRENT_SOURCE_DIR}/test/test_signal.cpp
    )

    target_link_libraries(
        signal_test
        PRIVATE
            kirsch::kirsch
    )

    enable_testing()
    add_test(
        NAME test_delegate
        COMMAND delegate_test
    )

    add_test(
        NAME test_signal
        COMMAND signal_test
    )
endif()