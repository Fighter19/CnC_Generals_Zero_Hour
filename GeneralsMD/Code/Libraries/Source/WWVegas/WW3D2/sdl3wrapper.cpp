#include "sdl3wrapper.h"

#include "default_shader.h"

#include "wwdebug.h"
#include "vector4.h"
#include "matrix4.h"

extern SDL_Window* TheSDL3WindowVulkan;
SDL_GPUDevice *SDLGPUDevice = NULL;
SDL3Wrapper TheSDL3Wrapper;

struct D3D9FixedFunctionVS
{
  Matrix4x4 WorldView;
  Matrix4x4 Normal;
  Matrix4x4 InverseView;
  Matrix4x4 Projection;
  Matrix4x4 TexcoordTransform[8];
  Vector4 ViewportInfo_InverseOffset;
  Vector4 ViewportInfo_InverseExtent;
};

bool SDL3Wrapper::Init(void *hwnd, bool lite)
{
  return true;
}

void SDL3Wrapper::Shutdown(void)
{
  // Cleanup resources if needed
  if (SDLGPUDevice)
  {
    SDL_DestroyGPUDevice(SDLGPUDevice);
    SDLGPUDevice = NULL;
  }
}

bool SDL3Wrapper::CreateDevice(void)
{
  SDLGPUDevice = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, false, NULL);
  if (SDLGPUDevice == NULL)
  {
    WWDEBUG_SAY(("Failed to create SDL GPU device: %s\n", SDL_GetError()));
    return false;
  }

  if (!SDL_ClaimWindowForGPUDevice(SDLGPUDevice, TheSDL3WindowVulkan))
  {
    WWDEBUG_SAY(("Failed to claim SDL GPU device for window: %s\n", SDL_GetError()));
    SDL_DestroyGPUDevice(SDLGPUDevice);
    SDLGPUDevice = NULL;
    return false;
  }

  CurrentGPUCommandBuffer = SDL_AcquireGPUCommandBuffer(SDLGPUDevice);
  if (!CurrentGPUCommandBuffer)
  {
    WWDEBUG_SAY(("Failed to acquire command buffer for SDL GPU device: %s\n", SDL_GetError()));
    SDL_DestroyGPUDevice(SDLGPUDevice);
    SDLGPUDevice = NULL;
    return false;
  }

  // Create a default vertex buffer for rendering a viewport quad
  SDL_GPUBufferCreateInfo vertexBufferCreateInfo = {};
  vertexBufferCreateInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
  vertexBufferCreateInfo.size = 4 * sizeof(float) * 8; // 4 vertices, each with 8 floats (x, y, z, w, r, g, b, a)
  // vertexBufferCreateInfo.props = SDL_CreateProperties();
  DefaultVertexBuffer = SDL_CreateGPUBuffer(SDLGPUDevice, &vertexBufferCreateInfo);

  // SDL_DestroyProperties(vertexBufferCreateInfo.props);

  // Start of uploading a quad to the GPU
  SDL_GPUTransferBufferCreateInfo transferBufferCreateInfo = {};
  transferBufferCreateInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
  transferBufferCreateInfo.size = 4 * sizeof(float) * 8; // 4 vertices, each with 4 floats (x, y, z, w)
  SDL_GPUTransferBuffer *pTransferBuffer = SDL_CreateGPUTransferBuffer(SDLGPUDevice, &transferBufferCreateInfo);
  if (!pTransferBuffer)
  {
    WWDEBUG_SAY(("Failed to create transfer buffer for SDL GPU device: %s\n", SDL_GetError()));
    SDL_DestroyGPUDevice(SDLGPUDevice);
    SDLGPUDevice = NULL;
    return false;
  }

  void *data = SDL_MapGPUTransferBuffer(SDLGPUDevice, pTransferBuffer, false);
  if (!data)
  {
    WWDEBUG_SAY(("Failed to map transfer buffer for SDL GPU device: %s\n", SDL_GetError()));
    SDL_ReleaseGPUTransferBuffer(SDLGPUDevice, pTransferBuffer);
    SDL_DestroyGPUDevice(SDLGPUDevice);
    SDLGPUDevice = NULL;
    return false;
  }

    float quadVertices[] = {
      // x, y, z, w, r, g, b, a
      800, 600, 0.0, 1.0, 0.63, 0.63, 0.63, 1.0,
      800, 0.0, 0.0, 1.0, 0.63, 0.63, 0.63, 1.0,
      0.0, 600, 0.0, 1.0, 0.63, 0.63, 0.63, 1.0,
      0.0, 0.0, 0.0, 1.0, 0.63, 0.63, 0.63, 1.0,
    };

  memcpy(data, quadVertices, sizeof(quadVertices));
  SDL_UnmapGPUTransferBuffer(SDLGPUDevice, pTransferBuffer);

  SDL_GPUCopyPass *pCopyPass = SDL_BeginGPUCopyPass(CurrentGPUCommandBuffer);
  if (!pCopyPass)
  {
    WWDEBUG_SAY(("Failed to begin GPU copy pass for SDL GPU device: %s\n", SDL_GetError()));
    SDL_ReleaseGPUTransferBuffer(SDLGPUDevice, pTransferBuffer);
    SDL_DestroyGPUDevice(SDLGPUDevice);
    SDLGPUDevice = NULL;
    return false;
  }

  SDL_GPUTransferBufferLocation srcLocation = {};
  srcLocation.transfer_buffer = pTransferBuffer;
  srcLocation.offset = 0;

  SDL_GPUBufferRegion dstRegion = {};
  dstRegion.buffer = DefaultVertexBuffer;
  dstRegion.offset = 0;
  dstRegion.size = 4 * sizeof(float) * 8; // 4 vertices, each with 8 floats (x, y, z, w, r, g, b, a)
  static_assert(4 * sizeof(float) * 8 == sizeof(quadVertices), "Size mismatch for quad vertices");

  SDL_UploadToGPUBuffer(pCopyPass, &srcLocation, &dstRegion, false);
  SDL_EndGPUCopyPass(pCopyPass);
  SDL_SubmitGPUCommandBuffer(CurrentGPUCommandBuffer);

  SDL_ReleaseGPUTransferBuffer(SDLGPUDevice, pTransferBuffer);
  // End of uploading a quad to the GPU

  DefaultPipeline = CreateDefaultPipeline();

  return true;
}

void SDL3Wrapper::BeginScene()
{
  // BeginScene doesn't actually do anything,
  // so SDL doesn't care about it
  CurrentGPUCommandBuffer = SDL_AcquireGPUCommandBuffer(SDLGPUDevice);
  if (!CurrentGPUCommandBuffer)
  {
    WWDEBUG_SAY(("Failed to acquire GPU command buffer for DX8Wrapper::Begin_Scene()"));
    return;
  }

  SDL_GPUTexture *swapChainTexture = NULL;
  Uint32 textureWidth = 0;
  Uint32 textureHeight = 0;
  if (!SDL_WaitAndAcquireGPUSwapchainTexture(CurrentGPUCommandBuffer, TheSDL3WindowVulkan, &swapChainTexture, &textureWidth, &textureHeight))
  {
    WWDEBUG_SAY(("Failed to acquire GPU swapchain texture for DX8Wrapper::Begin_Scene()"));
    return;
  }

  if (!swapChainTexture)
  {
    WWDEBUG_SAY(("No swapchain texture available for DX8Wrapper::Begin_Scene()"));
    SDL_CancelGPUCommandBuffer(CurrentGPUCommandBuffer);
    return;
  }

  SDL_GPUColorTargetInfo colorTargetInfo = {};
  colorTargetInfo.clear_color.a = 1.0f;
  colorTargetInfo.clear_color.r = 1.0f;
  colorTargetInfo.clear_color.g = 0.0f;
  colorTargetInfo.clear_color.b = 1.0f;
  colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
  colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;
  colorTargetInfo.texture = swapChainTexture;

  SDL_GPUDepthStencilTargetInfo depthTargetInfo = {};
  depthTargetInfo.clear_depth = 1.0f;
  depthTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
  depthTargetInfo.store_op = SDL_GPU_STOREOP_DONT_CARE;
  depthTargetInfo.stencil_load_op = SDL_GPU_LOADOP_DONT_CARE;
  depthTargetInfo.stencil_store_op = SDL_GPU_STOREOP_DONT_CARE;
  // TODO: Create depth buffer texture and pass
  depthTargetInfo.texture = NULL; // No depth buffer for now
  depthTargetInfo.cycle = false;

  D3D9FixedFunctionVS vsConstants = {};
  vsConstants.ViewportInfo_InverseOffset = Vector4(-1.0,1.0,0.0,0.0);
  vsConstants.ViewportInfo_InverseExtent = Vector4(2.0 / 800, -2.0 / 600, 1.0, 1.0);

  SDL_PushGPUVertexUniformData(CurrentGPUCommandBuffer, 0, &vsConstants, sizeof(vsConstants));

  CurrentGPUPass = SDL_BeginGPURenderPass(CurrentGPUCommandBuffer, &colorTargetInfo, 1, NULL /*&depthTargetInfo*/);
  if (!CurrentGPUPass)
  {
    WWDEBUG_SAY(("Failed to begin GPU render pass for DX8Wrapper::Begin_Scene()"));
    SDL_CancelGPUCommandBuffer(CurrentGPUCommandBuffer);
    return;
  }

  SDL_BindGPUGraphicsPipeline(CurrentGPUPass, DefaultPipeline);

  SDL_GPUBufferBinding vertexBinding;
  // Currently the viewport quad
  vertexBinding.buffer = DefaultVertexBuffer;
  vertexBinding.offset = 0;
  SDL_BindGPUVertexBuffers(CurrentGPUPass, 0, &vertexBinding, 1);
  SDL_DrawGPUPrimitives(CurrentGPUPass, 4, 1, 0, 0);
}

void SDL3Wrapper::EndScene()
{
  SDL_EndGPURenderPass(CurrentGPUPass);
  SDL_SubmitGPUCommandBuffer(CurrentGPUCommandBuffer);
  CurrentGPUPass = NULL;
}

// Private functions
SDL_GPUShader *SDL3Wrapper::LoadDefaultShader(bool bIsVertex)
{
  SDL_GPUShaderCreateInfo createInfo;
  createInfo.num_samplers = 0;
  createInfo.num_uniform_buffers = 0;
  createInfo.num_storage_buffers = 0;
  createInfo.num_storage_textures = 0;
  createInfo.stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
  createInfo.entrypoint = "main";
  if (bIsVertex)
  {
    // Used to pass window size
    createInfo.num_uniform_buffers = 1;
    createInfo.stage = SDL_GPU_SHADERSTAGE_VERTEX;
  }

  createInfo.code = defaultShaderCode;
  createInfo.code_size = sizeof(defaultShaderCode);

  return SDL_CreateGPUShader(SDLGPUDevice, &createInfo);
}

SDL_GPUGraphicsPipeline *SDL3Wrapper::CreateDefaultPipeline()
{
  SDL_GPUGraphicsPipelineCreateInfo pipelineCreateInfo = {0};

  pipelineCreateInfo.target_info.num_color_targets = 1;
  static const SDL_GPUColorTargetDescription colorTargetDescription = {
      .format = SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM,
      .blend_state = {
          // Apply shadow blending directly to the color target
          // (Ignore dst color and just use pure color, then add to dst)
          .src_color_blendfactor = SDL_GPU_BLENDFACTOR_DST_COLOR,
          .dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ZERO,
          .color_blend_op = SDL_GPU_BLENDOP_ADD,
          .src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_DST_COLOR,
          .dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ZERO,
          .alpha_blend_op = SDL_GPU_BLENDOP_ADD,
          .color_write_mask = SDL_GPU_COLORCOMPONENT_R | SDL_GPU_COLORCOMPONENT_G | SDL_GPU_COLORCOMPONENT_B,
          // Set to true, when shadow should be blended
          // For now we use the shadow shaders as default
          .enable_blend = false,
          .enable_color_write_mask = false,
      },
  };
  pipelineCreateInfo.target_info.color_target_descriptions = &colorTargetDescription;

  SDL_GPUVertexBufferDescription vertexBufferDescription = {};
  vertexBufferDescription.pitch = 8 * sizeof(float); // 4 floats per vertex
  vertexBufferDescription.slot = 0;
  vertexBufferDescription.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
  vertexBufferDescription.instance_step_rate = 0;
  
  SDL_GPUVertexAttribute vertexAttributes[2] = {};
  vertexAttributes[0].location = 0; // Position
  vertexAttributes[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4;
  vertexAttributes[0].offset = 0;
  vertexAttributes[0].buffer_slot = 0;

  vertexAttributes[1].location = 12; // Color
  vertexAttributes[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4;
  vertexAttributes[1].offset = 4 * sizeof(float); // After position
  vertexAttributes[1].buffer_slot = 0;

  pipelineCreateInfo.vertex_input_state = {};
  pipelineCreateInfo.vertex_input_state.num_vertex_buffers = 1;
  pipelineCreateInfo.vertex_input_state.vertex_buffer_descriptions = &vertexBufferDescription;
  pipelineCreateInfo.vertex_input_state.num_vertex_attributes = 2;
  pipelineCreateInfo.vertex_input_state.vertex_attributes = vertexAttributes; 

  pipelineCreateInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLESTRIP;

  pipelineCreateInfo.vertex_shader = LoadDefaultShader(true);
  pipelineCreateInfo.fragment_shader = LoadDefaultShader(false);

  return SDL_CreateGPUGraphicsPipeline(SDLGPUDevice, &pipelineCreateInfo);
}

class SDL3Surface: public ISurface
{
public:
  SDL3Surface(SDL_GPUTexture *texture) : texture(texture) {}
  ~SDL3Surface() override
  {
    if (texture)
    {
      SDL_ReleaseGPUTexture(SDLGPUDevice, texture);
    }
  }

private:
  SDL_GPUTexture *texture;
};

std::unique_ptr<ISurface> SDL3Wrapper::CreateSurface(int width, int height, int format)
{
  SDL_GPUTextureCreateInfo textureCreateInfo = {};
  textureCreateInfo.type = SDL_GPU_TEXTURETYPE_2D;
  textureCreateInfo.width = width;
  textureCreateInfo.height = height;
  // TODO: Convert WW3DFormat to SDL_GPUTextureFormat
  WWASSERT_PRINT(false, ("Not yet supported"));
  textureCreateInfo.format = static_cast<SDL_GPUTextureFormat>(format);
  textureCreateInfo.usage = SDL_GPU_TEXTUREUSAGE_COLOR_TARGET | SDL_GPU_TEXTUREUSAGE_SAMPLER;
  textureCreateInfo.num_levels = 1; // No mipmaps for now
  textureCreateInfo.props = 0;

  return std::make_unique<SDL3Surface>(SDL_CreateGPUTexture(SDLGPUDevice, &textureCreateInfo));
}