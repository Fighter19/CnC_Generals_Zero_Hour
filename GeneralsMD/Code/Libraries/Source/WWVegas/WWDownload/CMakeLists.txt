set(SOURCES_WWDOWNLOAD Download.cpp FTP.CPP registry.cpp urlBuilder.cpp)

add_library(WWDownload STATIC ${SOURCES_WWDOWNLOAD})

target_include_directories(WWDownload PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

if (UNIX)
    target_compile_definitions(WWDownload PUBLIC
        -D_UNIX
    )
    target_link_libraries(WWDownload PUBLIC CompatLib)
endif()