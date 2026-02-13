cmake_minimum_required(VERSION 3.23)
project(
  SAGE
  VERSION 0.1
  LANGUAGES C CXX)

# Required for filesystem
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CMakeDependentOption)
option(SAGE_BUILD_GENERALS "Build the C&C Generals game" OFF)
option(SAGE_BUILD_ZEROHOUR "Build the C&C Generals Zero Hour game" ON)
option(SAGE_BUILD_TOOLS "Build the tools" ON)
option(SAGE_BUILD_TESTS "Build the tests" OFF)
option(SAGE_USE_GAMESPY "Use GameSpy" ON)
option(SAGE_USE_MILES "Use Miles Sound System" ON)
option(SAGE_USE_ZLIB "Use ZLib" ON)
option(SAGE_USE_LZHL "Use LZH-Light" ON)
option(SAGE_USE_GLM "Use GLM math library" OFF)
option(SAGE_USE_SDL3 "Use SDL3" OFF)
option(SAGE_USE_FFMPEG "Use FFmpeg" ON)
option(SAGE_USE_STDFS "Use std::filesystem" ON)
cmake_dependent_option(SAGE_USE_OPENAL "Use OpenAL" OFF "SAGE_USE_FFMPEG" OFF)
cmake_dependent_option(SAGE_USE_BINK "Use Bink Video Library" ON "WIN32" OFF)
cmake_dependent_option(SAGE_USE_DX8 "Use DirectX 8" ON "WIN32" OFF)
cmake_dependent_option(SAGE_USE_WINED3D8 "Use WineD3D DirectX 8" ON "EMSCRIPTEN" OFF)
cmake_dependent_option(SAGE_USE_FREETYPE "Use FreeType" ON "NOT WIN32" OFF)
cmake_dependent_option(SAGE_USE_FONTCONFIG "Use FontConfig" ON "NOT WIN32" OFF)
option(SAGE_USE_UBSAN "Use UBSan" OFF)
option(SAGE_USE_ASAN "Use ASan" OFF)

message(STATUS "Building Generals: ${SAGE_BUILD_GENERALS}")
message(STATUS "Building Zero Hour: ${SAGE_BUILD_ZEROHOUR}")
message(STATUS "Building Tools: ${SAGE_BUILD_TOOLS}")
message(STATUS "Building Tests: ${SAGE_BUILD_TESTS}")
message(STATUS "Using Bink: ${SAGE_USE_BINK}")
message(STATUS "Using GameSpy: ${SAGE_USE_GAMESPY}")
message(STATUS "Using Miles: ${SAGE_USE_MILES}")
message(STATUS "Using DirectX 8: ${SAGE_USE_DX8}")
message(STATUS "Using WineD3D DirectX 8: ${SAGE_USE_WINED3D8}")
message(STATUS "Using ZLib: ${SAGE_USE_ZLIB}")
message(STATUS "Using LZH-Light: ${SAGE_USE_LZHL}")
message(STATUS "Using FFmpeg: ${SAGE_USE_FFMPEG}")
message(STATUS "Using OpenAL: ${SAGE_USE_OPENAL}")
message(STATUS "Using GLM: ${SAGE_USE_GLM}")
message(STATUS "Using SDL3: ${SAGE_USE_SDL3}")
message(STATUS "Using STDFS: ${SAGE_USE_STDFS}")
message(STATUS "Using FreeType: ${SAGE_USE_FREETYPE}")
message(STATUS "Using FontConfig: ${SAGE_USE_FONTCONFIG}")
message(STATUS "Using UBSan: ${SAGE_USE_UBSAN}")
message(STATUS "Using ASan: ${SAGE_USE_ASAN}")

if(EMSCRIPTEN)
add_compile_options(-pthread)
add_link_options(-pthread)

# add_compile_options(-fsanitize=address)
# add_link_options(-fsanitize=address)

add_compile_options("SHELL:-s USE_PTHREADS")
add_link_options("SHELL:-s USE_PTHREADS -s INITIAL_MEMORY=512MB")
add_link_options("SHELL:-sINVOKE_RUN=0")
add_link_options("SHELL:-sFORCE_FILESYSTEM=1")
# add_link_options("SHELL:-sALLOW_MEMORY_GROWTH=1")
#-sERROR_ON_WASM_CHANGES_AFTER_LINK -sWASM_BIGINT -O1
add_link_options("SHELL:-sWASM_BIGINT")
add_link_options("SHELL:-sASSERTIONS=2")
add_link_options("-lworkerfs.js")
add_link_options("-lidbfs.js")
# This is slow, but doing so will prevent deadlocks, when not using sASSERTIONS=2
#add_link_options("SHELL:-sPROXY_TO_PTHREAD=1")
# This is important! It will crash after some time without it, without any error message.
add_link_options("SHELL:-sSTACK_SIZE=1MB")
# add_link_options("SHELL:-sSAFE_HEAP=1")
add_link_options("SHELL:-sFULL_ES2=1")

# Add lto for emscripten builds
add_compile_options(-O1)
add_link_options(-O1 -g0 -gsource-map)
#add_link_options(-O1 -g0)
endif()

if(SAGE_USE_ASAN)
  if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fsanitize=address")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /fsanitize=address")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
  endif()
endif()

if(SAGE_USE_UBSAN)
  if(MSVC)
    message(WARNING "UBSan is not supported on MSVC")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=undefined")
  endif()
endif()

if(SAGE_BUILD_TESTS)
  enable_testing()
endif()

if(WIN32)
  add_definitions(-D_WINDOWS)
  add_definitions(-DWINVER=0x400)
  add_definitions(-D_WIN32_WINNT=0x0400)
elseif(UNIX)
  if(NOT APPLE)
    add_definitions(-D_LINUX)
  else()
    add_definitions(-D_MACOSX)
  endif()
  add_definitions(-D_UNIX)
endif()

include(FetchContent)
if(SAGE_USE_BINK)
  FetchContent_Declare(
    bink
    GIT_REPOSITORY https://github.com/TheSuperHackers/bink-sdk-stub.git
    GIT_TAG f554b7fa68c97d4e6b607d535e726ef37622fe65)
  FetchContent_MakeAvailable(bink)
endif()

if(SAGE_USE_GAMESPY)
  set(GS_OPENSSL FALSE)
  FetchContent_Declare(
      gamespy
      GIT_REPOSITORY https://github.com/feliwir/GamespySDK.git
      GIT_TAG        582c79105aa851c5aa847f638722f61195b79c9b
  )
  FetchContent_MakeAvailable(gamespy)
endif()

if(SAGE_USE_MILES)
  FetchContent_Declare(
    miles
    GIT_REPOSITORY https://github.com/TheSuperHackers/miles-sdk-stub.git
    GIT_TAG 0fef646a85c822475d55f19e3ca185263fb4a967)
  FetchContent_MakeAvailable(miles)
endif()

if(SAGE_USE_DX8)
  FetchContent_Declare(
    dx8
    GIT_REPOSITORY https://github.com/TheSuperHackers/min-dx8-sdk.git
    GIT_TAG        20d31185872e1304e0573f7f4885ae11e50670d3)
  FetchContent_MakeAvailable(dx8)
else()
  FetchContent_Declare(
    dxvk
    URL        https://github.com/doitsujin/dxvk/releases/download/v2.6/dxvk-native-2.6-steamrt-sniper.tar.gz
  )
  FetchContent_MakeAvailable(dxvk)
  message(STATUS "DXVK source directory: ${dxvk_SOURCE_DIR}")
endif()

if(SAGE_USE_WINED3D8)
  FetchContent_Declare(
    wined3d
    GIT_REPOSITORY https://github.com/fighter19/d3d8-native.git
    GIT_TAG        d460058387a050eda6f06bb8872bff69649e9468)
  FetchContent_MakeAvailable(wined3d)
endif()

if(SAGE_USE_LZHL)
  FetchContent_Declare(
    liblzhl
    GIT_REPOSITORY https://github.com/feliwir/liblzhl
    GIT_TAG fd7c70c4bb96e7a4a682f574e788c499f00a0b8d)
  FetchContent_MakeAvailable(liblzhl)
endif()

if(NOT MSVC)
  add_compile_options(-Wuninitialized -Werror=vexing-parse)
  set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG" )
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  add_compile_options(-ffp-model=strict)
endif()

if(SAGE_BUILD_GENERALS)
  add_subdirectory(Generals/Code)
endif()

if(SAGE_BUILD_ZEROHOUR)
  add_subdirectory(GeneralsMD/Code)
endif()

include(CPack)