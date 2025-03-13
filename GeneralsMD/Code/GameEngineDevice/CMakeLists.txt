file(GLOB_RECURSE SOURCES_W3DDevice
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/W3DDevice/*.cpp
)

set(SOURCES_WIN32Device )
if(WIN32)
    file(GLOB_RECURSE SOURCES_WIN32Device
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/Win32Device/*.cpp
    )
endif()

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
    )
    find_package(OpenAL REQUIRED)
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