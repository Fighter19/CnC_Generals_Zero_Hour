#include "d3dx8core.h"

HRESULT WINAPID3DXGetErrorStringA(HRESULT hr, LPSTR pBuffer, UINT BufferLen)
{
	return D3DERR_INVALIDCALL;
}

HRESULT WINAPI
D3DXCreateTexture(LPDIRECT3DDEVICE8 pDevice,
	UINT Width,
	UINT Height,
	UINT MipLevels,
	DWORD Usage,
	D3DFORMAT Format,
	D3DPOOL Pool,
	LPDIRECT3DTEXTURE8 *ppTexture)
{
	return pDevice->CreateTexture(Width, Height, MipLevels, Usage, Format, Pool, ppTexture);
}

#include <iostream>

HRESULT WINAPI
D3DXCreateTextureFromFileExA(
	LPDIRECT3DDEVICE8 pDevice,
	LPCSTR pSrcFile,
	UINT Width,
	UINT Height,
	UINT MipLevels,
	DWORD Usage,
	D3DFORMAT Format,
	D3DPOOL Pool,

	DWORD Filter,
	DWORD MipFilter,
	D3DCOLOR ColorKey,
	D3DXIMAGE_INFO *pSrcInfo,
	PALETTEENTRY *pPalette,

	LPDIRECT3DTEXTURE8 *ppTexture)
{
	HRESULT hr;

	return D3DERR_INVALIDCALL;
}

#include <boost/gil.hpp>
#include <boost/gil/extension/numeric/sampler.hpp>
#include <boost/gil/extension/numeric/resample.hpp>

HRESULT WINAPI
D3DXLoadSurfaceFromSurface(
	LPDIRECT3DSURFACE8 pDestSurface,
	CONST PALETTEENTRY *pDestPalette,
	CONST RECT *pDestRect,
	LPDIRECT3DSURFACE8 pSrcSurface,
	CONST PALETTEENTRY *pSrcPalette,
	CONST RECT *pSrcRect,
	DWORD Filter,
	D3DCOLOR ColorKey)
{
	D3DSURFACE_DESC descSrc;
	D3DSURFACE_DESC descDest;

	//DEBUG_ASSERTCRASH(pDestPalette == NULL);

	pSrcSurface->GetDesc(&descSrc);
	pDestSurface->GetDesc(&descDest);

	if (descSrc.Format != descDest.Format)
	{
		// Currently we only support scaling between formats of the same type
		return D3DERR_INVALIDCALL;
	}

	// Pick a compatible format
	//gli::format imageFormat = gli::format::FORMAT_RGBA8_UNORM_PACK8;

	if (descSrc.Format == D3DFMT_A8R8G8B8)
	{
	//	imageFormat = gli::format::FORMAT_RGBA8_UNORM_PACK8;	
	}
	else
	{
		// DebugBreak();
	}

	D3DLOCKED_RECT srcRect;
	pSrcSurface->LockRect(&srcRect, NULL, 0);

	D3DLOCKED_RECT destRect;
	pDestSurface->LockRect(&destRect, NULL, 0);

	unsigned int *buffer = (unsigned int *)destRect.pBits;
	// Fill dest rect with custom pattern
	for (unsigned int y = 0; y < descDest.Height; y++)
	{
		for (unsigned int x = 0; x < descDest.Width; x++)
		{
			//*buffer++=missing_image_palette[*pixels++];
			//*buffer++ = 0x7FFF00FF;
			*buffer++ = 0x7F7F7F7F;
		}
		buffer = (unsigned int *)destRect.pBits;
		buffer += destRect.Pitch / sizeof(unsigned int) * y;
	}

	// // Create gli textures from pBits
	// //gli::texture2d texSrc(gli::format::FORMAT_RGBA8_UNORM_PACK8, gli::extent2d(descSrc.Width, descSrc.Height));
	// boost::gil::argb8_image_t srcImage(descSrc.Width, descSrc.Height);
	// boost::gil::argb8_image_t destImage(descDest.Width, descDest.Height);
	
	// // Copy the data
	// memcpy(boost::gil::view(srcImage).row_begin(0), destRect.pBits, srcRect.Pitch * descSrc.Height);
	
	// // Scale the image (Choose sampler based on filter, it appears that the game only uses nearest neighbor)
	// boost::gil::resize_view(boost::gil::const_view(srcImage), boost::gil::view(destImage), boost::gil::nearest_neighbor_sampler());
	
	// // Copy the data back
	// memcpy(destRect.pBits, boost::gil::view(destImage).row_begin(0), destRect.Pitch * descDest.Height);
	
	pDestSurface->UnlockRect();
	pSrcSurface->UnlockRect();

	return D3D_OK;
}

HRESULT WINAPI
D3DXGetErrorStringA(
	HRESULT hr,
	LPSTR pBuffer,
	UINT BufferLen)
{
	return D3DERR_INVALIDCALL;
}

// Taken and adopted from Wine 3.21
HRESULT WINAPI
D3DXFilterTexture(
	LPDIRECT3DBASETEXTURE8 pBaseTexture,
	CONST PALETTEENTRY *pPalette,
	UINT SrcLevel,
	DWORD Filter)
{
	HRESULT hr = D3DERR_INVALIDCALL;
	if (SrcLevel == D3DX_DEFAULT)
	{
		SrcLevel = 0;
	}
	else if (SrcLevel >= pBaseTexture->GetLevelCount())
	{
		return D3DERR_INVALIDCALL;
	}

	D3DRESOURCETYPE type;
	switch (type = pBaseTexture->GetType())
	{
		case D3DRTYPE_TEXTURE:
		{
			IDirect3DTexture8 *tex = (IDirect3DTexture8 *)pBaseTexture;
			IDirect3DSurface8 *topsurf, *mipsurf;
			D3DSURFACE_DESC desc;
			int i;

			tex->GetLevelDesc(SrcLevel, &desc);
			if (Filter == D3DX_DEFAULT)
			{
				Filter = D3DX_FILTER_BOX;
			}

			int Level = SrcLevel + 1;
			hr = tex->GetSurfaceLevel(SrcLevel, &topsurf);
			if (FAILED(hr))
			{
				return hr;
			}

			while (tex->GetSurfaceLevel(Level, &mipsurf) == D3D_OK)
			{
				// Copy the data
				D3DXLoadSurfaceFromSurface(mipsurf, NULL, NULL, topsurf, NULL, NULL, Filter, 0);

				// Release the surface
				mipsurf->Release();
				topsurf = mipsurf;

				Level++;
			}

			topsurf->Release();
			if (FAILED(hr))
			{
				return hr;
			}
		}
		return D3D_OK;
	}

	return D3D_OK;
}

HRESULT WINAPI
D3DXCreateCubeTexture(
	LPDIRECT3DDEVICE8 pDevice,
	UINT Size,
	UINT MipLevels,
	DWORD Usage,
	D3DFORMAT Format,
	D3DPOOL Pool,
	LPDIRECT3DCUBETEXTURE8 *ppCubeTexture)
{
	// Also unused by the game
	return D3DERR_INVALIDCALL;
}

HRESULT WINAPI
D3DXCreateVolumeTexture(
	LPDIRECT3DDEVICE8 pDevice,
	UINT Width,
	UINT Height,
	UINT Depth,
	UINT MipLevels,
	DWORD Usage,
	D3DFORMAT Format,
	D3DPOOL Pool,
	LPDIRECT3DVOLUMETEXTURE8 *ppVolumeTexture)
{
	// Used to create volume textures by the texture loader
	// Actually not used in the game
	return D3DERR_INVALIDCALL;
}

HRESULT WINAPI
D3DXAssembleShader(
	LPCVOID pSrcData,
	UINT SrcDataLen,
	DWORD Flags,
	LPD3DXBUFFER *ppConstants,
	LPD3DXBUFFER *ppCompiledShader,
	LPD3DXBUFFER *ppCompilationErrors)
{
	// Called to create water shader amongst other things
	// Code seems to be handle assembly failing
	return D3DERR_INVALIDCALL;
}

HRESULT WINAPI
D3DXAssembleShaderFromFileA(
	LPCSTR pSrcFile,
	DWORD Flags,
	LPD3DXBUFFER *ppConstants,
	LPD3DXBUFFER *ppCompiledShader,
	LPD3DXBUFFER *ppCompilationErrors)
{
	// Not required by the game
	return D3DERR_INVALIDCALL;
}

// Taken from Wine at ca46949
static UINT Get_TexCoord_Size_From_FVF(DWORD FVF, int tex_num)
{
    return (((((FVF) >> (16 + (2 * (tex_num)))) + 1) & 0x03) + 1);
}

UINT WINAPI D3DXGetFVFVertexSize(DWORD FVF)
{
    DWORD size = 0;
    UINT i;
    UINT numTextures = (FVF & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT;

    if (FVF & D3DFVF_NORMAL) size += sizeof(D3DVECTOR);
    if (FVF & D3DFVF_DIFFUSE) size += sizeof(DWORD);
    if (FVF & D3DFVF_SPECULAR) size += sizeof(DWORD);
    if (FVF & D3DFVF_PSIZE) size += sizeof(DWORD);

    switch (FVF & D3DFVF_POSITION_MASK)
    {
        case D3DFVF_XYZ:    size += sizeof(D3DVECTOR); break;
        case D3DFVF_XYZRHW: size += 4 * sizeof(FLOAT); break;
        case D3DFVF_XYZB1:  size += 4 * sizeof(FLOAT); break;
        case D3DFVF_XYZB2:  size += 5 * sizeof(FLOAT); break;
        case D3DFVF_XYZB3:  size += 6 * sizeof(FLOAT); break;
        case D3DFVF_XYZB4:  size += 7 * sizeof(FLOAT); break;
        case D3DFVF_XYZB5:  size += 8 * sizeof(FLOAT); break;
		// Not present in D3D8, thus uncommented
        //case D3DFVF_XYZW:   size += 4 * sizeof(FLOAT); break;
    }

    for (i = 0; i < numTextures; i++)
    {
        size += Get_TexCoord_Size_From_FVF(FVF, i) * sizeof(FLOAT);
    }

    return size;
}
