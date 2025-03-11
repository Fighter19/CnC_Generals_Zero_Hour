cmake_minimum_required(VERSION 3.23)
project(
  SAGE
  VERSION 0.1
  LANGUAGES C CXX)

# Required for filesystem
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(SAGE_BUILD_GENERALS "Build the C&C Generals game" OFF)
option(SAGE_BUILD_ZEROHOUR "Build the C&C Generals Zero Hour game" ON)
option(SAGE_BUILD_TOOLS "Build the tools" ON)
option(SAGE_USE_GAMESPY "Use GameSpy" ON)
option(SAGE_USE_MILES "Use Miles Sound System" ON)
option(SAGE_USE_ZLIB "Use ZLib" ON)
option(SAGE_USE_LZHL "Use LZH-Light" ON)
option(SAGE_USE_GLM "Use GLM math library" OFF)
option(SAGE_USE_SDL3 "Use SDL3" OFF)
include(CMakeDependentOption)
cmake_dependent_option(SAGE_USE_BINK "Use Bink Video Library" ON "WIN32" OFF)
cmake_dependent_option(SAGE_USE_DX8 "Use DirectX 8" ON "WIN32" OFF)

message(STATUS "Building Generals: ${SAGE_BUILD_GENERALS}")
message(STATUS "Building Zero Hour: ${SAGE_BUILD_ZEROHOUR}")
message(STATUS "Using Bink: ${SAGE_USE_BINK}")
message(STATUS "Using GameSpy: ${SAGE_USE_GAMESPY}")
message(STATUS "Using Miles: ${SAGE_USE_MILES}")
message(STATUS "Using DirectX 8: ${SAGE_USE_DX8}")
message(STATUS "Using ZLib: ${SAGE_USE_ZLIB}")
message(STATUS "Using LZH-Light: ${SAGE_USE_LZHL}")
message(STATUS "Using GLM: ${SAGE_USE_GLM}")
message(STATUS "Using SDL3: ${SAGE_USE_SDL3}")

include_directories(${CMAKE_SOURCE_DIR}/extern/dxvk-native/usr/include/dxvk)

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
    GIT_REPOSITORY https://github.com/feliwir/miles-sdk-stub.git
    GIT_TAG 680e6cb112cbf407d5a69bc984c40bfab032ada9)
  FetchContent_MakeAvailable(miles)
endif()

if(SAGE_USE_DX8)
  FetchContent_Declare(
    dx8
    GIT_REPOSITORY https://github.com/TheSuperHackers/min-dx8-sdk.git
    GIT_TAG        efd4b6d9f4915bd38aaa2184d49f404a18be59a3)
  FetchContent_MakeAvailable(dx8)
endif()

if(SAGE_USE_LZHL)
  FetchContent_Declare(
    liblzhl
    GIT_REPOSITORY https://github.com/feliwir/liblzhl
    GIT_TAG fd7c70c4bb96e7a4a682f574e788c499f00a0b8d)
  FetchContent_MakeAvailable(liblzhl)
endif()

if(WIN32)
  add_definitions(-D_WINDOWS)
  add_definitions(-DWINVER=0x400)
  add_definitions(-D_WIN32_WINNT=0x0400)
elseif(UNIX)
  add_definitions(-D_UNIX)
endif()

if(SAGE_BUILD_GENERALS)
  add_subdirectory(Generals/Code)
endif()

if(SAGE_BUILD_ZEROHOUR)
  add_subdirectory(GeneralsMD/Code)
endif()
