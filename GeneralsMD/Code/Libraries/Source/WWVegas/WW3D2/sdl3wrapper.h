#pragma once
#include <SDL3/SDL.h>
#include <memory>

#include "rect.h"
#include "vector3.h"
#include "texturefilter.h"
#include "wwdebug.h"
#include "ww3dformat.h"

namespace Rendering {

WWINLINE void CheckForError(bool hr, char const* msg = nullptr)
{
  if (!hr)
  {
    WWDEBUG_SAY(("Rendering error! %s\n", msg ? msg : ""));
    WWASSERT(false);
  }
}

typedef SDL_GPUViewport Viewport;

struct LockedRect
{
  // Length of a line in bytes
  unsigned int nPitch;
  // Pointer to the locked data
  void* pBits;
};

/** Information about a certain Mip level of a texture. */
struct LevelInfo
{
  union
  {
    WW3DFormat Format;
    WW3DZFormat DepthStencilFormat;
  };
  int Width;
  int Height;
  int Depth; // For volume textures
};

enum ResourceLocation
{
  // Determines the best location for the resource
  // Prefers video memory if available
  RESOURCE_LOCATION_UNSPECIFIED = 0,
  RESOURCE_LOCATION_VIDEO_MEMORY,
  RESOURCE_LOCATION_SYSTEM_MEMORY,
};

// These match the flags from SDL3
enum TextureUsage
{
  TEXTUREUSAGE_SAMPLER = 1 << 0,
  TEXTUREUSAGE_COLOR_TARGET = 1 << 1,
  TEXTUREUSAGE_DEPTH_STENCIL_TARGET = 1 << 2,
  TEXTUREUSAGE_GRAPHICS_STORAGE_READ = 1 << 3,
  TEXTUREUSAGE_COMPUTE_STORAGE_READ = 1 << 4,
  TEXTUREUSAGE_COMPUTE_STORAGE_WRITE = 1 << 5,
  TEXTUREUSAGE_COMPUTE_STORAGE_SIMULTANEOUS_READ_WRITE = 1 << 6,
};

class ITexture
{
public:
  virtual ~ITexture() = default;

  /** Lock rectangle area of a texture to allow direct memory access.
   * @param level Mipmap level to lock.
   * @param pLockedRect Pointer to a LockedRect structure to receive the locked data.
   * @param pRect Pointer to a RectClass structure that specifies the area to lock.
   *             If NULL is passed, the entire level will be locked.
   * @param flags Locking flags (e.g. read-only/don't upload after unlock)
   */
  virtual bool LockRect(unsigned int level, LockedRect *pLockedRect, const RectClass* pRect=NULL, unsigned int flags=0) = 0;
  /** Unlock a previously locked texture level.
   * @param level Mipmap level to unlock.
   */
  virtual bool UnlockRect(unsigned int level) = 0;

  /** Get the number of mipmap levels in the texture.
   * @return Number of mipmap levels.
   */
  virtual int GetLevelCount() const = 0;

  /** Get information about a certain mip level of the texture.
   * @param level Mipmap level to query.
   * @return LevelInfo structure containing information about the specified level.
   */
  virtual bool GetLevelInfo(unsigned int level, LevelInfo& outInfo) const = 0;
};

class IRenderDevice
{
public:
  virtual ~IRenderDevice() = default;

  virtual bool Init(void *hwnd, bool lite = false) = 0;
  virtual void Shutdown(void) = 0;
  virtual bool CreateDevice(void) = 0;

  virtual void BeginScene(void) = 0;
  virtual void EndScene(void) = 0;

  virtual void SetViewport(const Viewport* pViewport) = 0;
  virtual void Clear(bool clear_color, bool clear_z_stencil, const Vector3 &color, float dest_alpha, float z, unsigned int stencil) = 0;

  /** Create a texture.
   * @param width Width of the texture in pixels.
   * @param height Height of the texture in pixels.
   * @param mip_count Number of mipmap levels. Use MIP_LEVELS_ALL to generate full mipmap chain.
   * @param usage Texture usage flags (see TextureUsage enum).
   * @param format Texture format (see WW3DFormat enum).
   * @param location Preferred resource location (see ResourceLocation enum).
   * @return Pointer to the created texture, or nullptr if creation failed.
   */
  virtual std::unique_ptr<ITexture> CreateTexture(int width, int height, MipCountType mip_count, int usage, WW3DFormat format, ResourceLocation location) = 0;
};

class SDL3Texture;

class SDL3Wrapper : public IRenderDevice
{
  friend SDL3Texture;
public:
  bool Init(void *hwnd, bool lite = false) override;
  void Shutdown(void) override;
  bool CreateDevice(void) override;

  void BeginScene(void) override;
  void EndScene(void) override;

  void SetViewport(const Viewport* pViewport) override;
  void Clear(bool clear_color, bool clear_z_stencil, const Vector3 &color, float dest_alpha = 0.0f, float z = 1.0f, unsigned int stencil = 0) override;

  std::unique_ptr<ITexture> CreateTexture(int width, int height, MipCountType mip_count, int usage, WW3DFormat format, ResourceLocation location) override;

private:
  SDL_GPUShader *LoadDefaultShader(bool bIsVertex);

  SDL_GPUGraphicsPipeline *CreateDefaultPipeline();


  //SDL_GPUShader *DefaultFragmentShader = NULL;
  //SDL_GPUShader *DefaultVertexShader = NULL;
  SDL_GPUGraphicsPipeline *DefaultPipeline = NULL;

  SDL_GPUCommandBuffer *CurrentGPUCommandBuffer = NULL;
  SDL_GPUCommandBuffer *CurrentGPUCopyCommandBuffer = NULL;

  SDL_GPUColorTargetInfo ColorTargetInfo = {};
  SDL_GPUDepthStencilTargetInfo DepthStencilTargetInfo = {};

  SDL_GPUBuffer *DefaultVertexBuffer = NULL;
  SDL_GPURenderPass *CurrentGPUPass = NULL;
};

Rendering::IRenderDevice* GetRenderDevice();

} // namespace Rendering

extern Rendering::SDL3Wrapper TheSDL3Wrapper;
