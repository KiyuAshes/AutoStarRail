add_compile_definitions($<$<CONFIG:Debug>:DEBUG>)

aux_source_directory(Utils/src ASR_UTILS_SOURCES)
file(GLOB_RECURSE Utils/include/* ASR_UTILS_HEADERS)
add_library(AsrUtils STATIC ${ASR_UTILS_SOURCES} ${ASR_UTILS_HEADERS})
set_property(TARGET AsrUtils PROPERTY POSITION_INDEPENDENT_CODE ON)
target_include_directories(AsrUtils PUBLIC Utils/include)
target_link_libraries(AsrUtils PUBLIC Asr3rdParty)

# Prevent visual studio treat headers as sources.
#file(GLOB_RECURSE ASR_COMMON_HEADERS ${CMAKE_SOURCE_DIR}/include/*) ${ASR_COMMON_HEADERS}
add_library(AsrCommonHeaders INTERFACE)
target_include_directories(AsrCommonHeaders INTERFACE ${CMAKE_SOURCE_DIR}/include/)
target_link_libraries(AsrUtils PUBLIC AsrCommonHeaders)

add_library(ASR_EX_PRIVATE_LIBS ALIAS AsrUtils)

if(${ASR_BUILD_TEST})
    include(GoogleTest)
    asr_add_additional_test(AdditionalTest)
    target_link_libraries(AdditionalTest PUBLIC AsrUtils Boost::url)
endif()

add_subdirectory(Core)

add_subdirectory(Plugins)

add_subdirectory(AskrHttp)

if(${ASR_BUILD_QT5_GUI})
    add_subdirectory(Qt5Gui)
endif()
