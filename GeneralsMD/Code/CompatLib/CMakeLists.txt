# On 64-bit Windows or Unix, use our d3dx8 implementation
if (UNIX OR WIN32 AND CMAKE_SIZEOF_VOID_P EQUAL 8)
list(APPEND SOURCE_D3DX_COMPAT
    d3dx8_compat.cpp
    d3dx8math.cpp
)

list(TRANSFORM SOURCE_D3DX_COMPAT PREPEND "Source/")

add_library(d3dx8 STATIC ${SOURCE_D3DX_COMPAT})

find_package(glm REQUIRED)
target_link_libraries(d3dx8 PUBLIC glm::glm)

find_package(gli REQUIRED)
target_link_libraries(d3dx8 PUBLIC gli)

# Use the headers from the DX8 SDK
if (WIN32)
    target_link_libraries(d3dx8 PUBLIC d3d8lib)
endif()

target_include_directories(d3dx8 PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/Include
    ${dxvk_SOURCE_DIR}/include/dxvk
)

endif()

if (UNIX)

set(SOURCE_COMPAT
    module_compat.cpp
    string_compat.cpp
    thread_compat.cpp
    time_compat.cpp
    wchar_compat.cpp
)

list(TRANSFORM SOURCE_COMPAT PREPEND "Source/")

add_library(CompatLib STATIC ${SOURCE_COMPAT})

target_include_directories(CompatLib PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/Include
)

# Add an empty library and link it to libdxvk_d3d8.so.0
add_library(d3d8lib INTERFACE)
find_package(glm REQUIRED)
target_link_libraries(d3d8lib INTERFACE
    dxvk_d3d8 d3dx8 dxvk_d3d9
)

target_include_directories(d3d8lib INTERFACE
    ${dxvk_SOURCE_DIR}/include/dxvk
)

# Search library in dxvk-native folder
target_link_directories(d3d8lib INTERFACE
    ${dxvk_SOURCE_DIR}/lib
)

target_link_libraries(CompatLib PRIVATE d3d8lib)

endif()