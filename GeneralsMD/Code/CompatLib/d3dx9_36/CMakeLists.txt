set (SOURCE_D3DX9
    animation.c
    core.c
    #effect.c
    #font.c
    line.c
    #main.c
    #math.c
    preshader.c
    render.c
    #shader.c
    skin.c
    sprite.c
    surface.c
    texture.c
    txc_compress_dxtn.c
    txc_fetch_dxtn.c
    util.c
    volume.c
    xfile.c
)

list(TRANSFORM SOURCE_D3DX9 PREPEND "Source/")

add_library(d3dx9 STATIC ${SOURCE_D3DX9})

target_include_directories(d3dx9 PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/Include
)

target_include_directories(d3dx9 PUBLIC 
    ${CMAKE_SOURCE_DIR}/extern/dxvk-native/usr/include/dxvk
)