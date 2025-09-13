include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.16.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(SageTests
  TestCRC.cpp
  TestMain.cpp
  TestString.cpp
  FastFloatTest.cpp
)

target_link_libraries(
  SageTests
  GameEngine
  GameEngineDevice
  GTest::gtest
)

if(WIN32)
  target_link_libraries(SageTests winmm Imm32 vfw32)
endif()

include(GoogleTest)
gtest_discover_tests(SageTests)

add_executable(TestGraphics
  TestGraphics.cpp
)

target_link_libraries(TestGraphics
  GameEngine
  GameEngineDevice
)

if(WIN32)
  target_link_libraries(TestGraphics winmm Imm32 vfw32)
endif()
