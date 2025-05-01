file(GLOB_RECURSE TEST_SRC CONFIGURE_DEPENDS mocks/*.cpp)
add_library(test_user STATIC ${TEST_SRC})
target_include_directories(test_user PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(test_user domain_user)