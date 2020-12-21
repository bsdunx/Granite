add_library(rapidjson STATIC rapidjson_wrapper.hpp)
target_include_directories(rapidjson PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/rapidjson/include)
set_target_properties(rapidjson PROPERTIES LINKER_LANGUAGE CXX)

add_library(renderdoc-app INTERFACE)
target_include_directories(renderdoc-app INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/renderdoc)
