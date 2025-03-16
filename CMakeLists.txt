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
option(SAGE_USE_UBSAN "Use UBSan" OFF)
option(SAGE_USE_ASAN "Use ASan" OFF)
option(SAGE_USE_STDFS "Use std::filesystem" ON)
cmake_dependent_option(SAGE_USE_OPENAL "Use OpenAL" OFF "SAGE_USE_FFMPEG" OFF)
cmake_dependent_option(SAGE_USE_BINK "Use Bink Video Library" ON "WIN32" OFF)
cmake_dependent_option(SAGE_USE_DX8 "Use DirectX 8" ON "WIN32" OFF)

message(STATUS "Building Generals: ${SAGE_BUILD_GENERALS}")
message(STATUS "Building Zero Hour: ${SAGE_BUILD_ZEROHOUR}")
message(STATUS "Building Tools: ${SAGE_BUILD_TOOLS}")
message(STATUS "Building Tests: ${SAGE_BUILD_TESTS}")
message(STATUS "Using Bink: ${SAGE_USE_BINK}")
message(STATUS "Using GameSpy: ${SAGE_USE_GAMESPY}")
message(STATUS "Using Miles: ${SAGE_USE_MILES}")
message(STATUS "Using DirectX 8: ${SAGE_USE_DX8}")
message(STATUS "Using ZLib: ${SAGE_USE_ZLIB}")
message(STATUS "Using LZH-Light: ${SAGE_USE_LZHL}")
message(STATUS "Using FFmpeg: ${SAGE_USE_FFMPEG}")
message(STATUS "Using OpenAL: ${SAGE_USE_OPENAL}")
message(STATUS "Using GLM: ${SAGE_USE_GLM}")
message(STATUS "Using UBSan: ${SAGE_USE_UBSAN}")
message(STATUS "Using ASan: ${SAGE_USE_ASAN}")
message(STATUS "Using SDL3: ${SAGE_USE_SDL3}")
message(STATUS "Using STDFS: ${SAGE_USE_STDFS}")

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
    GIT_REPOSITORY https://github.com/Fighter19/miles-sdk-stub.git
    GIT_TAG a4a0da4d6083f71357080e4a8021ad77ba2c62bf)
  FetchContent_MakeAvailable(miles)
endif()

if(SAGE_USE_DX8)
  FetchContent_Declare(
    dx8
    GIT_REPOSITORY https://github.com/TheSuperHackers/min-dx8-sdk.git
    GIT_TAG        efd4b6d9f4915bd38aaa2184d49f404a18be59a3)
  FetchContent_MakeAvailable(dx8)
else()
  FetchContent_Declare(
    dxvk
    URL        https://github.com/doitsujin/dxvk/releases/download/v2.6/dxvk-native-2.6-steamrt-sniper.tar.gz
  )
  FetchContent_MakeAvailable(dxvk)
  message(STATUS "DXVK source directory: ${dxvk_SOURCE_DIR}")
endif()

if(SAGE_USE_LZHL)
  FetchContent_Declare(
    liblzhl
    GIT_REPOSITORY https://github.com/feliwir/liblzhl
    GIT_TAG fd7c70c4bb96e7a4a682f574e788c499f00a0b8d)
  FetchContent_MakeAvailable(liblzhl)
endif()

if(NOT MSVC)
  add_compile_options(-Wuninitialized)
endif()

if(SAGE_BUILD_GENERALS)
  add_subdirectory(Generals/Code)
endif()

if(SAGE_BUILD_ZEROHOUR)
  add_subdirectory(GeneralsMD/Code)
endif()

include(CPack)