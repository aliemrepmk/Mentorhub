cmake_minimum_required(VERSION 3.16)
project(BasicDB VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Qt auto features
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Qt & PostgreSQL dependencies
find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBPQXX REQUIRED libpqxx)
pkg_check_modules(LIBSODIUM REQUIRED libsodium)

# Include directories
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/include/database
    ${PROJECT_SOURCE_DIR}/include/gui
    ${LIBPQXX_INCLUDE_DIRS}
    ${LIBSODIUM_INCLUDE_DIRS}
    /opt/homebrew/include
)

# Link directories (for pqxx & libsodium)
link_directories(${LIBPQXX_LIBRARY_DIRS} ${LIBSODIUM_LIBRARY_DIRS})

# Source files
set(SOURCES
    main.cpp
    src/database/dbManager.cpp
    src/database/userManager.cpp
    src/gui/loginWindow.cpp
    src/gui/registerWindow.cpp
)

# Header files
set(HEADERS
    include/database/dbManager.hpp
    include/database/userManager.hpp
    include/gui/loginWindow.hpp
    include/gui/registerWindow.hpp
)

# UI files
set(UIS
    gui_designs/LoginWindow.ui
)

# Add UI directory to Qt's UIC include path
set(CMAKE_AUTOUIC_SEARCH_PATHS ${PROJECT_SOURCE_DIR}/gui_designs)

# Final executable
add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    ${UIS}
)

# Link libraries
target_link_libraries(${PROJECT_NAME}
    Qt6::Widgets
    ${LIBPQXX_LIBRARIES}
    ${LIBSODIUM_LIBRARIES}
)