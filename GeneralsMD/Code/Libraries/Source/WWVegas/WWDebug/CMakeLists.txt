set(SOURCE_WWDEBUG
    wwdebug.cpp
    wwmemlog.cpp
    wwprofile.cpp
)

add_library(WWDebug STATIC ${SOURCE_WWDEBUG})

target_include_directories(WWDebug PUBLIC 
    #${CMAKE_SOURCE_DIR}/GameEngine/Include 
    ${CMAKE_CURRENT_SOURCE_DIR} 
    ${CMAKE_CURRENT_SOURCE_DIR}/../WWLib
)

if(SAGE_USE_TRACY)
    find_package(Tracy REQUIRED)
    target_link_libraries(WWDebug PUBLIC
        Tracy::TracyClient
    )
    target_compile_definitions(WWDebug PUBLIC
        -DSAGE_USE_TRACY
    )
endif()

if(UNIX)
    target_compile_definitions(WWDebug PUBLIC
        -D_UNIX
    )
    target_link_libraries(WWDebug PUBLIC CompatLib)
else()
    target_compile_definitions(WWDebug PUBLIC
        -D_WINDOWS
        -DWINVER=0x400
        -D_WIN32_WINNT=0x0400
    )
endif()
