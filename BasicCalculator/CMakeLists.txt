cmake_minimum_required(VERSION 3.5)
project(SimpleCalculator)

set(CMAKE_CXX_STANDARD 11)

find_package(Qt5 COMPONENTS Widgets Qml REQUIRED)

set(SOURCES
    main.cpp
    src/calculator.cpp
)

set(HEADERS
    include/calculator.h
)

include_directories(include)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

add_executable(SimpleCalculator ${SOURCES} ${HEADERS})
target_link_libraries(SimpleCalculator Qt5::Widgets Qt5::Qml)