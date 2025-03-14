
find_package(GTest REQUIRED)

add_executable(SageTests
  FastFloatTest.cpp
)

target_link_libraries(
  SageTests
  GameEngine
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(SageTests)