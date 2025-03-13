cmake_minimum_required(VERSION 3.23)
project(ZeroHour LANGUAGES C CXX)

add_subdirectory(CompatLib)

add_subdirectory(Libraries)
add_subdirectory(GameEngine)
add_subdirectory(GameEngineDevice)

if(WIN32)
    add_executable(RTS WIN32 Main/WinMain.cpp)
    target_link_libraries(RTS GameEngine GameEngineDevice winmm Imm32 vfw32)
    install(TARGETS RTS RUNTIME_DEPENDENCIES
        PRE_EXCLUDE_REGEXES
            "api-ms-.*" # windows API
            "ext-ms-.*" # windows API
            "^hvsifiletrust\\.dll$"
            "^pdmutilities\\.dll$"
            "^wpaxholder\\.dll$"
            "^vc.*"
            "^msvcp.*"
            "^concrt.*"
        POST_EXCLUDE_REGEXES
            ".*system32/.*\.dll" # windows system dlls
        RUNTIME DESTINATION .
    )
endif()

if(SAGE_BUILD_TOOLS)
    add_subdirectory(Tools/Compress)
    add_subdirectory(Tools/CRCDiff)
endif()