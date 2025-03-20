add_executable(Compress Compress.cpp)
target_link_libraries(Compress PRIVATE Compression WWLib)
install(TARGETS Compress RUNTIME DESTINATION tools)