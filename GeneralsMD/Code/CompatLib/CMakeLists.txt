if (UNIX)

set(SOURCE_COMPAT
    module_compat.cpp
    string_compat.cpp
    thread_compat.cpp
    time_compat.cpp
    wchar_compat.cpp
    d3dx8_compat.cpp
    d3dx8math.cpp
)

list(TRANSFORM SOURCE_COMPAT PREPEND "Source/")

add_library(CompatLib STATIC ${SOURCE_COMPAT})

target_include_directories(CompatLib PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/Include
)

# add_subdirectory(d3dx9_36)

# set(SOURCE_DX_COMPAT
#     d3dx8_compat.cpp
# )

# list(TRANSFORM SOURCE_DX_COMPAT PREPEND "Source/")

# add_library(DXCompatLib STATIC ${SOURCE_DX_COMPAT})

# target_link_libraries(DXCompatLib PUBLIC
#     CompatLib
# )

# target_include_directories(DXCompatLib PUBLIC 
#     ${CMAKE_SOURCE_DIR}/extern/dxvk-native/usr/include/dxvk
# )

# Add an empty library and link it to libdxvk_d3d8.so.0
add_library(d3d8lib INTERFACE)
target_link_libraries(d3d8lib INTERFACE
    dxvk_d3d8
)

# Search library in dxvk-native folder
target_link_directories(d3d8lib INTERFACE
    ${CMAKE_SOURCE_DIR}/extern/dxvk-native/usr/lib
)

endif()