set(SOURCES
    LoginWindow.cpp
)

set(HEADERS
    LoginWindow.hpp
)

set(UIS
    LoginWindow.ui
)

add_library(ui_login ${SOURCES} ${HEADERS} ${UIS})

target_link_libraries(ui_login
    Qt6::Widgets           # Change to Qt5::Widgets if using Qt5
    app_user
)

# Ensure the auto-generated ui_LoginWindow.h is visible to consumers
target_include_directories(ui_login
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}                   # for LoginWindow.hpp
        ${CMAKE_CURRENT_BINARY_DIR}                   # for moc_*.cpp, ui_*.h
        ${CMAKE_CURRENT_BINARY_DIR}/ui_login_autogen/include  # fallback path for ui headers
)