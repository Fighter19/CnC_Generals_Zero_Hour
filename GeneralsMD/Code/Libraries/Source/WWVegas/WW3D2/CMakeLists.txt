file(GLOB_RECURSE SOURCES_WW3D2
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.CPP
)

# Exclude the following files from the build
list(REMOVE_ITEM SOURCES_WW3D2 ${CMAKE_CURRENT_SOURCE_DIR}/sr_util.cpp)
add_library(WW3D2 STATIC ${SOURCES_WW3D2})

target_link_libraries(WW3D2 PRIVATE
    WWAudio
)

target_link_libraries(WW3D2 PUBLIC
    WWMath d3d8lib
)

target_include_directories(WW3D2 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

if (UNIX)
    target_compile_definitions(WW3D2 PUBLIC
        _UNIX
    )
endif()