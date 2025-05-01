file(GLOB_RECURSE INFRA_SRC CONFIGURE_DEPENDS
    config/*.cpp
    crypto/*.cpp
    db/*.cpp
    persistence/*.cpp
)

add_library(infra_user STATIC ${INFRA_SRC})
target_include_directories(infra_user PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(infra_user domain_user)