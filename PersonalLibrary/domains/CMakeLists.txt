file(GLOB_RECURSE DOMAIN_SRC CONFIGURE_DEPENDS user/*.cpp)
add_library(domain_user STATIC ${DOMAIN_SRC})
target_include_directories(domain_user PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})