# Target setup
add_library(g_gameenginedevice)

target_sources(
  g_gameenginedevice
  PRIVATE Include/W3DDevice/Common/W3DConvert.h
          Include/W3DDevice/Common/W3DFunctionLexicon.h
          Include/W3DDevice/Common/W3DModuleFactory.h
          Include/W3DDevice/Common/W3DRadar.h
          Include/W3DDevice/Common/W3DThingFactory.h
          Include/W3DDevice/GameClient/HeightMap.h
          Include/W3DDevice/GameClient/Module/W3DDebrisDraw.h
          Include/W3DDevice/GameClient/Module/W3DDefaultDraw.h
          Include/W3DDevice/GameClient/Module/W3DDependencyModelDraw.h
          Include/W3DDevice/GameClient/Module/W3DLaserDraw.h
          Include/W3DDevice/GameClient/Module/W3DModelDraw.h
          Include/W3DDevice/GameClient/Module/W3DOverlordTankDraw.h
          Include/W3DDevice/GameClient/Module/W3DPoliceCarDraw.h
          Include/W3DDevice/GameClient/Module/W3DProjectileStreamDraw.h
          Include/W3DDevice/GameClient/Module/W3DRopeDraw.h
          Include/W3DDevice/GameClient/Module/W3DScienceModelDraw.h
          Include/W3DDevice/GameClient/Module/W3DSupplyDraw.h
          Include/W3DDevice/GameClient/Module/W3DTankDraw.h
          Include/W3DDevice/GameClient/Module/W3DTankTruckDraw.h
          Include/W3DDevice/GameClient/Module/W3DTracerDraw.h
          Include/W3DDevice/GameClient/Module/W3DTruckDraw.h
          Include/W3DDevice/GameClient/TerrainTex.h
          Include/W3DDevice/GameClient/TileData.h
          Include/W3DDevice/GameClient/W3DAssetManagerExposed.h
          Include/W3DDevice/GameClient/W3DAssetManager.h
          Include/W3DDevice/GameClient/W3DBibBuffer.h
          Include/W3DDevice/GameClient/W3DBridgeBuffer.h
          Include/W3DDevice/GameClient/W3DBufferManager.h
          Include/W3DDevice/GameClient/W3DCustomEdging.h
          Include/W3DDevice/GameClient/W3DCustomScene.h
          Include/W3DDevice/GameClient/W3DDebugDisplay.h
          Include/W3DDevice/GameClient/W3DDebugIcons.h
          Include/W3DDevice/GameClient/W3DDisplay.h
          Include/W3DDevice/GameClient/W3DDisplayString.h
          Include/W3DDevice/GameClient/W3DDisplayStringManager.h
          Include/W3DDevice/GameClient/W3DDynamicLight.h
          Include/W3DDevice/GameClient/W3DFileSystem.h
          Include/W3DDevice/GameClient/W3DGadget.h
          Include/W3DDevice/GameClient/W3DGameClient.h
          Include/W3DDevice/GameClient/W3DGameFont.h
          Include/W3DDevice/GameClient/W3DGameWindow.h
          Include/W3DDevice/GameClient/W3DGameWindowManager.h
          Include/W3DDevice/GameClient/W3DGranny.h
          Include/W3DDevice/GameClient/W3DGUICallbacks.h
          Include/W3DDevice/GameClient/W3DInGameUI.h
          Include/W3DDevice/GameClient/W3DMirror.h
          Include/W3DDevice/GameClient/W3DMouse.h
          Include/W3DDevice/GameClient/W3DParticleSys.h
          Include/W3DDevice/GameClient/W3DPoly.h
          Include/W3DDevice/GameClient/W3DProjectedShadow.h
          Include/W3DDevice/GameClient/W3DRoadBuffer.h
          Include/W3DDevice/GameClient/W3DScene.h
          Include/W3DDevice/GameClient/W3DShaderManager.h
          Include/W3DDevice/GameClient/W3DShadow.h
          Include/W3DDevice/GameClient/W3DShroud.h
          Include/W3DDevice/GameClient/W3DStatusCircle.h
          Include/W3DDevice/GameClient/W3DTerrainTracks.h
          Include/W3DDevice/GameClient/W3DTerrainVisual.h
          Include/W3DDevice/GameClient/W3DTreeBuffer.h
          Include/W3DDevice/GameClient/W3DVideobuffer.h
          Include/W3DDevice/GameClient/W3DView.h
          Include/W3DDevice/GameClient/W3DVolumetricShadow.h
          Include/W3DDevice/GameClient/W3DWater.h
          Include/W3DDevice/GameClient/W3DWaterTracks.h
          Include/W3DDevice/GameClient/W3DWaypointBuffer.h
          Include/W3DDevice/GameClient/W3DWebBrowser.h
          Include/W3DDevice/GameClient/WorldHeightMap.h
          Include/W3DDevice/GameLogic/W3DGameLogic.h
          Include/W3DDevice/GameLogic/W3DGhostObject.h
          Include/W3DDevice/GameLogic/W3DTerrainLogic.h
          Source/W3DDevice/Common/System/W3DFunctionLexicon.cpp
          Source/W3DDevice/Common/System/W3DRadar.cpp
          Source/W3DDevice/Common/Thing/W3DModuleFactory.cpp
          Source/W3DDevice/Common/Thing/W3DThingFactory.cpp
          Source/W3DDevice/Common/W3DConvert.cpp
          Source/W3DDevice/GameClient/Drawable/Draw/W3DDebrisDraw.cpp
          Source/W3DDevice/GameClient/Drawable/Draw/W3DDefaultDraw.cpp
          Source/W3DDevice/GameClient/Drawable/Draw/W3DDependencyModelDraw.cpp
          Source/W3DDevice/GameClient/Drawable/Draw/W3DLaserDraw.cpp
          Source/W3DDevice/GameClient/Drawable/Draw/W3DModelDraw.cpp
          Source/W3DDevice/GameClient/Drawable/Draw/W3DOverlordTankDraw.cpp
          Source/W3DDevice/GameClient/Drawable/Draw/W3DPoliceCarDraw.cpp
          Source/W3DDevice/GameClient/Drawable/Draw/W3DProjectileStreamDraw.cpp
          Source/W3DDevice/GameClient/Drawable/Draw/W3DRopeDraw.cpp
          Source/W3DDevice/GameClient/Drawable/Draw/W3DScienceModelDraw.cpp
          Source/W3DDevice/GameClient/Drawable/Draw/W3DSupplyDraw.cpp
          Source/W3DDevice/GameClient/Drawable/Draw/W3DTankDraw.cpp
          Source/W3DDevice/GameClient/Drawable/Draw/W3DTankTruckDraw.cpp
          Source/W3DDevice/GameClient/Drawable/Draw/W3DTracerDraw.cpp
          Source/W3DDevice/GameClient/Drawable/Draw/W3DTruckDraw.cpp
          Source/W3DDevice/GameClient/GUI/Gadget/W3DCheckBox.cpp
          Source/W3DDevice/GameClient/GUI/Gadget/W3DComboBox.cpp
          Source/W3DDevice/GameClient/GUI/Gadget/W3DHorizontalSlider.cpp
          Source/W3DDevice/GameClient/GUI/Gadget/W3DListBox.cpp
          Source/W3DDevice/GameClient/GUI/Gadget/W3DProgressBar.cpp
          Source/W3DDevice/GameClient/GUI/Gadget/W3DPushButton.cpp
          Source/W3DDevice/GameClient/GUI/Gadget/W3DRadioButton.cpp
          Source/W3DDevice/GameClient/GUI/Gadget/W3DStaticText.cpp
          Source/W3DDevice/GameClient/GUI/Gadget/W3DTabControl.cpp
          Source/W3DDevice/GameClient/GUI/Gadget/W3DTextEntry.cpp
          Source/W3DDevice/GameClient/GUI/Gadget/W3DVerticalSlider.cpp
          Source/W3DDevice/GameClient/GUI/GUICallbacks/W3DControlBar.cpp
          Source/W3DDevice/GameClient/GUI/GUICallbacks/W3DMainMenu.cpp
          Source/W3DDevice/GameClient/GUI/GUICallbacks/W3DMOTD.cpp
          Source/W3DDevice/GameClient/GUI/W3DGameFont.cpp
          Source/W3DDevice/GameClient/GUI/W3DGameWindow.cpp
          Source/W3DDevice/GameClient/GUI/W3DGameWindowManager.cpp
          Source/W3DDevice/GameClient/HeightMap.cpp
          Source/W3DDevice/GameClient/Shadow/W3DBufferManager.cpp
          Source/W3DDevice/GameClient/Shadow/W3DProjectedShadow.cpp
          Source/W3DDevice/GameClient/Shadow/W3DShadow.cpp
          Source/W3DDevice/GameClient/Shadow/W3DVolumetricShadow.cpp
          Source/W3DDevice/GameClient/TerrainTex.cpp
          Source/W3DDevice/GameClient/TileData.cpp
          Source/W3DDevice/GameClient/W3DAssetManager.cpp
          Source/W3DDevice/GameClient/W3DAssetManagerExposed.cpp
          Source/W3DDevice/GameClient/W3DBibBuffer.cpp
          Source/W3DDevice/GameClient/W3DBridgeBuffer.cpp
          Source/W3DDevice/GameClient/W3DCustomEdging.cpp
          Source/W3DDevice/GameClient/W3DDebugDisplay.cpp
          Source/W3DDevice/GameClient/W3DDebugIcons.cpp
          Source/W3DDevice/GameClient/W3DDisplay.cpp
          Source/W3DDevice/GameClient/W3DDisplayString.cpp
          Source/W3DDevice/GameClient/W3DDisplayStringManager.cpp
          Source/W3DDevice/GameClient/W3DDynamicLight.cpp
          Source/W3DDevice/GameClient/W3DFileSystem.cpp
          Source/W3DDevice/GameClient/W3DGameClient.cpp
          Source/W3DDevice/GameClient/W3DGranny.cpp
          Source/W3DDevice/GameClient/W3DInGameUI.cpp
          Source/W3DDevice/GameClient/W3DMouse.cpp
          Source/W3DDevice/GameClient/W3DParticleSys.cpp
          Source/W3DDevice/GameClient/W3DPoly.cpp
          Source/W3DDevice/GameClient/W3DRoadBuffer.cpp
          Source/W3DDevice/GameClient/W3DScene.cpp
          Source/W3DDevice/GameClient/W3DShaderManager.cpp
          Source/W3DDevice/GameClient/W3DShroud.cpp
          Source/W3DDevice/GameClient/W3DStatusCircle.cpp
          Source/W3DDevice/GameClient/W3DTerrainTracks.cpp
          Source/W3DDevice/GameClient/W3DTerrainVisual.cpp
          Source/W3DDevice/GameClient/W3DTreeBuffer.cpp
          Source/W3DDevice/GameClient/W3DVideoBuffer.cpp
          Source/W3DDevice/GameClient/W3DView.cpp
          Source/W3DDevice/GameClient/W3dWaypointBuffer.cpp
          Source/W3DDevice/GameClient/W3DWebBrowser.cpp
          Source/W3DDevice/GameClient/Water/W3DWater.cpp
          Source/W3DDevice/GameClient/Water/W3DWaterTracks.cpp
          Source/W3DDevice/GameClient/WorldHeightMap.cpp
          Source/W3DDevice/GameLogic/W3DGameLogic.cpp
          Source/W3DDevice/GameLogic/W3DGhostObject.cpp
          Source/W3DDevice/GameLogic/W3DTerrainLogic.cpp)

if(SAGE_USE_MILES)
  target_sources(
    g_gameenginedevice PRIVATE Include/MilesAudioDevice/MilesAudioManager.h
                               Source/MilesAudioDevice/MilesAudioManager.cpp)
  target_compile_definitions(g_gameenginedevice PRIVATE SAGE_USE_MILES)
endif()

if(SAGE_USE_BINK)
  target_sources(
    g_gameenginedevice PRIVATE Include/VideoDevice/Bink/BinkVideoPlayer.h
                               Source/VideoDevice/Bink/BinkVideoPlayer.cpp)
  target_compile_definitions(g_gameenginedevice PRIVATE SAGE_USE_BINK)
endif()

if(SAGE_USE_SDL3)
  target_sources(
    g_gameenginedevice
    PRIVATE Include/SDL3Device/Common/SDL3CDManager.h
            Include/SDL3Device/Common/SDL3BIGFile.h
            Include/SDL3Device/Common/SDL3BIGFileSystem.h
            Include/SDL3Device/Common/SDL3GameEngine.h
            Include/SDL3Device/Common/SDL3LocalFile.h
            Include/SDL3Device/Common/SDL3LocalFileSystem.h
            Include/SDL3Device/GameClient/SDL3Keyboard.h
            Include/SDL3Device/GameClient/SDL3Mouse.h
            Source/SDL3Device/Common/SDL3BIGFile.cpp
            Source/SDL3Device/Common/SDL3BIGFileSystem.cpp
            Source/SDL3Device/Common/SDL3CDManager.cpp
            Source/SDL3Device/Common/SDL3GameEngine.cpp
            Source/SDL3Device/Common/SDL3IMEManager.cpp
            Source/SDL3Device/Common/SDL3LocalFile.cpp
            Source/SDL3Device/Common/SDL3LocalFileSystem.cpp
            Source/SDL3Device/Common/SDL3OSDisplay.cpp
            Source/SDL3Device/GameClient/SDL3Keyboard.cpp
            Source/SDL3Device/GameClient/SDL3Mouse.cpp)

  find_package(SDL3 CONFIG REQUIRED)
  find_package(SDL3_image CONFIG REQUIRED)
  target_link_libraries(g_gameenginedevice PUBLIC SDL3::SDL3
                                                  SDL3_image::SDL3_image)
  target_compile_definitions(g_gameenginedevice PUBLIC SAGE_USE_SDL3)
endif()

find_package(glm CONFIG REQUIRED)
target_link_libraries(g_gameenginedevice PRIVATE glm::glm)

if(SAGE_USE_FFMPEG)
  target_sources(
    g_gameenginedevice PRIVATE Include/VideoDevice/FFmpeg/FFmpegFile.h
                               Include/VideoDevice/FFmpeg/FFmpegVideoPlayer.h
                               Source/VideoDevice/FFmpeg/FFmpegFile.cpp
                               Source/VideoDevice/FFmpeg/FFmpegVideoPlayer.cpp)
  find_package(FFMPEG REQUIRED)
  target_include_directories(g_gameenginedevice PRIVATE ${FFMPEG_INCLUDE_DIRS})
  target_link_libraries(g_gameenginedevice PRIVATE ${FFMPEG_LIBRARIES})
  target_compile_definitions(g_gameenginedevice PRIVATE SAGE_USE_FFMPEG)
endif()

if(SAGE_USE_MINIAUDIO)
  target_sources(
    g_gameenginedevice PRIVATE Include/MiniAudioDevice/MiniAudioManager.h
                               Source/MiniAudioDevice/MiniAudioManager.cpp)

  target_compile_definitions(g_gameenginedevice PRIVATE SAGE_USE_MINIAUDIO)
endif()

if(SAGE_USE_OPENAL)
  if(NOT SAGE_USE_FFMPEG)
    message(FATAL_ERROR "OpenAL support requires FFmpeg support to be enabled.")
  endif()

  target_sources(
    g_gameenginedevice PRIVATE Include/OpenALDevice/OpenALAudioManager.h
                               Include/OpenALDevice/OpenALAudioStream.h
                               Include/OpenALDevice/FFmpegOpenALAudioStream.h
                               Source/OpenALDevice/OpenALAudioFileCache.h
                               Source/OpenALDevice/OpenALAudioFileCache.cpp
                               Source/OpenALDevice/OpenALAudioManager.cpp
                               Source/OpenALDevice/OpenALAudioStream.cpp
                               Source/OpenALDevice/FFmpegOpenALAudioStream.cpp)

  find_package(OpenAL REQUIRED)
  find_package(fmt REQUIRED)
  target_link_libraries(g_gameenginedevice PRIVATE OpenAL::OpenAL fmt::fmt)
  target_compile_definitions(g_gameenginedevice PRIVATE SAGE_USE_OPENAL)
endif()

if(WIN32)
  target_sources(
    g_gameenginedevice
    PRIVATE Include/Win32Device/Common/Win32BIGFile.h
            Include/Win32Device/Common/Win32BIGFileSystem.h
            Include/Win32Device/Common/Win32CDManager.h
            Include/Win32Device/Common/Win32GameEngine.h
            Include/Win32Device/Common/Win32LocalFile.h
            Include/Win32Device/Common/Win32LocalFileSystem.h
            Include/Win32Device/GameClient/Win32DIKeyboard.h
            Include/Win32Device/GameClient/Win32DIMouse.h
            Include/Win32Device/GameClient/Win32Mouse.h
            Source/Win32Device/Common/Win32BIGFile.cpp
            Source/Win32Device/Common/Win32CDManager.cpp
            Source/Win32Device/Common/Win32GameEngine.cpp
            Source/Win32Device/Common/Win32OSDisplay.cpp
            Source/Win32Device/GameClient/Win32DIKeyboard.cpp
            Source/Win32Device/GameClient/Win32DIMouse.cpp
            Source/Win32Device/GameClient/Win32Mouse.cpp
            Source/Win32Device/Common/Win32BIGFileSystem.cpp
            Source/Win32Device/Common/Win32LocalFile.cpp
            Source/Win32Device/Common/Win32LocalFileSystem.cpp)
endif()

set_target_properties(g_gameenginedevice PROPERTIES OUTPUT_NAME
                                                    "GameEngineDevice")

target_include_directories(g_gameenginedevice
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include)

target_link_libraries(g_gameenginedevice PUBLIC g_gameengine)
