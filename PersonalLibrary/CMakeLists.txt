cmake_minimum_required(VERSION 3.16)
project(PersonalLibraryDDD)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt automatic processing
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# External dependencies
find_package(PkgConfig REQUIRED)
pkg_check_modules(PQXX REQUIRED libpqxx)
pkg_check_modules(SODIUM REQUIRED libsodium)
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# Include external headers
include_directories(${PQXX_INCLUDE_DIRS} ${SODIUM_INCLUDE_DIRS})
link_directories(${PQXX_LIBRARY_DIRS} ${SODIUM_LIBRARY_DIRS})

# Add internal modules
add_subdirectory(domains)
add_subdirectory(application)
add_subdirectory(infrastructure)
add_subdirectory(tests)
add_subdirectory(ui)

# Main executable
add_executable(main main.cpp)

# Include project-wide headers
include_directories(${PROJECT_SOURCE_DIR})

# Link libraries to main
target_link_libraries(main
    domain_user
    app_user
    infra_user
    test_user
    ui_login
    ${PQXX_LIBRARIES}
    ${SODIUM_LIBRARIES}
    Qt6::Widgets
)