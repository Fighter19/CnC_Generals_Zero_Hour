cmake_minimum_required(VERSION 3.23)
project(ZeroHour LANGUAGES C CXX)

add_subdirectory(CompatLib)

add_subdirectory(Libraries)
add_subdirectory(GameEngine)
add_subdirectory(GameEngineDevice)

if(WIN32)
    add_executable(RTS WIN32 Main/WinMain.cpp)
    target_link_libraries(RTS GameEngine GameEngineDevice winmm)
elseif(SAGE_USE_SDL3)
    add_executable(RTS Main/SDL3Main.cpp)

    target_include_directories(RTS PRIVATE
        ${CMAKE_SOURCE_DIR}/extern/dxvk-native/usr/include/dxvk
    )

    find_package(SDL3 REQUIRED)
    target_link_libraries(RTS GameEngine GameEngineDevice SDL3::SDL3)
endif()

if(SAGE_BUILD_TOOLS)
    add_subdirectory(Tools/Compress)
endif()