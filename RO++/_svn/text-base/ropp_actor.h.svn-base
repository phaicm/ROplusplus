// Copyright (c) 2011 Pablo Ariel Zorrilla Cepeda
//
// Permission is hereby granted, free of charge, to any person obtaining 
// a copy of this software and associated documentation files (the "Software"), 
// to deal in the Software without restriction, including without limitation the rights to use, 
// copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, 
// and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// 
// http://www.opensource.org/licenses/mit-license.php
//
#include "ropp_defines.h"

#ifndef _ROPP_ACTOR_H
#define _ROPP_ACTOR_H

namespace ropp
{
#pragma pack( push, 1 )
	//
	// Notes:
	// - magenta RGB(255,0,255) is usually displayed as an 'invisible' color
	// - images with square dimentions are recommended 
	//
	// On version 1.0 the system palette is used instead.
	// 1.1 - Added palette 
	// 2.0 - Added RGBA images
	// 2.1 - Added RLE-encoding of the background in palette images


	// Palette image (RLE-encoded background)
	// Each byte represents an index in the palette.
	// Palette index 0 is the 'invisible' background.
	// Only the background is RLE encoded.
	// When a 0x00 byte is parsed, the next byte indicates the number of 0x00 bytes it decompresses to. (0x00 0x00 decompresses to a single 0x00 byte) 
	typedef struct _PALETTE_IMAGE_RLE_DATA
	{
		uint16_t		nWidth;
		uint16_t		nHeight;
		uint16_t		nCompressedSize; //  size of the compressed data // set 0 for uncompressed images
		uint8_t*		pImageData; // image data, RLE encoded for the background. if compressedsize>0 then the size is nWidth*nHeight
	} PALETTE_IMAGE_RLE_DATA;

	// 32bpp
	typedef struct _RGBA_IMAGE_DATA
	{
		uint16_t	nWidth;
		uint16_t	nHeight;
		uint32_t*	pImageData;
	} RGBA_IMAGE_DATA;

	typedef struct _SPR_2_1_FILE_DATA
	{
		uint16_t				nMagicNumber;	// 'PS'
		uint8_t					nVersionMinor; 	
		uint8_t					nVersionMajor;	// I know it should be the other way but it seems to use this order
		uint16_t				nPaletteImages;
		uint16_t				nRGBAImages;	// v >= 2.0
		PALETTE_IMAGE_RLE_DATA*	pPaletteImages; // v >= 2.1
		RGBA_IMAGE_DATA*		pRGBAImages;	// v >= 2.0
		uint32_t				Palette[256];	// v >= 1.1
	} SPR_2_1_FILE_DATA, SPR_FILE_DATA;

	// this seems to add an offset to the sprite
	typedef struct _ACT_PIVOT_DATA
	{
	// TODO investigate. imf files are related to this data
		uint32_t	Unknown;	//???			4			??? ignored
		int32_t		CenterX;	//			4			??? offset in the frame (center is 0,0)
		int32_t		CenterY;	//			4			??? offset in the frame (center is 0,0)
		uint32_t	nAttribute;	//		4			???
	}ACT_PIVOT_DATA;

	// this structure contains info about how to map the frames from the images
	typedef struct _ACT_FRAME_DATA
	{
		int32_t		CenterX;		// offset in the frame (center is 0,0?) 
		int32_t		CenterY;		// 
		uint32_t	nFrameIndex;	// frame index in the sprite texture
		uint32_t	bMirror;		// If non zero the image is mirrored along the vertical axis
		uint32_t	nColor;			// RGBA color (for blending?) (default 0xFFFFFFFF)
		float		ScaleX;			
		float		ScaleY;
		float		fRotationAngle;			
		uint32_t	nSpriteType;	// 0 = palette image, 1 = rgb image (default 0) 
		uint32_t	nWidth;			// ??
		uint32_t	nHeight;		// ??
	}ACT_FRAME_DATA;

	typedef struct _ACT_SPRITE_DATA
	{
		int32_t			Range1[4];	//RECT{left,top,right,bottom} ?
		int32_t			Range2[4];
		uint32_t		nFrameCount;	// Number of sprite frame (max=1000) (int)
		ACT_FRAME_DATA*	lstFrameData;	// 
//		{ >= v2.0 (set to -1 on v2.0)
		uint32_t		nEventIndex; //	(default -1)
//			{	>= v2.3
		uint32_t		nPivotCount; // ??? number of anchors (default 0)
		ACT_PIVOT_DATA* lstPivotData;
//			}
//		}
	}ACT_SPRITE_DATA;

	typedef struct _ACT_ACTION_DATA
	{
		uint32_t nSpriteCount;
		ACT_SPRITE_DATA* lstSpriteData;
	}ACT_ACTION_DATA;

	typedef char ACT_EVENT_NAME[40];// event ("atk") or pathname of sound file

	typedef struct _ACT_FILE_DATA
	{
		uint16_t			nMagicNumber;
		uint8_t				nVersionMinor; 	
		uint8_t				nVersionMajor; 	
		uint16_t			nActionCount;	// Number of actions 
		uint8_t				nUknown[10];	// Whadda u know about the ways of the UnderSide?
		ACT_ACTION_DATA*	lstActionData;	
//	{ >= v2.1
		uint32_t			nEventCount; // number of events/sound files (default 0)
		ACT_EVENT_NAME* 	lstEventNames;
//		{ >= v2.2
		float*				lstFrameDelay; //sizeof(float)*nActionCount interval of action frames? (float) (default 4.0)
//		}
//	}
	}ACT_FILE_DATA;
#pragma pack( pop )

	void CleanSPRData( SPR_FILE_DATA& pSPRData );

	int32_t LoadSPRDataFromFile( const wchar_t* pFilename, SPR_FILE_DATA* pSPRData );
	int32_t LoadSPRDataFromMemory( const char* pBuffer, SPR_FILE_DATA* pSPRData );
	
	int32_t	LoadACTDataFromFile( const wchar_t* pFilename, ACT_FILE_DATA* pACTData );
	int32_t	LoadACTDataFromFile( const char* pFilename, ACT_FILE_DATA* pACTData );
	void	CleanACTData( ACT_FILE_DATA& ActData );

	int32_t BuildTexelsFromPaletteImage( const PALETTE_IMAGE_RLE_DATA* pRLEData, 
		const uint32_t* pPalette, uint32_t* out_nTexelCount, uint32_t* out_pColors );
	int32_t BuildTexelsFromRGBAImage( const RGBA_IMAGE_DATA* pRGBAData, 
		uint32_t* out_pColors, uint32_t* out_nTexelCount );
} // namespace

#endif