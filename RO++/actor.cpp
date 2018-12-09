// Copyright (c) 2011 Pablo Ariel Zorrilla Cepeda
// 
// License: http://www.opensource.org/licenses/mit-license.php
//
#include "ropp_actor.h"
#include <cstdio>
#include <iostream>

#ifdef __APPLE__
#include "utf8.h"
#endif /* __APPLE__ */

void ropp::CleanSPRData( SPR_FILE_DATA& SPRData )
{
	uint32_t i;
	for( i=0; i< SPRData.nPaletteImages; i++ )
	{
		if( SPRData.pPaletteImages[i].pImageData )
		{
			free( SPRData.pPaletteImages[i].pImageData );
			//SPRData.pPaletteImages[i].pImageData = 0;
		}
	}
	SPRData.nPaletteImages = 0; // this prevents cleaning again
	if( SPRData.pPaletteImages )
	{
		free( SPRData.pPaletteImages );
		SPRData.pPaletteImages = 0;
	}
	for( i=0; i< SPRData.nRGBAImages; i++ )
	{
		if( SPRData.pRGBAImages[i].pImageData )
		{
			free( SPRData.pRGBAImages[i].pImageData );
			//SPRData.pPaletteImages[i].pImageData = 0;
		}
	}
	SPRData.nRGBAImages = 0;
	if( SPRData.pRGBAImages )
	{
		free( SPRData.pRGBAImages );
		SPRData.pRGBAImages	= 0;
	}
}

int32_t ropp::LoadSPRDataFromFile( const wchar_t* pFilename, SPR_FILE_DATA* pSPRData )
{
	if( 0 == pFilename || 0 == pSPRData )
		return -1; // invalid args

	memset( pSPRData, 0, sizeof(SPR_FILE_DATA) ); // set contents to 0

	FILE* fp = _wfopen( pFilename, L"rb" );
	if( 0 == fp )
		return -1; // file not found

	fseek( fp, 0, SEEK_SET );

	fread( &pSPRData->nMagicNumber, sizeof( uint16_t ), 1, fp );
    const unsigned char *ptr = (unsigned char*)&(pSPRData->nMagicNumber);
	if( ptr[0] != 'S' || ptr[1] != 'P' )
	{
		fclose ( fp );
		return -1;
	}

	fread( &pSPRData->nVersionMinor, sizeof( uint8_t ), 1, fp );
	fread( &pSPRData->nVersionMajor, sizeof( uint8_t ), 1, fp );
	if( pSPRData->nVersionMajor > 2 || 
		( pSPRData->nVersionMajor == 2 && pSPRData->nVersionMinor > 1 ) )
	{
		fclose ( fp );
		return -1;
	};
	
	fread( &pSPRData->nPaletteImages, sizeof( uint16_t ), 1, fp );
	if( pSPRData->nVersionMajor > 1 )
		fread( &pSPRData->nRGBAImages, sizeof( uint16_t ), 1, fp );

	if( pSPRData->nPaletteImages > 0 ) // we just discard RLE if not supported
	{
		pSPRData->pPaletteImages = (PALETTE_IMAGE_RLE_DATA*)malloc( sizeof( PALETTE_IMAGE_RLE_DATA )*pSPRData->nPaletteImages );
		memset( pSPRData->pPaletteImages, 0, sizeof( PALETTE_IMAGE_RLE_DATA )*pSPRData->nPaletteImages );
	}
	if( pSPRData->nRGBAImages > 0 )
	{
		pSPRData->pRGBAImages = (RGBA_IMAGE_DATA*)malloc( sizeof( RGBA_IMAGE_DATA )*pSPRData->nRGBAImages );
		memset( pSPRData->pRGBAImages, 0, sizeof( RGBA_IMAGE_DATA )*pSPRData->nRGBAImages );
	}

	PALETTE_IMAGE_RLE_DATA* p; 
	uint32_t i;

	if( pSPRData->nVersionMajor > 2 ||  
		(pSPRData->nVersionMajor == 2 && pSPRData->nVersionMinor >= 1 ) ) 
	{
		for( i=0; i< pSPRData->nPaletteImages; i++ )
		{
			p = &pSPRData->pPaletteImages[i],
			fread( &p->nWidth, sizeof( uint16_t ), 1, fp );
			fread( &p->nHeight, sizeof( uint16_t ), 1, fp );
			fread( &p->nCompressedSize, sizeof( uint16_t ), 1, fp ); 
			// read compressed, uncompress when copy to texture buffer
			p->pImageData = (uint8_t*)malloc( sizeof( uint8_t )*p->nCompressedSize );
			fread( &p->pImageData[0], sizeof( uint8_t ), p->nCompressedSize, fp );
		}
	}
	else
	{
		for( i=0; i< pSPRData->nPaletteImages; i++ )
		{
			p = &pSPRData->pPaletteImages[i],
			fread( &p->nWidth, sizeof( uint16_t ), 1, fp );
			fread( &p->nHeight, sizeof( uint16_t ), 1, fp );
			p->nCompressedSize = 0; // we set to 0 to let know pImageData is not RLE processed
			p->pImageData = (uint8_t*)malloc( sizeof( uint8_t )*p->nWidth*p->nHeight );
			fread( &p->pImageData[0], sizeof( uint8_t ), p->nWidth*p->nHeight, fp );
		}
	}
	RGBA_IMAGE_DATA* p2;
	if( pSPRData->nVersionMajor >= 2 ) // 2.0 support 32bpp sprites
	{
		for( i=0; i< pSPRData->nRGBAImages; i++ )
		{
			p2 = &pSPRData->pRGBAImages[i];
			fread( &p2->nWidth, sizeof( uint16_t ), 1, fp );
			fread( &p2->nHeight, sizeof( uint16_t ), 1, fp );
			p2->pImageData = (uint32_t*)malloc( sizeof( uint32_t )*p2->nWidth*p2->nHeight );
			fread( &p2->pImageData[0], sizeof( uint32_t ), p2->nWidth*p2->nHeight, fp );
		}
	}

	if( pSPRData->nVersionMajor > 1 || 
		(pSPRData->nVersionMajor == 1 && pSPRData->nVersionMajor >= 1 ) ) // I hope version 3 still include the palette :P
	{
		fread( pSPRData->Palette, sizeof( uint32_t ), 256, fp );
	}

	fclose( fp );
	return 0;
};

int32_t ropp::LoadSPRDataFromMemory( const char* pBuffer, SPR_FILE_DATA* pSPRData )
{
	if( 0 == pBuffer || 0 == pSPRData )
		return -1; // error

	memset( pSPRData, 0, sizeof(SPR_FILE_DATA) ); // set contents to 0

	size_t pos=0;

	memcpy( &pSPRData->nMagicNumber, &pBuffer[pos], sizeof( uint16_t ) );
	pos += sizeof( uint16_t );
    const unsigned char *ptr = (unsigned char*)&(pSPRData->nMagicNumber);
	if( ptr[0] != 'S' || ptr[1] != 'P' )
	{
		return -1;
	}

	memcpy( &pSPRData->nVersionMinor, &pBuffer[pos], sizeof( uint8_t ) );
	pos += sizeof( uint8_t );
	memcpy( &pSPRData->nVersionMajor, &pBuffer[pos], sizeof( uint8_t ) );
	pos += sizeof( uint8_t );
	if( pSPRData->nVersionMajor > 2 || 
		( pSPRData->nVersionMajor == 2 && pSPRData->nVersionMinor > 1 ) )
	{
		return -1;
	};
	
	memcpy( &pSPRData->nPaletteImages, &pBuffer[pos], sizeof( uint16_t ) );
	pos += sizeof( uint16_t );
	if( pSPRData->nVersionMajor > 1 )
	{
		memcpy( &pSPRData->nRGBAImages, &pBuffer[pos], sizeof( uint16_t ) );
		pos += sizeof( uint16_t );
	}

	if( pSPRData->nPaletteImages > 0 ) // we just discard RLE if not supported
	{
		pSPRData->pPaletteImages = (PALETTE_IMAGE_RLE_DATA*)malloc( sizeof( PALETTE_IMAGE_RLE_DATA )*pSPRData->nPaletteImages );
		memset( pSPRData->pPaletteImages, 0, sizeof( PALETTE_IMAGE_RLE_DATA )*pSPRData->nPaletteImages );
	}
	if( pSPRData->nRGBAImages > 0 )
	{
		pSPRData->pRGBAImages = (RGBA_IMAGE_DATA*)malloc( sizeof( RGBA_IMAGE_DATA )*pSPRData->nRGBAImages );
		memset( pSPRData->pRGBAImages, 0, sizeof( RGBA_IMAGE_DATA )*pSPRData->nRGBAImages );
	}
	PALETTE_IMAGE_RLE_DATA* p; 
	uint32_t i;

	if( pSPRData->nVersionMajor > 2 ||  
		(pSPRData->nVersionMajor == 2 && pSPRData->nVersionMinor >= 1 ) ) 
	{
		for( i=0; i< pSPRData->nPaletteImages; i++ )
		{
			p = &pSPRData->pPaletteImages[i],
			memcpy( &p->nWidth, &pBuffer[pos], sizeof( uint16_t ) );
			pos += sizeof( uint16_t );
			memcpy( &p->nHeight, &pBuffer[pos], sizeof( uint16_t ) );
			pos += sizeof( uint16_t );
			memcpy( &p->nCompressedSize, &pBuffer[pos], sizeof( uint16_t ) ); 
			// read compressed, uncompress when copy to texture buffer
			p->pImageData = (uint8_t*)malloc( sizeof( uint8_t )*p->nCompressedSize );
			memcpy( &p->pImageData[0], &pBuffer[pos], sizeof( uint8_t )*p->nCompressedSize );
			pos += sizeof( uint8_t )*p->nCompressedSize;
		}
	}
	else
	{
		for( i=0; i< pSPRData->nPaletteImages; i++ )
		{
			p = &pSPRData->pPaletteImages[i],
			memcpy( &p->nWidth, &pBuffer[pos], sizeof( uint16_t ) );
			pos += sizeof( uint16_t );
			memcpy( &p->nHeight, &pBuffer[pos], sizeof( uint16_t ) );
			pos += sizeof( uint16_t );
			p->nCompressedSize = 0; // we set to 0 to let know pImageData is not RLE processed
			p->pImageData = (uint8_t*)malloc( sizeof( uint8_t )*p->nWidth*p->nHeight );
			memcpy( &p->pImageData[0], &pBuffer[pos], sizeof( uint8_t )*p->nWidth*p->nHeight );
			pos += sizeof( uint8_t )*p->nWidth*p->nHeight;
		}
	}
	RGBA_IMAGE_DATA* p2;
	if( pSPRData->nVersionMajor >= 2 ) // >= 2.0 support 32bpp sprites
	{
		for( i=0; i< pSPRData->nRGBAImages; i++ )
		{
			p2 = &pSPRData->pRGBAImages[i];
			memcpy( &p2->nWidth, &pBuffer[pos], sizeof( uint16_t ) );
			pos += sizeof( uint16_t );
			memcpy( &p2->nHeight, &pBuffer[pos], sizeof( uint16_t ) );
			pos += sizeof( uint16_t );
			p2->pImageData = (uint32_t*)malloc( sizeof( uint32_t )*p2->nWidth*p2->nHeight );
			memcpy( &p2->pImageData[0], &pBuffer[pos], sizeof( uint32_t )*p2->nWidth*p2->nHeight );
			pos += sizeof( uint32_t )*p2->nWidth*p2->nHeight;
		}
	}

	if( pSPRData->nVersionMajor > 1 || 
		(pSPRData->nVersionMajor == 1 && pSPRData->nVersionMajor >= 1 ) ) // I hope version 3 still include the palette :P
	{
		memcpy( pSPRData->Palette, &pBuffer[pos], sizeof( uint32_t )*256 );
		pos += sizeof( uint32_t )*256;
	}

	return 0;
};

void ropp::CleanACTData( ACT_FILE_DATA& ActData )
{
	uint32_t i, j, nSpriteCount, nActionCount=ActData.nActionCount;

	ACT_SPRITE_DATA* pCurrSprite;
	for( i=0; i<nActionCount; i++ )
	{
		nSpriteCount = ActData.lstActionData[i].nSpriteCount;
		for( j=0; j<nSpriteCount; j++ )
		{
			pCurrSprite  = &ActData.lstActionData[i].lstSpriteData[j];
			if( pCurrSprite->lstFrameData )
			{
				free( pCurrSprite->lstFrameData );
				pCurrSprite->lstFrameData	= 0;
			}
			pCurrSprite->nFrameCount = 0;
			if( pCurrSprite->lstPivotData )
			{
				free( pCurrSprite->lstPivotData );
				pCurrSprite->lstPivotData	= 0;
			}
			pCurrSprite->nPivotCount = 0;
		}
		if( ActData.lstActionData[i].lstSpriteData )
			free( ActData.lstActionData[i].lstSpriteData );
	}

	ActData.nActionCount = 0;
	if( ActData.lstActionData )
		free(ActData.lstActionData);
	ActData.nEventCount = 0;
	if( ActData.lstEventNames )
		free( ActData.lstEventNames );
	if( ActData.lstFrameDelay )
		free( ActData.lstFrameDelay );

};


int32_t ropp::LoadACTDataFromFile( const wchar_t* pFilename, ropp::ACT_FILE_DATA* pACTData )
{
	if( 0 == pFilename || 0 == pACTData )
		return -1; // invalid args

	memset( pACTData, 0, sizeof(ropp::ACT_FILE_DATA) ); // clear contents
	FILE* fp = _wfopen( pFilename, L"rb" );
	if( 0 == fp )
		return -1; // file not found

	fseek( fp, 0, SEEK_SET );	

	fread( &pACTData->nMagicNumber, 2, 1, fp );
	fread( &pACTData->nVersionMinor, 1, 1, fp );
	fread( &pACTData->nVersionMajor, 1, 1, fp );
	if( (1 > pACTData->nVersionMajor) 
		|| (1 == pACTData->nVersionMajor && 1 != pACTData->nVersionMinor) 
		|| (2 < pACTData->nVersionMajor)
		|| (2 == pACTData->nVersionMajor && 5 < pACTData->nVersionMinor) 
		)
	{// unrecognized version, cancel operation
		fclose ( fp );
		return -1;
	};

	fread( &pACTData->nActionCount, 2, 1, fp );
	fread( pACTData->nUknown, 1, 10, fp );
	pACTData->lstActionData = (ACT_ACTION_DATA*)malloc( sizeof( ACT_ACTION_DATA )*pACTData->nActionCount );
	uint32_t i, j, k, nActionCount = pACTData->nActionCount, nSpriteCount, nFrameCount, nPivotCount;
	memset( pACTData->lstActionData, 0, sizeof( ACT_ACTION_DATA )*nActionCount );

	ACT_ACTION_DATA* pCurrAction;
	ACT_FRAME_DATA* pCurrFrame;
	ACT_PIVOT_DATA* pCurrPivot;
	ACT_SPRITE_DATA* pCurrSprite;
	for( i=0; i<nActionCount; i++ )
	{
		pCurrAction = &pACTData->lstActionData[i];
		fread( &pCurrAction->nSpriteCount, 1, 4, fp );
		nSpriteCount = pCurrAction->nSpriteCount;
		pCurrAction->lstSpriteData = (ACT_SPRITE_DATA*)malloc( sizeof( ACT_SPRITE_DATA )*nSpriteCount );
		for( j=0; j<nSpriteCount; j++ )
		{		
			pCurrSprite = &pCurrAction->lstSpriteData[j];
			fread( &pCurrSprite->Range1[0], 1, 16, fp );
			fread( &pCurrSprite->Range2[0], 1, 16, fp );
			fread( &pCurrSprite->nFrameCount, 1, 4, fp );
			nFrameCount = pCurrSprite->nFrameCount;
			pCurrSprite->lstFrameData = (ACT_FRAME_DATA*)malloc( sizeof( ACT_FRAME_DATA )*nFrameCount );
			for( k=0; k<nFrameCount; k++ )
			{
				pCurrFrame = &pCurrSprite->lstFrameData[k];
				fread( &pCurrFrame->CenterX, 4, 1, fp );
				fread( &pCurrFrame->CenterY, 4, 1, fp );
				fread( &pCurrFrame->nFrameIndex, 4, 1, fp );
				fread( &pCurrFrame->bMirror, 4, 1, fp );
				if( 2 <= pACTData->nVersionMajor )
				{
					fread( &pCurrFrame->nColor, 4, 1, fp );
					fread( &pCurrFrame->ScaleX, 4, 1, fp );
					if( 2 < pACTData->nVersionMajor 
						|| ( 2 == pACTData->nVersionMajor && 4 <= pACTData->nVersionMinor )
						)
						fread( &pCurrFrame->ScaleY, 4, 1, fp );
					else
						pCurrFrame->ScaleY = pCurrFrame->ScaleX;

					fread( &pCurrFrame->fRotationAngle, 4, 1, fp );
					fread( &pCurrFrame->nSpriteType, 4, 1, fp );
					if( 2 < pACTData->nVersionMajor 
						|| ( 2 == pACTData->nVersionMajor && 5 <= pACTData->nVersionMinor )
						)
					{
						fread( &pCurrFrame->nWidth, 4, 1, fp );
						fread( &pCurrFrame->nHeight, 4, 1, fp );
					}
				}
			} // for( k, nFrameCount )
			if( 2 <= pACTData->nVersionMajor )
			{
				fread( &pCurrSprite->nEventIndex, 4, 1, fp );
				if( 2 < pACTData->nVersionMajor 
					|| ( 2 == pACTData->nVersionMajor && 3 <= pACTData->nVersionMinor )
					)
				{
					fread( &pCurrSprite->nPivotCount, 4, 1, fp );
					nPivotCount = pCurrSprite->nPivotCount;
					pCurrSprite->lstPivotData = (ACT_PIVOT_DATA*)malloc( sizeof(ACT_PIVOT_DATA)*nPivotCount );
					for( k=0; k<nPivotCount; k++ )
					{
						pCurrPivot = &pCurrSprite->lstPivotData[k];
						fread( &pCurrPivot->Unknown, 4, 1, fp );
						fread( &pCurrPivot->CenterX, 4, 1, fp );
						fread( &pCurrPivot->CenterY, 4, 1, fp );
						fread( &pCurrPivot->nAttribute, 4, 1, fp );
					}// for( k, nPivotCount )
				} // version >= 2.3
			} // version >= 2
		}// for( j, nSpriteCount )
	}// for( i, nActionCount )
	if( 2 < pACTData->nVersionMajor 
		|| ( 2 == pACTData->nVersionMajor && 1 <= pACTData->nVersionMinor )
		)
	{
		//char* pCurrEvent;
		fread( &pACTData->nEventCount, 4, 1, fp );
		uint32_t nEventCount = pACTData->nEventCount;
		pACTData->lstEventNames = (ACT_EVENT_NAME*)malloc(sizeof(ACT_EVENT_NAME)*nEventCount);
		for( i=0; i<nEventCount; i++ )
			fread( pACTData->lstEventNames[i], 1, 40, fp );
	}
	if( 2 < pACTData->nVersionMajor 
		|| ( 2 == pACTData->nVersionMajor && 2 <= pACTData->nVersionMinor )
		)
	{
		pACTData->lstFrameDelay = (float*)malloc( sizeof( float )*pACTData->nActionCount );
		fread( &pACTData->lstFrameDelay[0], 4, nActionCount, fp );
	}

	fclose( fp );
	return 0;
}


int32_t ropp::LoadACTDataFromFile( const char* pFilename, ropp::ACT_FILE_DATA* pACTData )
{
	if( 0 == pFilename || 0 == pACTData )
		return -1; // invalid args

	memset( pACTData, 0, sizeof(ropp::ACT_FILE_DATA) ); // clear contents
	FILE* fp = fopen( pFilename, "rb" );
	if( 0 == fp )
		return -1; // file not found

	fseek( fp, 0, SEEK_SET );	

	fread( &pACTData->nMagicNumber, 2, 1, fp );
	fread( &pACTData->nVersionMinor, 1, 1, fp );
	fread( &pACTData->nVersionMajor, 1, 1, fp );
	if( (1 > pACTData->nVersionMajor) 
		|| (1 == pACTData->nVersionMajor && 1 != pACTData->nVersionMinor) 
		|| (2 < pACTData->nVersionMajor)
		|| (2 == pACTData->nVersionMajor && 5 < pACTData->nVersionMinor) 
		)
	{// unrecognized version, cancel operation
		fclose ( fp );
		return -1;
	};

	fread( &pACTData->nActionCount, 2, 1, fp );
	fread( pACTData->nUknown, 1, 10, fp );
	pACTData->lstActionData = (ACT_ACTION_DATA*)malloc( sizeof( ACT_ACTION_DATA )*pACTData->nActionCount );
	uint32_t i, j, k, nActionCount = pACTData->nActionCount, nSpriteCount, nFrameCount, nPivotCount;
	memset( pACTData->lstActionData, 0, sizeof( ACT_ACTION_DATA )*nActionCount );

	ACT_ACTION_DATA* pCurrAction;
	ACT_FRAME_DATA* pCurrFrame;
	ACT_PIVOT_DATA* pCurrPivot;
	ACT_SPRITE_DATA* pCurrSprite;
	for( i=0; i<nActionCount; i++ )
	{
		pCurrAction = &pACTData->lstActionData[i];
		fread( &pCurrAction->nSpriteCount, 1, 4, fp );
		nSpriteCount = pCurrAction->nSpriteCount;
		pCurrAction->lstSpriteData = (ACT_SPRITE_DATA*)malloc( sizeof( ACT_SPRITE_DATA )*nSpriteCount );
		for( j=0; j<nSpriteCount; j++ )
		{		
			pCurrSprite = &pCurrAction->lstSpriteData[j];
			fread( &pCurrSprite->Range1[0], 1, 16, fp );
			fread( &pCurrSprite->Range2[0], 1, 16, fp );
			fread( &pCurrSprite->nFrameCount, 1, 4, fp );
			nFrameCount = pCurrSprite->nFrameCount;
			pCurrSprite->lstFrameData = (ACT_FRAME_DATA*)malloc( sizeof( ACT_FRAME_DATA )*nFrameCount );
			for( k=0; k<nFrameCount; k++ )
			{
				pCurrFrame = &pCurrSprite->lstFrameData[k];
				fread( &pCurrFrame->CenterX, 4, 1, fp );
				fread( &pCurrFrame->CenterY, 4, 1, fp );
				fread( &pCurrFrame->nFrameIndex, 4, 1, fp );
				fread( &pCurrFrame->bMirror, 4, 1, fp );
				if( 2 <= pACTData->nVersionMajor )
				{
					fread( &pCurrFrame->nColor, 4, 1, fp );
					fread( &pCurrFrame->ScaleX, 4, 1, fp );
					if( 2 < pACTData->nVersionMajor 
						|| ( 2 == pACTData->nVersionMajor && 4 <= pACTData->nVersionMinor )
						)
						fread( &pCurrFrame->ScaleY, 4, 1, fp );
					else
						pCurrFrame->ScaleY = pCurrFrame->ScaleX;

					fread( &pCurrFrame->fRotationAngle, 4, 1, fp );
					fread( &pCurrFrame->nSpriteType, 4, 1, fp );
					if( 2 < pACTData->nVersionMajor 
						|| ( 2 == pACTData->nVersionMajor && 5 <= pACTData->nVersionMinor )
						)
					{
						fread( &pCurrFrame->nWidth, 4, 1, fp );
						fread( &pCurrFrame->nHeight, 4, 1, fp );
					}
				}
			} // for( k, nFrameCount )
			if( 2 <= pACTData->nVersionMajor )
			{
				fread( &pCurrSprite->nEventIndex, 4, 1, fp );
				if( 2 < pACTData->nVersionMajor 
					|| ( 2 == pACTData->nVersionMajor && 3 <= pACTData->nVersionMinor )
					)
				{
					fread( &pCurrSprite->nPivotCount, 4, 1, fp );
					nPivotCount = pCurrSprite->nPivotCount;
					pCurrSprite->lstPivotData = (ACT_PIVOT_DATA*)malloc( sizeof(ACT_PIVOT_DATA)*nPivotCount );
					for( k=0; k<nPivotCount; k++ )
					{
						pCurrPivot = &pCurrSprite->lstPivotData[k];
						fread( &pCurrPivot->Unknown, 4, 1, fp );
						fread( &pCurrPivot->CenterX, 4, 1, fp );
						fread( &pCurrPivot->CenterY, 4, 1, fp );
						fread( &pCurrPivot->nAttribute, 4, 1, fp );
					}// for( k, nPivotCount )
				} // version >= 2.3
			} // version >= 2
		}// for( j, nSpriteCount )
	}// for( i, nActionCount )
	if( 2 < pACTData->nVersionMajor 
		|| ( 2 == pACTData->nVersionMajor && 1 <= pACTData->nVersionMinor )
		)
	{
		//char* pCurrEvent;
		fread( &pACTData->nEventCount, 4, 1, fp );
		uint32_t nEventCount = pACTData->nEventCount;
		pACTData->lstEventNames = (ACT_EVENT_NAME*)malloc(sizeof(ACT_EVENT_NAME)*nEventCount);
		for( i=0; i<nEventCount; i++ )
			fread( pACTData->lstEventNames[i], 1, 40, fp );
	}
	if( 2 < pACTData->nVersionMajor 
		|| ( 2 == pACTData->nVersionMajor && 2 <= pACTData->nVersionMinor )
		)
	{
		pACTData->lstFrameDelay = (float*)malloc( sizeof( float )*pACTData->nActionCount );
		fread( &pACTData->lstFrameDelay[0], 4, nActionCount, fp );
	}

	fclose( fp );
	return 0;
}