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
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/SDL3Device/GameClient/SDL3Keyboard.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/SDL3Device/GameClient/SDL3Mouse.cpp
    )
    find_package(SDL3 CONFIG REQUIRED)
    find_package(SDL3_image CONFIG REQUIRED)
    target_link_libraries(GameEngineDevice PUBLIC SDL3::SDL3 SDL3_image::SDL3_image)
    target_compile_definitions(GameEngineDevice PUBLIC SAGE_USE_SDL3)
endif()

if(SAGE_USE_STDFS)
    target_sources(GameEngineDevice PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/StdDevice/Common/StdBIGFile.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/StdDevice/Common/StdBIGFileSystem.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/StdDevice/Common/StdLocalFile.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/StdDevice/Common/StdLocalFileSystem.cpp
    )
    target_compile_definitions(GameEngineDevice PUBLIC SAGE_USE_STDFS)
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

set(SHADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Source/W3DDevice/GameClient/Shaders)
set(SHADER_OUTPUT_DIR ${CMAKE_BINARY_DIR}/shaders)

file(MAKE_DIRECTORY ${SHADER_OUTPUT_DIR})

set(SHADERS
    shadow.vert.glsl
    shadow.frag.glsl
    simple2d_viewport.vert.glsl
    simple2d_textured.frag.glsl
)

set(SPIRV_SHADERS)
set(SPIRV_SHADERS_HEADERS)

foreach(SHADER ${SHADERS})
    string(REPLACE ".glsl" ".spv" SPIRV_NAME ${SHADER})
    set(SPIRV_OUT ${SHADER_OUTPUT_DIR}/${SPIRV_NAME})
    add_custom_command(
        OUTPUT ${SPIRV_OUT}
        COMMAND glslang -V ${SHADER_DIR}/${SHADER} -o ${SPIRV_OUT}
        DEPENDS ${SHADER_DIR}/${SHADER}
        COMMENT "Compiling GLSL shader ${SHADER} to SPIR-V"
        VERBATIM
    )
    list(APPEND SPIRV_SHADERS ${SPIRV_OUT})
    # Also generate header files for embedding shaders
    string(REPLACE ".glsl" ".h" HEADER_NAME ${SHADER})
    set(HEADER_OUT ${SHADER_OUTPUT_DIR}/${HEADER_NAME})
    add_custom_command(
        OUTPUT ${HEADER_OUT}
        WORKING_DIRECTORY ${SHADER_OUTPUT_DIR}
        COMMAND xxd -i ${SPIRV_NAME} > ${HEADER_OUT}
        DEPENDS ${SPIRV_OUT}
        COMMENT "Generating header file ${HEADER_NAME} from SPIR-V shader"
        VERBATIM
    )
    list(APPEND SPIRV_SHADERS_HEADERS ${HEADER_OUT})
endforeach()

add_custom_target(ShadersSPV ALL DEPENDS ${SPIRV_SHADERS} ${SPIRV_SHADERS_HEADERS})
add_dependencies(GameEngineDevice ShadersSPV)
