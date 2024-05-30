set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/Debug/plugins)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib/Debug/plugins)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/Debug/plugins)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/Release/plugins)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib/Release/plugins)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/Release/plugins)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/bin/RelWithDebInfo/plugins)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/lib/RelWithDebInfo/plugins)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/bin/RelWithDebInfo/plugins)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_BINARY_DIR}/bin/MinSizeRel/plugins)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_BINARY_DIR}/lib/MinSizeRel/plugins)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_BINARY_DIR}/bin/MinSizeRel/plugins)

add_custom_target(AsrAutoCopyPluginMetadataFile ALL)

asr_add_plugin_library(AsrAdbCapture ASR_EX_PRIVATE_LIBS)
asr_add_plugin_library(AsrAdbTouch ASR_EX_PRIVATE_LIBS)

target_link_libraries(AsrAdbCapture PRIVATE zlib)

if(WIN32)
    target_link_libraries(AsrAdbCapture PRIVATE Ws2_32)
    target_link_libraries(AsrAdbTouch PRIVATE Ws2_32)
endif()

target_include_directories(AsrAdbCapture PRIVATE ${gzip-hpp_SOURCE_DIR}/include)

if(ASR_USE_BUNDLED_BOOST)
    target_link_libraries(AsrAdbCapture PUBLIC
        Boost::process
        Boost::container_hash
        Boost::url
        Boost::asio
        Boost::pfr)
    target_link_libraries(AsrAdbTouch PUBLIC
        Boost::process
        Boost::url
        Boost::asio)
else()
    target_include_directories(AsrAdbCapture PUBLIC ${Boost_INCLUDE_DIRS})
    target_link_libraries(AsrAdbCapture PRIVATE ${Boost_LIBRARIES})

    target_include_directories(AsrAdbTouch PUBLIC ${Boost_INCLUDE_DIRS})
    target_link_libraries(AsrAdbTouch PRIVATE ${Boost_LIBRARIES})
endif()
