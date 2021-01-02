add_library(rapidjson STATIC rapidjson_wrapper.hpp)
target_include_directories(rapidjson PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/rapidjson/include)
set_target_properties(rapidjson PROPERTIES LINKER_LANGUAGE CXX)
