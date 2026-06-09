# Target setup
add_library(c_ww3d2)

target_sources(
  c_ww3d2
  PRIVATE aabtree.cpp
          aabtree.h
          aabtreebuilder.cpp
          aabtreebuilder.h
          agg_def.cpp
          agg_def.h
          animobj.cpp
          animobj.h
          assetmgr.cpp
          assetmgr.h
          bitmaphandler.cpp
          bitmaphandler.h
          bmp2d.cpp
          bmp2d.h
          boxrobj.cpp
          boxrobj.h
          bw_render.cpp
          bw_render.h
          bwrender.cpp
          bwrender.h
          camera.cpp
          camera.h
          classid.h
          collect.cpp
          collect.h
          colorspace.h
          coltest.cpp
          coltest.h
          coltype.h
          composite.cpp
          composite.h
          dazzle.cpp
          dazzle.h
          ddsfile.cpp
          ddsfile.h
          decalmsh.cpp
          decalmsh.h
          decalsys.cpp
          decalsys.h
          distlod.cpp
          distlod.h
          dllist.h
          dx8caps.cpp
          dx8caps.h
          dx8fvf.cpp
          dx8fvf.h
          dx8indexbuffer.cpp
          dx8indexbuffer.h
          dx8list.h
          dx8polygonrenderer.cpp
          dx8polygonrenderer.h
          dx8renderer.cpp
          dx8renderer.h
          dx8texman.cpp
          dx8texman.h
          dx8vertexbuffer.cpp
          dx8vertexbuffer.h
          dx8webbrowser.cpp
          dx8webbrowser.h
          dx8wrapper.cpp
          dx8wrapper.h
          dynamesh.cpp
          dynamesh.h
          font3d.cpp
          font3d.h
          formconv.cpp
          formconv.h
          hanim.cpp
          hanim.h
          hanimmgr.cpp
          hanimmgr.h
          hcanim.cpp
          hcanim.h
          hlod.cpp
          hlod.h
          hmdldef.cpp
          hmdldef.h
          hmorphanim.cpp
          hmorphanim.h
          hrawanim.cpp
          hrawanim.h
          htree.cpp
          htree.h
          htreemgr.cpp
          htreemgr.h
          intersec.cpp
          intersec.h
          intersec.inl
          inttest.h
          layer.cpp
          layer.h
          light.cpp
          light.h
          lightenvironment.cpp
          lightenvironment.h
          line3d.cpp
          line3d.h
          mapper.cpp
          mapper.h
          matinfo.cpp
          matinfo.h
          matpass.cpp
          matpass.h
          matrixmapper.cpp
          matrixmapper.h
          mesh.cpp
          mesh.h
          meshbuild.cpp
          meshbuild.h
          meshdam.cpp
          meshdam.h
          meshgeometry.cpp
          meshgeometry.h
          meshmatdesc.cpp
          meshmatdesc.h
          meshmdl.cpp
          meshmdl.h
          meshmdlio.cpp
          metalmap.cpp
          metalmap.h
          missingtexture.cpp
          missingtexture.h
          motchan.cpp
          motchan.h
          nullrobj.cpp
          nullrobj.h
          part_buf.cpp
          part_buf.h
          part_emt.cpp
          part_emt.h
          part_ldr.cpp
          part_ldr.h
          pivot.cpp
          pivot.h
          pointgr.cpp
          pointgr.h
          polyinfo.cpp
          polyinfo.h
          predlod.cpp
          predlod.h
          prim_anim.cpp
          prim_anim.h
          projector.cpp
          projector.h
          proto.cpp
          proto.h
          proxy.h
          rddesc.h
          render2d.cpp
          render2d.h
          render2dsentence.cpp
          render2dsentence.h
          renderobjectrecycler.cpp
          renderobjectrecycler.h
          rendobj.cpp
          rendobj.h
          rinfo.cpp
          rinfo.h
          ringobj.cpp
          ringobj.h
          robjlist.h
          scene.cpp
          scene.h
          segline.cpp
          segline.h
          seglinerenderer.cpp
          seglinerenderer.h
          shader.cpp
          shader.h
          shattersystem.cpp
          shattersystem.h
          snappts.cpp
          snappts.h
          sortingrenderer.cpp
          sortingrenderer.h
          # soundrobj.cpp soundrobj.h
          sphereobj.cpp
          sphereobj.h
          # sr_util.cpp sr_util.h
          statistics.cpp
          statistics.h
          streak.cpp
          streak.h
          streakrender.cpp
          streakrender.h
          stripoptimizer.cpp
          stripoptimizer.h
          surfaceclass.cpp
          surfaceclass.h
          texfcach.cpp
          texfcach.h
          texproject.cpp
          texproject.h
          textdraw.cpp
          textdraw.h
          texture.cpp
          texture.h
          texturefile.cpp
          texturefile.h
          textureloader.cpp
          textureloader.h
          texturethumbnail.cpp
          texturethumbnail.h
          txt.cpp
          txt.h
          txt2d.cpp
          txt2d.h
          vertmaterial.cpp
          vertmaterial.h
          visrasterizer.cpp
          visrasterizer.h
          w3d_dep.cpp
          w3d_dep.h
          w3d_file.h
          w3d_obsolete.h
          w3d_util.cpp
          w3d_util.h
          w3derr.h
          w3dexclusionlist.cpp
          w3dexclusionlist.h
          ww3d.cpp
          ww3d.h
          ww3dformat.cpp
          ww3dformat.h
          ww3dids.h
          ww3dtrig.h)

if(WIN32)
  target_sources(c_ww3d2 PRIVATE framgrab.cpp framgrab.h)
endif()

set_target_properties(c_ww3d2 PROPERTIES OUTPUT_NAME "WW3D2")

target_include_directories(c_ww3d2 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(c_ww3d2 PUBLIC c_wwlib c_wwmath c_wwsaveload)

# DXVK
find_package(DXVK CONFIG REQUIRED)
target_link_libraries(c_ww3d2 PUBLIC DXVK::d3d8)

# SDL3
find_package(SDL3 CONFIG REQUIRED)
target_link_libraries(c_ww3d2 PUBLIC SDL3::SDL3)

# GLM
find_package(glm CONFIG REQUIRED)
target_link_libraries(c_ww3d2 PRIVATE glm::glm)


if(SAGE_USE_FONTCONFIG)
  find_package(Fontconfig REQUIRED)
  target_link_libraries(c_ww3d2 PRIVATE Fontconfig::Fontconfig)
  target_compile_definitions(c_ww3d2 PUBLIC SAGE_USE_FONTCONFIG)
endif()

if(SAGE_USE_FREETYPE)
    find_package(freetype QUIET)
    if (NOT freetype_FOUND)
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(freetype REQUIRED IMPORTED_TARGET freetype2)
        add_library(Freetype::Freetype ALIAS PkgConfig::freetype)
    endif()
    target_link_libraries(c_ww3d2 PRIVATE
        Freetype::Freetype
    )
  target_compile_definitions(c_ww3d2 PUBLIC SAGE_USE_FREETYPE)
endif()
