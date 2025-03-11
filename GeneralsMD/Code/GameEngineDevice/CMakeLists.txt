set(SOURCES_MILES )
if (SAGE_USE_MILES)
    set(SOURCES_MILES
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/MilesAudioDevice/MilesAudioManager.cpp
    )
endif()

set(SOURCES_BINK )
if(SAGE_USE_BINK)
    set(SOURCES_BINK
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/VideoDevice/Bink/BinkVideoPlayer.cpp
    )
endif()

file(GLOB_RECURSE SOURCES_W3DDevice
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/W3DDevice/*.cpp
)

if(WIN32)
file(GLOB_RECURSE SOURCES_WIN32Device
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Win32Device/*.cpp
)
endif()

if(UNIX)
    file(GLOB_RECURSE SOURCES_UNIXDevice
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/UnixDevice/*.cpp
    )
endif()

add_library(GameEngineDevice STATIC 
    ${SOURCES_W3DDevice} 
    ${SOURCES_WIN32Device}
    ${SOURCES_UNIXDevice}
    ${SOURCES_MILES}
    ${SOURCES_BINK}
)

target_include_directories(GameEngineDevice PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/../Libraries/Include
)

target_include_directories(GameEngineDevice PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../Main
)

target_link_libraries(GameEngineDevice PUBLIC WW3D2 GameEngine BrowserDispatch)

if (SAGE_USE_MILES)
    # TODO: this shouldnt have to be public. Fix Win32GameEngine.h
    target_link_libraries(GameEngineDevice PUBLIC milesstub)
endif()

if (SAGE_USE_BINK)
    # TODO: this shouldnt have to be public. Fix Win32GameEngine.h
    target_link_libraries(GameEngineDevice PUBLIC binkstub)
endif()