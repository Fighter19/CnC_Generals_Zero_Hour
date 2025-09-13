#pragma once
#include <SDL3/SDL.h>
#include <memory>

#include "rect.h"
#include "vector3.h"

namespace Rendering {
// Implementation defined opaque types
class ISurface
{
public:
  virtual ~ISurface() = default;
};

class ITexture
{
public:
  virtual ~ITexture() = default;
};

typedef SDL_GPUViewport Viewport;

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

  virtual std::unique_ptr<ISurface> CreateSurface(int width, int height, int format) = 0;
};

class SDL3Wrapper : public IRenderDevice
{
public:
  bool Init(void *hwnd, bool lite = false) override;
  void Shutdown(void) override;
  bool CreateDevice(void) override;

  void BeginScene(void) override;
  void EndScene(void) override;

  void SetViewport(const Viewport* pViewport) override;
  void Clear(bool clear_color, bool clear_z_stencil, const Vector3 &color, float dest_alpha = 0.0f, float z = 1.0f, unsigned int stencil = 0) override;

  std::unique_ptr<ISurface> CreateSurface(int width, int height, int format) override;

private:
  SDL_GPUShader *LoadDefaultShader(bool bIsVertex);

  SDL_GPUGraphicsPipeline *CreateDefaultPipeline();


  //SDL_GPUShader *DefaultFragmentShader = NULL;
  //SDL_GPUShader *DefaultVertexShader = NULL;
  SDL_GPUGraphicsPipeline *DefaultPipeline = NULL;

  SDL_GPUCommandBuffer *CurrentGPUCommandBuffer = NULL;

  SDL_GPUColorTargetInfo ColorTargetInfo = {};
  SDL_GPUDepthStencilTargetInfo DepthStencilTargetInfo = {};

  SDL_GPUBuffer *DefaultVertexBuffer = NULL;
  SDL_GPURenderPass *CurrentGPUPass = NULL;
};

} // namespace Rendering

extern Rendering::SDL3Wrapper TheSDL3Wrapper;
