set(CMAKE_INCLUDE_CURRENT_DIR ON)

file(GLOB_RECURSE SOURCES src *.cpp)

add_executable(AsrHttp ${SOURCES})

target_include_directories(AsrHttp PRIVATE src)
target_link_libraries(AsrHttp PRIVATE oatpp)

set_target_properties(AsrHttp PROPERTIES
        CXX_STANDARD 11
        CXX_EXTENSIONS OFF
        CXX_STANDARD_REQUIRED ON
)

