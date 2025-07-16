#include <SDL3/SDL.h>

class SDL3Wrapper
{
public:
  bool Init(void *hwnd, bool lite = false);
  void Shutdown(void);
  bool CreateDevice(void);

  void BeginScene(void);
  void EndScene(void);

private:
  SDL_GPUShader *LoadDefaultShader(bool bIsVertex);

  SDL_GPUGraphicsPipeline *CreateDefaultPipeline();


  //SDL_GPUShader *DefaultFragmentShader = NULL;
  //SDL_GPUShader *DefaultVertexShader = NULL;
  SDL_GPUGraphicsPipeline *DefaultPipeline = NULL;

  SDL_GPUCommandBuffer *CurrentGPUCommandBuffer = NULL;

  SDL_GPUBuffer *DefaultVertexBuffer = NULL;
  SDL_GPURenderPass *CurrentGPUPass = NULL;
};

extern SDL3Wrapper TheSDL3Wrapper;
