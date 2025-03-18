file(GLOB_RECURSE SOURCES_COMMON
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Common/*.cpp
)

file(GLOB_RECURSE SOURCES_GAMECLIENT
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/GameClient/*.cpp
)

file(GLOB_RECURSE SOURCES_GAMELOGIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/GameLogic/*.cpp
)

file(GLOB_RECURSE SOURCES_GAMENETWORK
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/GameNetwork/*.cpp
)

list(REMOVE_ITEM SOURCES_GAMENETWORK 
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/GameNetwork/GameSpyChat.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/GameNetwork/GameSpyGameInfo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/GameNetwork/GameSpyGP.cpp
)

if(UNIX)
# Currently using a header-only stub for UNIX, excluded from build otherwise,
# as it relies on OLE
    list(REMOVE_ITEM SOURCES_GAMENETWORK ${CMAKE_CURRENT_SOURCE_DIR}/Source/GameNetwork/WOLBrowser/WebBrowser.cpp)
endif()

add_library(GameEngine STATIC ${SOURCES_COMMON} ${SOURCES_GAMECLIENT} ${SOURCES_GAMELOGIC} ${SOURCES_GAMENETWORK})

target_include_directories(GameEngine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/Precompiled
    ${CMAKE_CURRENT_SOURCE_DIR}/../Libraries/Include
)

target_include_directories(GameEngine PRIVATE
    # Used in Thing.h via WWMath
    ${CMAKE_CURRENT_SOURCE_DIR}/../Libraries/Source/WWVegas
)

target_link_libraries(GameEngine PUBLIC WWLib WWMath WWDownload)
target_link_libraries(GameEngine PRIVATE Compression)

if(WIN32)
    target_link_libraries(GameEngine PRIVATE d3d8lib gamespy BrowserDispatch)
else()
    target_include_directories(GameEngine PRIVATE
        ${dxvk_SOURCE_DIR}/include/dxvk
    )
    target_link_libraries(GameEngine PRIVATE gamespy)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")    
else()
    target_compile_options(GameEngine PRIVATE -Wno-invalid-offsetof)
endif()

target_precompile_headers(GameEngine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Include/Precompiled/PreRTS.h)