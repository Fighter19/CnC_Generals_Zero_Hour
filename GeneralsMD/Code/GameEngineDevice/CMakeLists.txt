file(GLOB_RECURSE SOURCES_W3DDevice
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/W3DDevice/*.cpp
)

add_library(GameEngineDevice STATIC 
    ${SOURCES_W3DDevice} 
    ${SOURCES_WIN32Device}
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
    target_sources(GameEngineDevice PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/MilesAudioDevice/MilesAudioCache.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/MilesAudioDevice/MilesAudioManager.cpp
    )
    target_link_libraries(GameEngineDevice PUBLIC milesstub)
    target_compile_definitions(GameEngineDevice PUBLIC SAGE_USE_MILES)
endif()

if (SAGE_USE_OPENAL)
    target_sources(GameEngineDevice PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/OpenALAudioDevice/OpenALAudioCache.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/OpenALAudioDevice/OpenALAudioManager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/OpenALAudioDevice/OpenALAudioStream.cpp
    )
    find_package(OpenAL CONFIG REQUIRED)
    target_link_libraries(GameEngineDevice PUBLIC OpenAL::OpenAL)
    target_compile_definitions(GameEngineDevice PUBLIC SAGE_USE_OPENAL)
endif()

if(SAGE_USE_BINK)
    target_sources(GameEngineDevice PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/VideoDevice/Bink/BinkVideoPlayer.cpp
    )
    target_link_libraries(GameEngineDevice PUBLIC binkstub)
    target_compile_definitions(GameEngineDevice PUBLIC SAGE_USE_BINK)
endif()

if(SAGE_USE_FFMPEG)
    target_sources(GameEngineDevice PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/VideoDevice/FFmpeg/FFmpegFile.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/VideoDevice/FFmpeg/FFmpegVideoPlayer.cpp
    )
    find_package(FFMPEG REQUIRED)
    target_include_directories(GameEngineDevice PRIVATE ${FFMPEG_INCLUDE_DIRS})
    target_link_libraries(GameEngineDevice PRIVATE ${FFMPEG_LIBRARIES})
    target_compile_definitions(GameEngineDevice PUBLIC SAGE_USE_FFMPEG)
endif()

if(SAGE_USE_SDL3)
    target_sources(GameEngineDevice PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/SDL3Device/Common/SDL3CDManager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/SDL3Device/Common/SDL3GameEngine.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/SDL3Device/Common/SDL3IMEManager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/SDL3Device/Common/SDL3OSDisplay.cpp
    )
    find_package(SDL3 REQUIRED)
    target_link_libraries(GameEngineDevice PUBLIC SDL3::SDL3)
    target_compile_definitions(GameEngineDevice PUBLIC SAGE_USE_SDL3)
endif()

if(WIN32)
    target_sources(GameEngineDevice PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/Win32Device/Common/Win32BIGFile.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/Win32Device/Common/Win32BIGFileSystem.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/Win32Device/Common/Win32CDManager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/Win32Device/Common/Win32GameEngine.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/Win32Device/Common/Win32LocalFile.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/Win32Device/Common/Win32LocalFileSystem.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/Win32Device/Common/Win32OSDisplay.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/Win32Device/GameClient/Win32DIKeyboard.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/Win32Device/GameClient/Win32DIMouse.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/Win32Device/GameClient/Win32Mouse.cpp
    )
endif()