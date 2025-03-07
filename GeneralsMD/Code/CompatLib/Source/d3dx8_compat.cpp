/*
 * Copyright (C) 2002 Raphael Junqueira
 * Copyright (C) 2007-2008 David Adam
 * Copyright (C) 2008 Tony Wasserka
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 *
 */
#include "d3dx8core.h"

#include <stdio.h>
#include <algorithm>

#if 0
#define IDirect3DSurface9 IDirect3DSurface8
#define IDirect3DDevice9 IDirect3DDevice8

#define IDirect3DSurface9_GetDesc IDirect3DSurface8_GetDesc
#define IDirect3DSurface9_GetDevice IDirect3DSurface8_GetDevice
#define IDirect3DSurface9_LockRect IDirect3DSurface8_LockRect
#define IDirect3DSurface9_UnlockRect IDirect3DSurface8_UnlockRect

#define IDirect3DDevice9_StretchRect IDirect3DDevice8_StretchRect
#define IDirect3DDevice9_Release IDirect3DDevice8_Release
#define IDirect3DDevice9_CreateOffscreenPlainSurface IDirect3DDevice8_CreateOffscreenPlainSurface
#define IDirect3DDevice9_CreateRenderTarget IDirect3DDevice8_CreateRenderTarget
#endif

#define TRACE(msg, ...)
#define WARN(msg, ...) fprintf(stderr, msg, ##__VA_ARGS__)
#define FIXME(msg, ...) fprintf(stderr, msg, ##__VA_ARGS__)

static const char *debugstr_fourcc(DWORD fourcc)
{
  static char buf[5];
  buf[0] = (char)(fourcc & 0xFF);
  buf[1] = (char)((fourcc >> 8) & 0xFF);
  buf[2] = (char)((fourcc >> 16) & 0xFF);
  buf[3] = (char)((fourcc >> 24) & 0xFF);
  buf[4] = '\0';
  return buf;
}

void SetRect(RECT *rect, LONG left, LONG top, LONG right, LONG bottom)
{
  rect->left = left;
  rect->top = top;
  rect->right = right;
  rect->bottom = bottom;
}

// d3dx9.h
#define D3DX_DEFAULT         ((UINT)-1)

#define _FACDD 0x876
#define MAKE_DDHRESULT(code) MAKE_HRESULT(1, _FACDD, code)

enum _D3DXERR {
    D3DXERR_CANNOTMODIFYINDEXBUFFER = MAKE_DDHRESULT(2900),
    D3DXERR_INVALIDMESH             = MAKE_DDHRESULT(2901),
    D3DXERR_CANNOTATTRSORT          = MAKE_DDHRESULT(2902),
    D3DXERR_SKINNINGNOTSUPPORTED    = MAKE_DDHRESULT(2903),
    D3DXERR_TOOMANYINFLUENCES       = MAKE_DDHRESULT(2904),
    D3DXERR_INVALIDDATA             = MAKE_DDHRESULT(2905),
    D3DXERR_LOADEDMESHASNODATA      = MAKE_DDHRESULT(2906),
    D3DXERR_DUPLICATENAMEDFRAGMENT  = MAKE_DDHRESULT(2907),
    D3DXERR_CANNOTREMOVELASTITEM    = MAKE_DDHRESULT(2908),
};

// d3dx9tex.h
#define D3DX_FILTER_NONE                 0x00000001
#define D3DX_FILTER_POINT                0x00000002
#define D3DX_FILTER_LINEAR               0x00000003
#define D3DX_FILTER_TRIANGLE             0x00000004
#define D3DX_FILTER_DITHER               0x00080000

// d3dx9_private.h
#define D3DX9_FILTER_INVALID_BITS 0xff80fff8
static inline HRESULT d3dx9_validate_filter(uint32_t filter)
{
    if ((filter & D3DX9_FILTER_INVALID_BITS) || !(filter & 0x7) || ((filter & 0x7) > D3DX_FILTER_BOX))
        return D3DERR_INVALIDCALL;

    return D3D_OK;
}

static inline HRESULT d3dx9_handle_load_filter(DWORD *filter)
{
    if (*filter == D3DX_DEFAULT)
        *filter = D3DX_FILTER_TRIANGLE | D3DX_FILTER_DITHER;

    return d3dx9_validate_filter(*filter);
}

struct volume
{
    UINT width;
    UINT height;
    UINT depth;
};

/* These values act as indexes into the pixel_format_desc table. */
enum d3dx_pixel_format_id
{
    D3DX_PIXEL_FORMAT_B8G8R8_UNORM,
    D3DX_PIXEL_FORMAT_B8G8R8A8_UNORM,
    D3DX_PIXEL_FORMAT_B8G8R8X8_UNORM,
    D3DX_PIXEL_FORMAT_R8G8B8A8_UNORM,
    D3DX_PIXEL_FORMAT_R8G8B8X8_UNORM,
    D3DX_PIXEL_FORMAT_B5G6R5_UNORM,
    D3DX_PIXEL_FORMAT_B5G5R5X1_UNORM,
    D3DX_PIXEL_FORMAT_B5G5R5A1_UNORM,
    D3DX_PIXEL_FORMAT_B2G3R3_UNORM,
    D3DX_PIXEL_FORMAT_B2G3R3A8_UNORM,
    D3DX_PIXEL_FORMAT_B4G4R4A4_UNORM,
    D3DX_PIXEL_FORMAT_B4G4R4X4_UNORM,
    D3DX_PIXEL_FORMAT_B10G10R10A2_UNORM,
    D3DX_PIXEL_FORMAT_R10G10B10A2_UNORM,
    D3DX_PIXEL_FORMAT_R16G16B16_UNORM,
    D3DX_PIXEL_FORMAT_R16G16B16A16_UNORM,
    D3DX_PIXEL_FORMAT_R16G16_UNORM,
    D3DX_PIXEL_FORMAT_A8_UNORM,
    D3DX_PIXEL_FORMAT_L8A8_UNORM,
    D3DX_PIXEL_FORMAT_L4A4_UNORM,
    D3DX_PIXEL_FORMAT_L8_UNORM,
    D3DX_PIXEL_FORMAT_L16_UNORM,
    D3DX_PIXEL_FORMAT_DXT1_UNORM,
    D3DX_PIXEL_FORMAT_DXT2_UNORM,
    D3DX_PIXEL_FORMAT_DXT3_UNORM,
    D3DX_PIXEL_FORMAT_DXT4_UNORM,
    D3DX_PIXEL_FORMAT_DXT5_UNORM,
    D3DX_PIXEL_FORMAT_R16_FLOAT,
    D3DX_PIXEL_FORMAT_R16G16_FLOAT,
    D3DX_PIXEL_FORMAT_R16G16B16A16_FLOAT,
    D3DX_PIXEL_FORMAT_R32_FLOAT,
    D3DX_PIXEL_FORMAT_R32G32_FLOAT,
    D3DX_PIXEL_FORMAT_R32G32B32A32_FLOAT,
    D3DX_PIXEL_FORMAT_P8_UINT,
    D3DX_PIXEL_FORMAT_P8_UINT_A8_UNORM,
    D3DX_PIXEL_FORMAT_U8V8W8Q8_SNORM,
    D3DX_PIXEL_FORMAT_U16V16W16Q16_SNORM,
    D3DX_PIXEL_FORMAT_U8V8_SNORM,
    D3DX_PIXEL_FORMAT_U16V16_SNORM,
    D3DX_PIXEL_FORMAT_U8V8_SNORM_L8X8_UNORM,
    D3DX_PIXEL_FORMAT_U10V10W10_SNORM_A2_UNORM,
    D3DX_PIXEL_FORMAT_R8G8_B8G8_UNORM,
    D3DX_PIXEL_FORMAT_G8R8_G8B8_UNORM,
    D3DX_PIXEL_FORMAT_UYVY,
    D3DX_PIXEL_FORMAT_YUY2,
    D3DX_PIXEL_FORMAT_COUNT,
};

/* for internal use */
enum component_type
{
    CTYPE_EMPTY,
    CTYPE_UNORM,
    CTYPE_SNORM,
    CTYPE_FLOAT,
    CTYPE_LUMA,
    CTYPE_INDEX,
};

enum format_flag
{
    FMT_FLAG_DXT  = 0x01,
    FMT_FLAG_PACKED = 0x02,
    /* Internal only format, has no exact D3DFORMAT equivalent. */
    FMT_FLAG_INTERNAL = 0x04,
};

struct pixel_format_desc {
  enum d3dx_pixel_format_id format;
  BYTE bits[4];
  BYTE shift[4];
  UINT bytes_per_pixel;
  UINT block_width;
  UINT block_height;
  UINT block_byte_count;
  enum component_type a_type;
  enum component_type rgb_type;
  uint32_t flags;
};

struct d3dx_pixels
{
    const void *data;
    uint32_t row_pitch;
    uint32_t slice_pitch;
    const PALETTEENTRY *palette;

    struct volume size;
    RECT unaligned_rect;
};

static inline BOOL is_unknown_format(const struct pixel_format_desc *format)
{
    return (format->format == D3DX_PIXEL_FORMAT_COUNT);
}
// d3dx9_private end

/************************************************************
 * pixel format table providing info about number of bytes per pixel,
 * number of bits per channel and format type.
 *
 * Call get_format_info to request information about a specific format.
 */
static const struct pixel_format_desc formats[] =
{
    /* format                                    bpc               shifts             bpp blocks   alpha type   rgb type     flags */
    {D3DX_PIXEL_FORMAT_B8G8R8_UNORM,             { 0,  8,  8,  8}, { 0, 16,  8,  0},  3, 1, 1,  3, CTYPE_EMPTY, CTYPE_UNORM, 0           },
    {D3DX_PIXEL_FORMAT_B8G8R8A8_UNORM,           { 8,  8,  8,  8}, {24, 16,  8,  0},  4, 1, 1,  4, CTYPE_UNORM, CTYPE_UNORM, 0           },
    {D3DX_PIXEL_FORMAT_B8G8R8X8_UNORM,           { 0,  8,  8,  8}, { 0, 16,  8,  0},  4, 1, 1,  4, CTYPE_EMPTY, CTYPE_UNORM, 0           },
    {D3DX_PIXEL_FORMAT_R8G8B8A8_UNORM,           { 8,  8,  8,  8}, {24,  0,  8, 16},  4, 1, 1,  4, CTYPE_UNORM, CTYPE_UNORM, 0           },
    {D3DX_PIXEL_FORMAT_R8G8B8X8_UNORM,           { 0,  8,  8,  8}, { 0,  0,  8, 16},  4, 1, 1,  4, CTYPE_EMPTY, CTYPE_UNORM, 0           },
    {D3DX_PIXEL_FORMAT_B5G6R5_UNORM,             { 0,  5,  6,  5}, { 0, 11,  5,  0},  2, 1, 1,  2, CTYPE_EMPTY, CTYPE_UNORM, 0           },
    {D3DX_PIXEL_FORMAT_B5G5R5X1_UNORM,           { 0,  5,  5,  5}, { 0, 10,  5,  0},  2, 1, 1,  2, CTYPE_EMPTY, CTYPE_UNORM, 0           },
    {D3DX_PIXEL_FORMAT_B5G5R5A1_UNORM,           { 1,  5,  5,  5}, {15, 10,  5,  0},  2, 1, 1,  2, CTYPE_UNORM, CTYPE_UNORM, 0           },
    {D3DX_PIXEL_FORMAT_B2G3R3_UNORM,             { 0,  3,  3,  2}, { 0,  5,  2,  0},  1, 1, 1,  1, CTYPE_EMPTY, CTYPE_UNORM, 0           },
    {D3DX_PIXEL_FORMAT_B2G3R3A8_UNORM,           { 8,  3,  3,  2}, { 8,  5,  2,  0},  2, 1, 1,  2, CTYPE_UNORM, CTYPE_UNORM, 0           },
    {D3DX_PIXEL_FORMAT_B4G4R4A4_UNORM,           { 4,  4,  4,  4}, {12,  8,  4,  0},  2, 1, 1,  2, CTYPE_UNORM, CTYPE_UNORM, 0           },
    {D3DX_PIXEL_FORMAT_B4G4R4X4_UNORM,           { 0,  4,  4,  4}, { 0,  8,  4,  0},  2, 1, 1,  2, CTYPE_EMPTY, CTYPE_UNORM, 0           },
    {D3DX_PIXEL_FORMAT_B10G10R10A2_UNORM,        { 2, 10, 10, 10}, {30, 20, 10,  0},  4, 1, 1,  4, CTYPE_UNORM, CTYPE_UNORM, 0           },
    {D3DX_PIXEL_FORMAT_R10G10B10A2_UNORM,        { 2, 10, 10, 10}, {30,  0, 10, 20},  4, 1, 1,  4, CTYPE_UNORM, CTYPE_UNORM, 0           },
    {D3DX_PIXEL_FORMAT_R16G16B16_UNORM,          { 0, 16, 16, 16}, { 0,  0, 16, 32},  6, 1, 1,  6, CTYPE_EMPTY, CTYPE_UNORM, FMT_FLAG_INTERNAL},
    {D3DX_PIXEL_FORMAT_R16G16B16A16_UNORM,       {16, 16, 16, 16}, {48,  0, 16, 32},  8, 1, 1,  8, CTYPE_UNORM, CTYPE_UNORM, 0           },
    {D3DX_PIXEL_FORMAT_R16G16_UNORM,             { 0, 16, 16,  0}, { 0,  0, 16,  0},  4, 1, 1,  4, CTYPE_EMPTY, CTYPE_UNORM, 0           },
    {D3DX_PIXEL_FORMAT_A8_UNORM,                 { 8,  0,  0,  0}, { 0,  0,  0,  0},  1, 1, 1,  1, CTYPE_UNORM, CTYPE_EMPTY, 0           },
    {D3DX_PIXEL_FORMAT_L8A8_UNORM,               { 8,  8,  0,  0}, { 8,  0,  0,  0},  2, 1, 1,  2, CTYPE_UNORM, CTYPE_LUMA,  0           },
    {D3DX_PIXEL_FORMAT_L4A4_UNORM,               { 4,  4,  0,  0}, { 4,  0,  0,  0},  1, 1, 1,  1, CTYPE_UNORM, CTYPE_LUMA,  0           },
    {D3DX_PIXEL_FORMAT_L8_UNORM,                 { 0,  8,  0,  0}, { 0,  0,  0,  0},  1, 1, 1,  1, CTYPE_EMPTY, CTYPE_LUMA,  0           },
    {D3DX_PIXEL_FORMAT_L16_UNORM,                { 0, 16,  0,  0}, { 0,  0,  0,  0},  2, 1, 1,  2, CTYPE_EMPTY, CTYPE_LUMA,  0           },
    {D3DX_PIXEL_FORMAT_DXT1_UNORM,               { 0,  0,  0,  0}, { 0,  0,  0,  0},  1, 4, 4,  8, CTYPE_UNORM, CTYPE_UNORM, FMT_FLAG_DXT},
    {D3DX_PIXEL_FORMAT_DXT2_UNORM,               { 0,  0,  0,  0}, { 0,  0,  0,  0},  1, 4, 4, 16, CTYPE_UNORM, CTYPE_UNORM, FMT_FLAG_DXT},
    {D3DX_PIXEL_FORMAT_DXT3_UNORM,               { 0,  0,  0,  0}, { 0,  0,  0,  0},  1, 4, 4, 16, CTYPE_UNORM, CTYPE_UNORM, FMT_FLAG_DXT},
    {D3DX_PIXEL_FORMAT_DXT4_UNORM,               { 0,  0,  0,  0}, { 0,  0,  0,  0},  1, 4, 4, 16, CTYPE_UNORM, CTYPE_UNORM, FMT_FLAG_DXT},
    {D3DX_PIXEL_FORMAT_DXT5_UNORM,               { 0,  0,  0,  0}, { 0,  0,  0,  0},  1, 4, 4, 16, CTYPE_UNORM, CTYPE_UNORM, FMT_FLAG_DXT},
    {D3DX_PIXEL_FORMAT_R16_FLOAT,                { 0, 16,  0,  0}, { 0,  0,  0,  0},  2, 1, 1,  2, CTYPE_EMPTY, CTYPE_FLOAT, 0           },
    {D3DX_PIXEL_FORMAT_R16G16_FLOAT,             { 0, 16, 16,  0}, { 0,  0, 16,  0},  4, 1, 1,  4, CTYPE_EMPTY, CTYPE_FLOAT, 0           },
    {D3DX_PIXEL_FORMAT_R16G16B16A16_FLOAT,       {16, 16, 16, 16}, {48,  0, 16, 32},  8, 1, 1,  8, CTYPE_FLOAT, CTYPE_FLOAT, 0           },
    {D3DX_PIXEL_FORMAT_R32_FLOAT,                { 0, 32,  0,  0}, { 0,  0,  0,  0},  4, 1, 1,  4, CTYPE_EMPTY, CTYPE_FLOAT, 0           },
    {D3DX_PIXEL_FORMAT_R32G32_FLOAT,             { 0, 32, 32,  0}, { 0,  0, 32,  0},  8, 1, 1,  8, CTYPE_EMPTY, CTYPE_FLOAT, 0           },
    {D3DX_PIXEL_FORMAT_R32G32B32A32_FLOAT,       {32, 32, 32, 32}, {96,  0, 32, 64}, 16, 1, 1, 16, CTYPE_FLOAT, CTYPE_FLOAT, 0           },
    {D3DX_PIXEL_FORMAT_P8_UINT,                  { 8,  8,  8,  8}, { 0,  0,  0,  0},  1, 1, 1,  1, CTYPE_INDEX, CTYPE_INDEX, 0           },
    {D3DX_PIXEL_FORMAT_P8_UINT_A8_UNORM,         { 8,  8,  8,  8}, { 8,  0,  0,  0},  2, 1, 1,  2, CTYPE_UNORM, CTYPE_INDEX, 0           },
    {D3DX_PIXEL_FORMAT_U8V8W8Q8_SNORM,           { 8,  8,  8,  8}, {24,  0,  8, 16},  4, 1, 1,  4, CTYPE_SNORM, CTYPE_SNORM, 0           },
    {D3DX_PIXEL_FORMAT_U16V16W16Q16_SNORM,       {16, 16, 16, 16}, {48,  0, 16, 32},  8, 1, 1,  8, CTYPE_SNORM, CTYPE_SNORM, 0           },
    {D3DX_PIXEL_FORMAT_U8V8_SNORM,               { 0,  8,  8,  0}, { 0,  0,  8,  0},  2, 1, 1,  2, CTYPE_EMPTY, CTYPE_SNORM, 0           },
    {D3DX_PIXEL_FORMAT_U16V16_SNORM,             { 0, 16, 16,  0}, { 0,  0, 16,  0},  4, 1, 1,  4, CTYPE_EMPTY, CTYPE_SNORM, 0           },
    {D3DX_PIXEL_FORMAT_U8V8_SNORM_L8X8_UNORM,    { 8,  8,  8,  0}, {16,  0,  8,  0},  4, 1, 1,  4, CTYPE_UNORM, CTYPE_SNORM, 0           },
    {D3DX_PIXEL_FORMAT_U10V10W10_SNORM_A2_UNORM, { 2, 10, 10, 10}, {30,  0, 10, 20},  4, 1, 1,  4, CTYPE_UNORM, CTYPE_SNORM, 0           },
    {D3DX_PIXEL_FORMAT_R8G8_B8G8_UNORM,          { 0,  0,  0,  0}, { 0,  0,  0,  0},  1, 2, 1,  4, CTYPE_EMPTY, CTYPE_UNORM, FMT_FLAG_PACKED},
    {D3DX_PIXEL_FORMAT_G8R8_G8B8_UNORM,          { 0,  0,  0,  0}, { 0,  0,  0,  0},  1, 2, 1,  4, CTYPE_EMPTY, CTYPE_UNORM, FMT_FLAG_PACKED},
    {D3DX_PIXEL_FORMAT_UYVY,                     { 0,  0,  0,  0}, { 0,  0,  0,  0},  1, 2, 1,  4, CTYPE_EMPTY, CTYPE_UNORM, FMT_FLAG_PACKED},
    {D3DX_PIXEL_FORMAT_YUY2,                     { 0,  0,  0,  0}, { 0,  0,  0,  0},  1, 2, 1,  4, CTYPE_EMPTY, CTYPE_UNORM, FMT_FLAG_PACKED},
    /* marks last element */
    {D3DX_PIXEL_FORMAT_COUNT,                    { 0,  0,  0,  0}, { 0,  0,  0,  0},  0, 1, 1,  0, CTYPE_EMPTY, CTYPE_EMPTY, 0           },
};

// utils.c
static enum d3dx_pixel_format_id d3dx_pixel_format_id_from_d3dformat(D3DFORMAT format)
{
    switch (format)
    {
        case D3DFMT_R8G8B8:        return D3DX_PIXEL_FORMAT_B8G8R8_UNORM;
        case D3DFMT_A8R8G8B8:      return D3DX_PIXEL_FORMAT_B8G8R8A8_UNORM;
        case D3DFMT_X8R8G8B8:      return D3DX_PIXEL_FORMAT_B8G8R8X8_UNORM;
        //case D3DFMT_A8B8G8R8:      return D3DX_PIXEL_FORMAT_R8G8B8A8_UNORM;
        //case D3DFMT_X8B8G8R8:      return D3DX_PIXEL_FORMAT_R8G8B8X8_UNORM;
        case D3DFMT_R5G6B5:        return D3DX_PIXEL_FORMAT_B5G6R5_UNORM;
        case D3DFMT_X1R5G5B5:      return D3DX_PIXEL_FORMAT_B5G5R5X1_UNORM;
        case D3DFMT_A1R5G5B5:      return D3DX_PIXEL_FORMAT_B5G5R5A1_UNORM;
        case D3DFMT_R3G3B2:        return D3DX_PIXEL_FORMAT_B2G3R3_UNORM;
        case D3DFMT_A8R3G3B2:      return D3DX_PIXEL_FORMAT_B2G3R3A8_UNORM;
        case D3DFMT_A4R4G4B4:      return D3DX_PIXEL_FORMAT_B4G4R4A4_UNORM;
        case D3DFMT_X4R4G4B4:      return D3DX_PIXEL_FORMAT_B4G4R4X4_UNORM;
        //case D3DFMT_A2R10G10B10:   return D3DX_PIXEL_FORMAT_B10G10R10A2_UNORM;
        case D3DFMT_A2B10G10R10:   return D3DX_PIXEL_FORMAT_R10G10B10A2_UNORM;
        //case D3DFMT_A16B16G16R16:  return D3DX_PIXEL_FORMAT_R16G16B16A16_UNORM;
        case D3DFMT_G16R16:        return D3DX_PIXEL_FORMAT_R16G16_UNORM;
        case D3DFMT_A8:            return D3DX_PIXEL_FORMAT_A8_UNORM;
        case D3DFMT_A8L8:          return D3DX_PIXEL_FORMAT_L8A8_UNORM;
        case D3DFMT_A4L4:          return D3DX_PIXEL_FORMAT_L4A4_UNORM;
        case D3DFMT_L8:            return D3DX_PIXEL_FORMAT_L8_UNORM;
        //case D3DFMT_L16:           return D3DX_PIXEL_FORMAT_L16_UNORM;
        case D3DFMT_DXT1:          return D3DX_PIXEL_FORMAT_DXT1_UNORM;
        case D3DFMT_DXT2:          return D3DX_PIXEL_FORMAT_DXT2_UNORM;
        case D3DFMT_DXT3:          return D3DX_PIXEL_FORMAT_DXT3_UNORM;
        case D3DFMT_DXT4:          return D3DX_PIXEL_FORMAT_DXT4_UNORM;
        case D3DFMT_DXT5:          return D3DX_PIXEL_FORMAT_DXT5_UNORM;
        //case D3DFMT_R16F:          return D3DX_PIXEL_FORMAT_R16_FLOAT;
        //case D3DFMT_G16R16F:       return D3DX_PIXEL_FORMAT_R16G16_FLOAT;
        //case D3DFMT_A16B16G16R16F: return D3DX_PIXEL_FORMAT_R16G16B16A16_FLOAT;
        //case D3DFMT_R32F:          return D3DX_PIXEL_FORMAT_R32_FLOAT;
        //case D3DFMT_G32R32F:       return D3DX_PIXEL_FORMAT_R32G32_FLOAT;
        //case D3DFMT_A32B32G32R32F: return D3DX_PIXEL_FORMAT_R32G32B32A32_FLOAT;
        case D3DFMT_P8:            return D3DX_PIXEL_FORMAT_P8_UINT;
        case D3DFMT_A8P8:          return D3DX_PIXEL_FORMAT_P8_UINT_A8_UNORM;
        case D3DFMT_Q8W8V8U8:      return D3DX_PIXEL_FORMAT_U8V8W8Q8_SNORM;
        case D3DFMT_V8U8:          return D3DX_PIXEL_FORMAT_U8V8_SNORM;
        case D3DFMT_V16U16:        return D3DX_PIXEL_FORMAT_U16V16_SNORM;
        case D3DFMT_X8L8V8U8:      return D3DX_PIXEL_FORMAT_U8V8_SNORM_L8X8_UNORM;
        case D3DFMT_A2W10V10U10:   return D3DX_PIXEL_FORMAT_U10V10W10_SNORM_A2_UNORM;
        //case D3DFMT_Q16W16V16U16:  return D3DX_PIXEL_FORMAT_U16V16W16Q16_SNORM;
        //case D3DFMT_R8G8_B8G8:     return D3DX_PIXEL_FORMAT_R8G8_B8G8_UNORM;
        //case D3DFMT_G8R8_G8B8:     return D3DX_PIXEL_FORMAT_G8R8_G8B8_UNORM;
        case D3DFMT_UYVY:          return D3DX_PIXEL_FORMAT_UYVY;
        case D3DFMT_YUY2:          return D3DX_PIXEL_FORMAT_YUY2;
        default:
            FIXME("No d3dx_pixel_format_id for D3DFORMAT %s.\n", debugstr_fourcc(format));
            return D3DX_PIXEL_FORMAT_COUNT;
    }
}

static const struct pixel_format_desc *get_d3dx_pixel_format_info(enum d3dx_pixel_format_id format)
{
    return &formats[min(format, D3DX_PIXEL_FORMAT_COUNT)];
}

/************************************************************
 * get_format_info
 *
 * Returns information about the specified format.
 * If the format is unsupported, it's filled with the D3DX_PIXEL_FORMAT_COUNT desc.
 *
 * PARAMS
 *   format [I] format whose description is queried
 *
 */
static const struct pixel_format_desc *get_format_info(D3DFORMAT format)
{
    const struct pixel_format_desc *fmt_desc = &formats[d3dx_pixel_format_id_from_d3dformat(format)];

    if (is_unknown_format(fmt_desc))
        FIXME("Unknown format %s.\n", debugstr_fourcc(format));
    return fmt_desc;
}

// surface.c

HRESULT lock_surface(IDirect3DSurface9 *surface, const RECT *surface_rect, D3DLOCKED_RECT *lock,
                     IDirect3DSurface9 **temp_surface, BOOL write)
{
  unsigned int width, height;
  IDirect3DDevice9 *device;
  D3DSURFACE_DESC desc;
  DWORD lock_flag;
  HRESULT hr;

  lock_flag = write ? 0 : D3DLOCK_READONLY;
  *temp_surface = NULL;
  if (FAILED(hr = IDirect3DSurface9_LockRect(surface, lock, surface_rect, lock_flag)))
  {
    IDirect3DSurface9_GetDevice(surface, &device);
    IDirect3DSurface9_GetDesc(surface, &desc);

    if (surface_rect)
    {
      width = surface_rect->right - surface_rect->left;
      height = surface_rect->bottom - surface_rect->top;
    }
    else
    {
      width = desc.Width;
      height = desc.Height;
    }

    hr = write ? IDirect3DDevice9_CreateOffscreenPlainSurface(device, width, height,
                                                              desc.Format, D3DPOOL_SYSTEMMEM, temp_surface, NULL)
               : IDirect3DDevice9_CreateRenderTarget(device, width, height,
                                                     desc.Format, D3DMULTISAMPLE_NONE, 0, TRUE, temp_surface, NULL);
    if (FAILED(hr))
    {
      WARN("Failed to create temporary surface, surface %p, format %#x, "
           "usage %#lx, pool %#x, write %#x, width %u, height %u.\n",
           surface, desc.Format, desc.Usage, desc.Pool, write, width, height);
      IDirect3DDevice9_Release(device);
      return hr;
    }

    if (write || SUCCEEDED(hr = IDirect3DDevice9_StretchRect(device, surface, surface_rect,
                                                             *temp_surface, NULL, D3DTEXF_NONE)))
      hr = IDirect3DSurface9_LockRect(*temp_surface, lock, NULL, lock_flag);

    IDirect3DDevice9_Release(device);
    if (FAILED(hr))
    {
      WARN("Failed to lock surface %p, write %#x, usage %#lx, pool %#x.\n",
           surface, write, desc.Usage, desc.Pool);
      IDirect3DSurface9_Release(*temp_surface);
      *temp_surface = NULL;
      return hr;
    }
    TRACE("Created temporary surface %p.\n", surface);
  }
  return hr;
}

HRESULT unlock_surface(IDirect3DSurface9 *surface, const RECT *surface_rect,
                       IDirect3DSurface9 *temp_surface, BOOL update)
{
  IDirect3DDevice9 *device;
  POINT surface_point;
  HRESULT hr;

  if (!temp_surface)
  {
    hr = IDirect3DSurface9_UnlockRect(surface);
    return hr;
  }

  hr = IDirect3DSurface9_UnlockRect(temp_surface);
  if (update)
  {
    if (surface_rect)
    {
      surface_point.x = surface_rect->left;
      surface_point.y = surface_rect->top;
    }
    else
    {
      surface_point.x = 0;
      surface_point.y = 0;
    }
    IDirect3DSurface9_GetDevice(surface, &device);
    if (FAILED(hr = IDirect3DDevice9_UpdateSurface(device, temp_surface, NULL, surface, &surface_point)))
      WARN("Updating surface failed, hr %#lx, surface %p, temp_surface %p.\n",
           hr, surface, temp_surface);
    IDirect3DDevice9_Release(device);
  }
  IDirect3DSurface9_Release(temp_surface);
  return hr;
}

// l. 1925
static HRESULT d3dx_load_surface_from_memory(IDirect3DSurface9 *dst_surface,
                                             const PALETTEENTRY *dst_palette, const RECT *dst_rect, const void *src_memory,
                                             enum d3dx_pixel_format_id src_format, uint32_t src_pitch, const PALETTEENTRY *src_palette, const RECT *src_rect,
                                             DWORD filter, D3DCOLOR color_key)
{
  const struct pixel_format_desc *src_desc, *dst_desc;
  struct d3dx_pixels src_pixels, dst_pixels;
  RECT dst_rect_tmp, dst_rect_aligned;
  IDirect3DSurface9 *surface;
  D3DLOCKED_RECT lock_rect;
  D3DSURFACE_DESC desc;
  HRESULT hr;

  IDirect3DSurface9_GetDesc(dst_surface, &desc);
  if (desc.MultiSampleType != D3DMULTISAMPLE_NONE)
  {
    TRACE("Multisampled destination surface, doing nothing.\n");
    return D3D_OK;
  }

  dst_desc = get_format_info(desc.Format);
  if (!dst_rect)
  {
    SetRect(&dst_rect_tmp, 0, 0, desc.Width, desc.Height);
    dst_rect = &dst_rect_tmp;
  }
  else
  {
    if (dst_rect->left > dst_rect->right || dst_rect->right > desc.Width || dst_rect->top > dst_rect->bottom || dst_rect->bottom > desc.Height || dst_rect->left < 0 || dst_rect->top < 0)
    {
      WARN("Invalid dst_rect specified.\n");
      return D3DERR_INVALIDCALL;
    }
    if (dst_rect->left == dst_rect->right || dst_rect->top == dst_rect->bottom)
    {
      WARN("Empty dst_rect specified.\n");
      return D3D_OK;
    }
  }

  src_desc = get_d3dx_pixel_format_info(src_format);
  hr = d3dx_pixels_init(src_memory, src_pitch, 0, src_palette, src_desc->format,
                        src_rect->left, src_rect->top, src_rect->right, src_rect->bottom, 0, 1, &src_pixels);
  if (FAILED(hr))
    return hr;

  get_aligned_rect(dst_rect->left, dst_rect->top, dst_rect->right, dst_rect->bottom, desc.Width, desc.Height,
                   dst_desc, &dst_rect_aligned);
  if (FAILED(hr = lock_surface(dst_surface, &dst_rect_aligned, &lock_rect, &surface, TRUE)))
    return hr;

  set_d3dx_pixels(&dst_pixels, lock_rect.pBits, lock_rect.Pitch, 0, dst_palette,
                  (dst_rect_aligned.right - dst_rect_aligned.left), (dst_rect_aligned.bottom - dst_rect_aligned.top), 1,
                  dst_rect);
  OffsetRect(&dst_pixels.unaligned_rect, -dst_rect_aligned.left, -dst_rect_aligned.top);

  if (FAILED(hr = d3dx_load_pixels_from_pixels(&dst_pixels, dst_desc, &src_pixels, src_desc, filter, color_key)))
  {
    unlock_surface(dst_surface, &dst_rect_aligned, surface, FALSE);
    return hr;
  }

  return unlock_surface(dst_surface, &dst_rect_aligned, surface, TRUE);
}

/************************************************************
 * D3DXLoadSurfaceFromMemory
 *
 * Loads data from a given memory chunk into a surface,
 * applying any of the specified filters.
 *
 * PARAMS
 *   pDestSurface [I] pointer to the surface
 *   pDestPalette [I] palette to use
 *   pDestRect    [I] to be filled area of the surface
 *   pSrcMemory   [I] pointer to the source data
 *   SrcFormat    [I] format of the source pixel data
 *   SrcPitch     [I] number of bytes in a row
 *   pSrcPalette  [I] palette used in the source image
 *   pSrcRect     [I] area of the source data to load
 *   dwFilter     [I] filter to apply on stretching
 *   Colorkey     [I] colorkey
 *
 * RETURNS
 *   Success: D3D_OK, if we successfully load the pixel data into our surface or
 *                    if pSrcMemory is NULL but the other parameters are valid
 *   Failure: D3DERR_INVALIDCALL, if pDestSurface, SrcPitch or pSrcRect is NULL or
 *                                if SrcFormat is an invalid format (other than D3DFMT_UNKNOWN) or
 *                                if DestRect is invalid
 *            D3DXERR_INVALIDDATA, if we fail to lock pDestSurface
 *            E_FAIL, if SrcFormat is D3DFMT_UNKNOWN or the dimensions of pSrcRect are invalid
 *
 * NOTES
 *   pSrcRect specifies the dimensions of the source data;
 *   negative values for pSrcRect are allowed as we're only looking at the width and height anyway.
 *
 */
HRESULT WINAPI D3DXLoadSurfaceFromMemory(IDirect3DSurface9 *dst_surface,
  const PALETTEENTRY *dst_palette, const RECT *dst_rect, const void *src_memory,
  D3DFORMAT src_format, UINT src_pitch, const PALETTEENTRY *src_palette, const RECT *src_rect,
  DWORD filter, D3DCOLOR color_key)
{
const struct pixel_format_desc *src_desc;
HRESULT hr;

TRACE("dst_surface %p, dst_palette %p, dst_rect %s, src_memory %p, src_format %#x, "
      "src_pitch %u, src_palette %p, src_rect %s, filter %#lx, color_key 0x%08lx.\n",
      dst_surface, dst_palette, wine_dbgstr_rect(dst_rect), src_memory, src_format,
      src_pitch, src_palette, wine_dbgstr_rect(src_rect), filter, color_key);

if (!dst_surface || !src_memory || !src_rect)
{
  WARN("Invalid argument specified.\n");
  return D3DERR_INVALIDCALL;
}

if (FAILED(hr = d3dx9_handle_load_filter(&filter)))
  return hr;

if (src_format == D3DFMT_UNKNOWN
      || src_rect->left >= src_rect->right
      || src_rect->top >= src_rect->bottom)
{
  WARN("Invalid src_format or src_rect.\n");
  return E_FAIL;
}

src_desc = get_format_info(src_format);
if (is_unknown_format(src_desc))
{
  FIXME("Unsupported format %#x.\n", src_format);
  return E_NOTIMPL;
}

return d3dx_load_surface_from_memory(dst_surface, dst_palette, dst_rect, src_memory, src_desc->format, src_pitch,
      src_palette, src_rect, filter, color_key);
}

/************************************************************
 * D3DXLoadSurfaceFromSurface
 *
 * Copies the contents from one surface to another, performing any required
 * format conversion, resizing or filtering.
 *
 * PARAMS
 *   pDestSurface [I] pointer to the destination surface
 *   pDestPalette [I] palette to use
 *   pDestRect    [I] to be filled area of the surface
 *   pSrcSurface  [I] pointer to the source surface
 *   pSrcPalette  [I] palette used for the source surface
 *   pSrcRect     [I] area of the source data to load
 *   dwFilter     [I] filter to apply on resizing
 *   Colorkey     [I] any ARGB value or 0 to disable color-keying
 *
 * RETURNS
 *   Success: D3D_OK
 *   Failure: D3DERR_INVALIDCALL, if pDestSurface or pSrcSurface is NULL
 *            D3DXERR_INVALIDDATA, if one of the surfaces is not lockable
 *
 */
HRESULT WINAPI D3DXLoadSurfaceFromSurface(IDirect3DSurface9 *dst_surface,
  const PALETTEENTRY *dst_palette, const RECT *dst_rect, IDirect3DSurface9 *src_surface,
  const PALETTEENTRY *src_palette, const RECT *src_rect, DWORD filter, D3DCOLOR color_key)
{
const struct pixel_format_desc *src_format_desc, *dst_format_desc;
D3DSURFACE_DESC src_desc, dst_desc;
struct volume src_size, dst_size;
IDirect3DSurface9 *temp_surface;
D3DTEXTUREFILTERTYPE d3d_filter;
IDirect3DDevice9 *device;
D3DLOCKED_RECT lock;
RECT dst_rect_temp;
HRESULT hr;
RECT s;

TRACE("dst_surface %p, dst_palette %p, dst_rect %s, src_surface %p, "
      "src_palette %p, src_rect %s, filter %#lx, color_key 0x%08lx.\n",
      dst_surface, dst_palette, wine_dbgstr_rect(dst_rect), src_surface,
      src_palette, wine_dbgstr_rect(src_rect), filter, color_key);

if (!dst_surface || !src_surface)
  return D3DERR_INVALIDCALL;

if (FAILED(hr = d3dx9_handle_load_filter(&filter)))
  return hr;

IDirect3DSurface9_GetDesc(src_surface, &src_desc);
src_format_desc = get_format_info(src_desc.Format);
if (!src_rect)
{
  SetRect(&s, 0, 0, src_desc.Width, src_desc.Height);
  src_rect = &s;
}
else if (src_rect->left == src_rect->right || src_rect->top == src_rect->bottom)
{
  WARN("Empty src_rect specified.\n");
  return filter == D3DX_FILTER_NONE ? D3D_OK : E_FAIL;
}
else if (src_rect->left > src_rect->right || src_rect->right > src_desc.Width
      || src_rect->left < 0 || src_rect->left > src_desc.Width
      || src_rect->top > src_rect->bottom || src_rect->bottom > src_desc.Height
      || src_rect->top < 0 || src_rect->top > src_desc.Height)
{
  WARN("Invalid src_rect specified.\n");
  return D3DERR_INVALIDCALL;
}

src_size.width = src_rect->right - src_rect->left;
src_size.height = src_rect->bottom - src_rect->top;
src_size.depth = 1;

IDirect3DSurface9_GetDesc(dst_surface, &dst_desc);
dst_format_desc = get_format_info(dst_desc.Format);
if (!dst_rect)
{
  SetRect(&dst_rect_temp, 0, 0, dst_desc.Width, dst_desc.Height);
  dst_rect = &dst_rect_temp;
}
else if (dst_rect->left == dst_rect->right || dst_rect->top == dst_rect->bottom)
{
  WARN("Empty dst_rect specified.\n");
  return filter == D3DX_FILTER_NONE ? D3D_OK : E_FAIL;
}
else if (dst_rect->left > dst_rect->right || dst_rect->right > dst_desc.Width
      || dst_rect->left < 0 || dst_rect->left > dst_desc.Width
      || dst_rect->top > dst_rect->bottom || dst_rect->bottom > dst_desc.Height
      || dst_rect->top < 0 || dst_rect->top > dst_desc.Height)
{
  WARN("Invalid dst_rect specified.\n");
  return D3DERR_INVALIDCALL;
}

dst_size.width = dst_rect->right - dst_rect->left;
dst_size.height = dst_rect->bottom - dst_rect->top;
dst_size.depth = 1;

if (!dst_palette && !src_palette && !color_key)
{
  if (src_desc.Format == dst_desc.Format
          && dst_size.width == src_size.width
          && dst_size.height == src_size.height
          && color_key == 0
          && !(src_rect->left & (src_format_desc->block_width - 1))
          && !(src_rect->top & (src_format_desc->block_height - 1))
          && !(dst_rect->left & (dst_format_desc->block_width - 1))
          && !(dst_rect->top & (dst_format_desc->block_height - 1)))
  {
      d3d_filter = D3DTEXF_NONE;
  }
  else
  {
      switch (filter)
      {
          case D3DX_FILTER_NONE:
              d3d_filter = D3DTEXF_NONE;
              break;

          case D3DX_FILTER_POINT:
              d3d_filter = D3DTEXF_POINT;
              break;

          case D3DX_FILTER_LINEAR:
              d3d_filter = D3DTEXF_LINEAR;
              break;

          default:
              d3d_filter = D3DTEXF_FORCE_DWORD;
              break;
      }
  }

  if (d3d_filter != D3DTEXF_FORCE_DWORD)
  {
      IDirect3DSurface9_GetDevice(src_surface, &device);
      //hr = IDirect3DDevice9_StretchRect(device, src_surface, src_rect, dst_surface, dst_rect, d3d_filter);
      hr = D3DERR_WRONGTEXTUREFORMAT;
      fprintf(stderr, "D3DXLoadSurfaceFromSurface: IDirect3DDevice9_StretchRect not implemented\n");
      IDirect3DDevice9_Release(device);
      if (SUCCEEDED(hr))
          return D3D_OK;
  }
}

if (FAILED(lock_surface(src_surface, NULL, &lock, &temp_surface, FALSE)))
  return D3DXERR_INVALIDDATA;

hr = D3DXLoadSurfaceFromMemory(dst_surface, dst_palette, dst_rect, lock.pBits,
      src_desc.Format, lock.Pitch, src_palette, src_rect, filter, color_key);

if (FAILED(unlock_surface(src_surface, NULL, temp_surface, FALSE)))
  return D3DXERR_INVALIDDATA;

return hr;
}