file(GLOB_RECURSE APP_SRC CONFIGURE_DEPENDS user/*.cpp)
add_library(app_user STATIC ${APP_SRC})
target_include_directories(app_user PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(app_user domain_user)