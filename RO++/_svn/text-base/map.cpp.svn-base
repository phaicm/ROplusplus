// Copyright (c) 2011 Pablo Ariel Zorrilla Cepeda
// 
// License: http://www.opensource.org/licenses/mit-license.php
//
#include "ropp_map.h"
#include <cstdio>
#include <iostream>

#ifdef __APPLE__
#include "utf8.h"
#endif /* __APPLE__ */

using namespace ropp;


//
void ropp::CleanGNDData( GND_FILE_DATA& GNDData )
{
	// clean textures
	uint32_t nMax = GNDData.nTextureCount;
	for( uint32_t i=0; i < nMax; i++ )
	{
		if( GNDData.ppTextureNames[i] )
			free( GNDData.ppTextureNames[i] );
	}
	GNDData.nTextureCount = 0;
	if( GNDData.ppTextureNames )
	{
		free( GNDData.ppTextureNames );
		GNDData.ppTextureNames = 0; 
	}
	// clean lightmaps
	GNDData.nLightmapCount	= 0;
	GNDData.nLightmapTiles	= 0;
	GNDData.nLightmapWidth	= 0;
	GNDData.nLightmapDepth	= 0;
	if( GNDData.pLightmapData )
	{
		free( GNDData.pLightmapData );
		GNDData.pLightmapData = 0;
	}
	// clean attributes
	GNDData.nAttributeCount = 0;
	if( GNDData.pAttributeData )
	{
		free( GNDData.pAttributeData );
		GNDData.pAttributeData = 0;
	}
	// clean tiles
	GNDData.nWidth	= 0;
	GNDData.nHeight = 0;
	if( GNDData.pTileData )
	{
		free( GNDData.pTileData );
		GNDData.pTileData = 0;
	}
	GNDData.nMagicHeader = GNDData.nVersionMajor = GNDData.nVersionMinor = 0;
};

//
void ropp::CleanGATData( GAT_FILE_DATA& GATData )
{
	if( GATData.pTiles )
		free( GATData.pTiles );
	GATData.pTiles = 0;
	GATData.nWidth = 0;
	GATData.nMagicHeader = GATData.nVersionMajor = GATData.nVersionMinor = 0;
//	GATData.nHeight = 0;
};

//
void ropp::CleanRSWData( RSW_FILE_DATA_2_1& RSWData )
{
	uint32_t nObjectCount = RSWData.nWorldObjectCount;
	RSW_LIGHT_DATA* pLightData=0;
	RSW_MODEL_DATA_1_3* pModelData=0;
	RSW_EFFECT_DATA* pEffectData=0;
	RSW_SOUND_DATA_2_0* pSoundData=0;
	uint32_t i;
	for( i=0; i< nObjectCount; i++ )
	{
		switch( RSWData.pWorldObjectData[i].nObjectType )
		{
		case WORLDOBJECT_EFFECT:
			pEffectData = ( RSW_EFFECT_DATA* )RSWData.pWorldObjectData[i].pObject;
			if(pEffectData) 
				free( pEffectData );
			continue;
		case WORLDOBJECT_LIGHT:
			pLightData = (RSW_LIGHT_DATA*)RSWData.pWorldObjectData[i].pObject;
			if(pLightData) 
				free( pLightData );
			continue;
		case WORLDOBJECT_MODEL:
			pModelData = (RSW_MODEL_DATA_1_3*)RSWData.pWorldObjectData[i].pObject;
			if(pModelData) 
				free( pModelData );
			continue;
		case WORLDOBJECT_SOUND:
			pSoundData = (RSW_SOUND_DATA_2_0*)RSWData.pWorldObjectData[i].pObject;
			if(pSoundData) 
				free( pSoundData );
			continue;
		}
	}
	RSWData.nWorldObjectCount = 0;
	if( RSWData.pWorldObjectData )
	{
		free( RSWData.pWorldObjectData );
		RSWData.pWorldObjectData = 0;
	}
	RSWData.nVersionMajor = RSWData.nVersionMinor = RSWData.nMagicNumber = 0;
};

int32_t ropp::LoadGNDDataFromFile( const wchar_t* pFilename, GND_FILE_DATA* pGNDData )
{
	if( 0 == pGNDData || 0 == pFilename )
		return -1;
	FILE* fp = _wfopen( pFilename, L"rb" );
	if( 0 == fp )
		return -1;
	fseek( fp, 0, SEEK_SET );
	fread( &pGNDData->nMagicHeader, sizeof( uint32_t ), 1, fp );
	if( pGNDData->nMagicHeader != 'NGRG' )
	{	
		fclose( fp );
		return -1;
	}
	fread( &pGNDData->nVersionMajor, sizeof( uint8_t ), 1, fp );
	fread( &pGNDData->nVersionMinor, sizeof( uint8_t ), 1, fp );
	if( ( pGNDData->nVersionMajor < 1 ) || 
		( pGNDData->nVersionMajor == 1 && pGNDData->nVersionMinor < 5 ) )
	{
		fclose( fp );
		return -1;
	}
	fread( &pGNDData->nWidth, sizeof( uint32_t ), 1, fp );
	fread( &pGNDData->nHeight, sizeof( uint32_t ), 1, fp );
	fread( &pGNDData->fScale, sizeof( float ), 1, fp );
	fread( &pGNDData->nTextureCount, sizeof( uint32_t ), 1, fp );
	fread( &pGNDData->nTextureStringSize, sizeof( uint32_t ), 1, fp );
	uint32_t i=0, nMax = pGNDData->nTextureCount, size = pGNDData->nTextureStringSize;
	pGNDData->ppTextureNames = (char**)malloc( sizeof( char* )*nMax );
	for( i=0; i < nMax; i++ )
	{
		pGNDData->ppTextureNames[i] = (char*)malloc( sizeof( char )*size );
		fread( pGNDData->ppTextureNames[i], sizeof( char ), size, fp );
	}
	fread( &pGNDData->nLightmapCount, sizeof(int32_t), 1, fp );
	fread( &pGNDData->nLightmapWidth, sizeof(int32_t), 1, fp );
	fread( &pGNDData->nLightmapDepth, sizeof(int32_t), 1, fp );
	fread( &pGNDData->nLightmapTiles, sizeof(int32_t), 1, fp ); // ??
	nMax = pGNDData->nLightmapCount;
	pGNDData->pLightmapData = (GND_LIGHTMAP_DATA*)malloc( sizeof( GND_LIGHTMAP_DATA )*nMax );
	for( i=0; i<nMax; i++ )
	{
		fread( pGNDData->pLightmapData[i].Brightness, sizeof( char ), 64, fp );
		fread( pGNDData->pLightmapData[i].ColorRGB, sizeof( char ), 3*64, fp );
	}
	fread( &pGNDData->nAttributeCount, sizeof( uint32_t ), 1, fp ); // almost there!
	nMax = pGNDData->nAttributeCount;
	pGNDData->pAttributeData = (GND_ATTRIBUTE_DATA*)malloc( sizeof( GND_ATTRIBUTE_DATA )*nMax );
	for( i=0; i<nMax; i++ )
	{
		fread( pGNDData->pAttributeData[i].u, sizeof( float ), 4, fp );
		fread( pGNDData->pAttributeData[i].v, sizeof( float ), 4, fp );
		fread( &pGNDData->pAttributeData[i].nTextureIndex, sizeof( int16_t ), 1, fp );
		fread( &pGNDData->pAttributeData[i].nLightmapIndex, sizeof( int16_t ), 1, fp );
		fread( &pGNDData->pAttributeData[i].dwColor, sizeof( uint32_t ), 1, fp );
	}
	uint32_t nTiles = pGNDData->nWidth * pGNDData->nHeight;
	pGNDData->pTileData = (GND_TILE_DATA*)malloc( sizeof( GND_TILE_DATA )*nTiles );

	if( pGNDData->nVersionMajor < 1 || 
		( pGNDData->nVersionMajor == 1 && pGNDData->nVersionMinor <= 5 ) )
	{// it seems old 1.5 format used 16 bit integers
		short top=0, front=0, right=0; // uint32_t heights[4];
		for( i=0; i<nTiles; i++ )
		{
//			fread( heights, sizeof( uint32_t ), 4, fp );
			fread( pGNDData->pTileData[i].fHeight, sizeof( float ), 4, fp );
			fread( &top, sizeof( int16_t ), 1, fp );
			fread( &right, sizeof( int16_t ), 1, fp );
			fread( &front, sizeof( int16_t ), 1, fp );
			pGNDData->pTileData[i].TopAttributeIndex = top;
			pGNDData->pTileData[i].RightAttributeIndex = right;
			pGNDData->pTileData[i].FrontAttributeIndex = front;
			fread( &pGNDData->pTileData[i].Unknown1, sizeof( int16_t ), 1, fp );
			//fread( &pGNDData->pTileData[i].Unknown2, sizeof( int16_t ), 1, fp );
		}
	}
	else if( pGNDData->nVersionMajor > 1 || 
		( pGNDData->nVersionMajor == 1 && pGNDData->nVersionMinor >= 7 ) )
	{
		for( i=0; i<nTiles; i++ )
		{
			fread( pGNDData->pTileData[i].fHeight, sizeof( float ), 4, fp );
			fread( &pGNDData->pTileData[i].TopAttributeIndex, sizeof( int32_t ), 1, fp );
			fread( &pGNDData->pTileData[i].RightAttributeIndex, sizeof( int32_t ), 1, fp );
			fread( &pGNDData->pTileData[i].FrontAttributeIndex, sizeof( int32_t ), 1, fp );
		}
	}
	fclose( fp );
	return 0;
};

int32_t ropp::LoadGNDDataFromFile( const char* pFilename, GND_FILE_DATA* pGNDData )
{
	if( 0 == pGNDData || 0 == pFilename )
		return -1;
	FILE* fp = fopen( pFilename, "rb" );
	if( 0 == fp )
		return -1;
	fseek( fp, 0, SEEK_SET );
	fread( &pGNDData->nMagicHeader, sizeof( uint32_t ), 1, fp );
	if( pGNDData->nMagicHeader != 'NGRG' )
	{	
		fclose( fp );
		return -1;
	}
	fread( &pGNDData->nVersionMajor, sizeof( uint8_t ), 1, fp );
	fread( &pGNDData->nVersionMinor, sizeof( uint8_t ), 1, fp );
	if( ( pGNDData->nVersionMajor < 1 ) || 
		( pGNDData->nVersionMajor == 1 && pGNDData->nVersionMinor < 5 ) )
	{
		fclose( fp );
		return -1;
	}
	fread( &pGNDData->nWidth, sizeof( uint32_t ), 1, fp );
	fread( &pGNDData->nHeight, sizeof( uint32_t ), 1, fp );
	fread( &pGNDData->fScale, sizeof( float ), 1, fp );
	fread( &pGNDData->nTextureCount, sizeof( uint32_t ), 1, fp );
	fread( &pGNDData->nTextureStringSize, sizeof( uint32_t ), 1, fp );
	uint32_t i=0, nMax = pGNDData->nTextureCount, size = pGNDData->nTextureStringSize;
	pGNDData->ppTextureNames = (char**)malloc( sizeof( char* )*nMax );
	for( i=0; i < nMax; i++ )
	{
		pGNDData->ppTextureNames[i] = (char*)malloc( sizeof( char )*size );
		fread( pGNDData->ppTextureNames[i], sizeof( char ), size, fp );
	}
	fread( &pGNDData->nLightmapCount, sizeof(int32_t), 1, fp );
	fread( &pGNDData->nLightmapWidth, sizeof(int32_t), 1, fp );
	fread( &pGNDData->nLightmapDepth, sizeof(int32_t), 1, fp );
	fread( &pGNDData->nLightmapTiles, sizeof(int32_t), 1, fp ); // ??
	nMax = pGNDData->nLightmapCount;
	pGNDData->pLightmapData = (GND_LIGHTMAP_DATA*)malloc( sizeof( GND_LIGHTMAP_DATA )*nMax );
	for( i=0; i<nMax; i++ )
	{
		fread( pGNDData->pLightmapData[i].Brightness, sizeof( char ), 64, fp );
		fread( pGNDData->pLightmapData[i].ColorRGB, sizeof( char ), 3*64, fp );
	}
	fread( &pGNDData->nAttributeCount, sizeof( uint32_t ), 1, fp ); // almost there!
	nMax = pGNDData->nAttributeCount;
	pGNDData->pAttributeData = (GND_ATTRIBUTE_DATA*)malloc( sizeof( GND_ATTRIBUTE_DATA )*nMax );
	for( i=0; i<nMax; i++ )
	{
		fread( pGNDData->pAttributeData[i].u, sizeof( float ), 4, fp );
		fread( pGNDData->pAttributeData[i].v, sizeof( float ), 4, fp );
		fread( &pGNDData->pAttributeData[i].nTextureIndex, sizeof( int16_t ), 1, fp );
		fread( &pGNDData->pAttributeData[i].nLightmapIndex, sizeof( int16_t ), 1, fp );
		fread( &pGNDData->pAttributeData[i].dwColor, sizeof( uint32_t ), 1, fp );
	}
	uint32_t nTiles = pGNDData->nWidth * pGNDData->nHeight;
	pGNDData->pTileData = (GND_TILE_DATA*)malloc( sizeof( GND_TILE_DATA )*nTiles );

	if( pGNDData->nVersionMajor < 1 || 
		( pGNDData->nVersionMajor == 1 && pGNDData->nVersionMinor <= 5 ) )
	{// it seems old 1.5 format used 16 bit integers
		short top=0, front=0, right=0; // uint32_t heights[4];
		for( i=0; i<nTiles; i++ )
		{
//			fread( heights, sizeof( uint32_t ), 4, fp );
			fread( pGNDData->pTileData[i].fHeight, sizeof( float ), 4, fp );
			fread( &top, sizeof( int16_t ), 1, fp );
			fread( &right, sizeof( int16_t ), 1, fp );
			fread( &front, sizeof( int16_t ), 1, fp );
			pGNDData->pTileData[i].TopAttributeIndex = top;
			pGNDData->pTileData[i].RightAttributeIndex = right;
			pGNDData->pTileData[i].FrontAttributeIndex = front;
			fread( &pGNDData->pTileData[i].Unknown1, sizeof( int16_t ), 1, fp );
			//fread( &pGNDData->pTileData[i].Unknown2, sizeof( int16_t ), 1, fp );
		}
	}
	else if( pGNDData->nVersionMajor > 1 || 
		( pGNDData->nVersionMajor == 1 && pGNDData->nVersionMinor >= 7 ) )
	{
		for( i=0; i<nTiles; i++ )
		{
			fread( pGNDData->pTileData[i].fHeight, sizeof( float ), 4, fp );
			fread( &pGNDData->pTileData[i].TopAttributeIndex, sizeof( int32_t ), 1, fp );
			fread( &pGNDData->pTileData[i].RightAttributeIndex, sizeof( int32_t ), 1, fp );
			fread( &pGNDData->pTileData[i].FrontAttributeIndex, sizeof( int32_t ), 1, fp );
		}
	}
	fclose( fp );
	return 0;
};

int32_t ropp::LoadGNDDataFromMemory( const char* pBuffer, GND_FILE_DATA* pGNDData )
{
	if( 0 == pGNDData || 0 == pBuffer )
		return -1;
	
	size_t pos = 0;
	memcpy( &pGNDData->nMagicHeader, &pBuffer[pos], sizeof( uint32_t ) );
	pos += sizeof( uint32_t );
	if( pGNDData->nMagicHeader != 'NGRG' )
	{	
		return -1;
	}
	memcpy( &pGNDData->nVersionMajor, &pBuffer[pos], sizeof( uint8_t ) );
	pos += sizeof( uint8_t );
	memcpy( &pGNDData->nVersionMinor, &pBuffer[pos], sizeof( uint8_t ) );
	pos += sizeof( uint8_t );
	if( ( pGNDData->nVersionMajor < 1 ) || 
		( pGNDData->nVersionMajor == 1 && pGNDData->nVersionMinor < 5 ) )
	{
		return -1;
	}
	memcpy( &pGNDData->nWidth, &pBuffer[pos], sizeof( uint32_t ) );
	pos += sizeof( uint32_t );
	memcpy( &pGNDData->nHeight, &pBuffer[pos], sizeof( uint32_t ) );
	pos += sizeof( uint32_t );
	memcpy( &pGNDData->fScale, &pBuffer[pos], sizeof( float ) );
	pos += sizeof( float );
	memcpy( &pGNDData->nTextureCount, &pBuffer[pos], sizeof( uint32_t ) );
	pos += sizeof( uint32_t );
	memcpy( &pGNDData->nTextureStringSize, &pBuffer[pos], sizeof( uint32_t ) );
	pos += sizeof( uint32_t );
	uint32_t i=0, nMax = pGNDData->nTextureCount, size = pGNDData->nTextureStringSize;
	pGNDData->ppTextureNames = (char**)malloc( sizeof( char* )*nMax );
	for( i=0; i < nMax; i++ )
	{
		pGNDData->ppTextureNames[i] = (char*)malloc( sizeof( char )*size );
		memcpy( pGNDData->ppTextureNames[i], &pBuffer[pos], sizeof( char )*size );
		pos += sizeof( char )*size;
	}
	memcpy( &pGNDData->nLightmapCount, &pBuffer[pos], sizeof(int32_t) );
	pos += sizeof( uint32_t );
	memcpy( &pGNDData->nLightmapWidth, &pBuffer[pos], sizeof(int32_t) );
	pos += sizeof( uint32_t );
	memcpy( &pGNDData->nLightmapDepth, &pBuffer[pos], sizeof(int32_t) );
	pos += sizeof( uint32_t );
	memcpy( &pGNDData->nLightmapTiles, &pBuffer[pos], sizeof(int32_t) ); // ??
	pos += sizeof( uint32_t );
	nMax = pGNDData->nLightmapCount;
	pGNDData->pLightmapData = (GND_LIGHTMAP_DATA*)malloc( sizeof( GND_LIGHTMAP_DATA )*nMax );
	for( i=0; i<nMax; i++ )
	{
		memcpy( pGNDData->pLightmapData[i].Brightness, &pBuffer[pos], sizeof( char )*64 );
		pos += sizeof( char )*64;
		memcpy( pGNDData->pLightmapData[i].ColorRGB, &pBuffer[pos], sizeof( char )*3*64 );
		pos += sizeof( char )*3*64;
	}
	memcpy( &pGNDData->nAttributeCount, &pBuffer[pos], sizeof( uint32_t ) ); // almost there!
	pos += sizeof( uint32_t );
	nMax = pGNDData->nAttributeCount;
	pGNDData->pAttributeData = (GND_ATTRIBUTE_DATA*)malloc( sizeof( GND_ATTRIBUTE_DATA )*nMax );
	for( i=0; i<nMax; i++ )
	{
		memcpy( &pGNDData->pAttributeData[i].u[0], &pBuffer[pos], sizeof( float )*4 );
		pos += sizeof( float )*4;
		memcpy( &pGNDData->pAttributeData[i].v[0], &pBuffer[pos], sizeof( float )*4 );
		pos += sizeof( float )*4;
		memcpy( &pGNDData->pAttributeData[i].nTextureIndex, &pBuffer[pos], sizeof( int16_t ) );
		pos += sizeof( int16_t );
		memcpy( &pGNDData->pAttributeData[i].nLightmapIndex, &pBuffer[pos], sizeof( int16_t ) );
		pos += sizeof( int16_t );
		memcpy( &pGNDData->pAttributeData[i].dwColor, &pBuffer[pos], sizeof( uint32_t ) );
		pos += sizeof( uint32_t );
	}
	uint32_t nTiles = pGNDData->nWidth * pGNDData->nHeight;
	pGNDData->pTileData = (GND_TILE_DATA*)malloc( sizeof( GND_TILE_DATA )*nTiles );

	if( pGNDData->nVersionMajor < 1 || 
		( pGNDData->nVersionMajor == 1 && pGNDData->nVersionMinor <= 5 ) )
	{// it seems old 1.5 format used 16 bit integers
		short top=0, front=0, right=0; // uint32_t heights[4];
		for( i=0; i<nTiles; i++ )
		{
			memcpy( &pGNDData->pTileData[i].fHeight[0], &pBuffer[pos], sizeof( float )*4 );
			pos += sizeof( float )*4;
			memcpy( &top, &pBuffer[pos], sizeof( int16_t ) );
			pos += sizeof( int16_t );
			memcpy( &right, &pBuffer[pos], sizeof( int16_t ) );
			pos += sizeof( int16_t );
			memcpy( &front, &pBuffer[pos], sizeof( int16_t ) );
			pos += sizeof( int16_t );
			pGNDData->pTileData[i].TopAttributeIndex = top;
			pGNDData->pTileData[i].RightAttributeIndex = right;
			pGNDData->pTileData[i].FrontAttributeIndex = front;
			memcpy( &pGNDData->pTileData[i].Unknown1, &pBuffer[pos], sizeof( int16_t ) );
			pos += sizeof( int16_t );
		}
	}
	else if( pGNDData->nVersionMajor > 1 || 
		( pGNDData->nVersionMajor == 1 && pGNDData->nVersionMinor >= 7 ) )
	{
		for( i=0; i<nTiles; i++ )
		{
			memcpy( &pGNDData->pTileData[i].fHeight[0], &pBuffer[pos], sizeof( float )*4 );
			pos += sizeof( float )*4;
			memcpy( &pGNDData->pTileData[i].TopAttributeIndex, &pBuffer[pos], sizeof( int32_t ) );
			pos += sizeof( int32_t );
			memcpy( &pGNDData->pTileData[i].RightAttributeIndex, &pBuffer[pos], sizeof( int32_t ) );
			pos += sizeof( int32_t );
			memcpy( &pGNDData->pTileData[i].FrontAttributeIndex, &pBuffer[pos], sizeof( int32_t ) );
			pos += sizeof( int32_t );
		}
	}
	return 0;
};


int32_t ropp::LoadGATDataFromFile( const wchar_t* pFilename, GAT_FILE_DATA* pGATData )
{
	if( 0 == pGATData || 0 == pFilename )
		return -1;
	FILE* fp = _wfopen( pFilename, L"rb" );
	if( 0 == fp )
		return -1;

	fseek( fp, 0, SEEK_SET );
	fread( &pGATData->nMagicHeader, sizeof( uint32_t ), 1, fp );
	if( 'TARG' != pGATData->nMagicHeader )
	{
		fclose( fp );
		return -1; // invalid GAT file
	}
	fread( &pGATData->nVersionMajor, sizeof( uint8_t ), 1, fp );
	fread( &pGATData->nVersionMinor, sizeof( uint8_t ), 1, fp );
	if( 1 != pGATData->nVersionMajor || 2 != pGATData->nVersionMinor )
	{
		fclose( fp );
		return -1;
	}
	
	fread( &pGATData->nWidth, sizeof( uint32_t ), 1, fp );
	fread( &pGATData->nDepth, sizeof( uint32_t ), 1, fp );
	
	uint32_t i, nTiles = pGATData->nWidth*pGATData->nDepth;
	pGATData->pTiles = (GAT_TILE_DATA*)malloc( sizeof( GAT_TILE_DATA )*nTiles );
	GAT_TILE_DATA* pTile;

	for( i=0; i < nTiles; i++ )
	{
		pTile = &pGATData->pTiles[i];
		fread( &pTile->fHeight[0], sizeof( float ), 4, fp );
		fread( &pTile->Type, sizeof( uint8_t ), 1, fp );
		fread( &pTile->Unknown[0], sizeof( uint8_t ), 3, fp ); // I will try to understand this later
		//fseek( fp, 3, SEEK_CUR ); 
	}

	fclose( fp );
	return 0;
};

int32_t ropp::LoadGATDataFromFile( const char* pFilename, GAT_FILE_DATA* pGATData )
{
	if( 0 == pGATData || 0 == pFilename )
		return -1;
	FILE* fp = fopen( pFilename, "rb" );
	if( 0 == fp )
		return -1;

	fseek( fp, 0, SEEK_SET );
	fread( &pGATData->nMagicHeader, sizeof( uint32_t ), 1, fp );
	if( 'TARG' != pGATData->nMagicHeader )
	{
		fclose( fp );
		return -1; // invalid GAT file
	}
	fread( &pGATData->nVersionMajor, sizeof( uint8_t ), 1, fp );
	fread( &pGATData->nVersionMinor, sizeof( uint8_t ), 1, fp );
	if( 1 != pGATData->nVersionMajor || 2 != pGATData->nVersionMinor )
	{
		fclose( fp );
		return -1;
	}
	
	fread( &pGATData->nWidth, sizeof( uint32_t ), 1, fp );
	fread( &pGATData->nDepth, sizeof( uint32_t ), 1, fp );
	
	uint32_t i, nTiles = pGATData->nWidth*pGATData->nDepth;
	pGATData->pTiles = (GAT_TILE_DATA*)malloc( sizeof( GAT_TILE_DATA )*nTiles );
	GAT_TILE_DATA* pTile;

	for( i=0; i < nTiles; i++ )
	{
		pTile = &pGATData->pTiles[i];
		fread( &pTile->fHeight[0], sizeof( float ), 4, fp );
		fread( &pTile->Type, sizeof( uint8_t ), 1, fp );
		fread( &pTile->Unknown[0], sizeof( uint8_t ), 3, fp ); // I will try to understand this later
		//fseek( fp, 3, SEEK_CUR ); 
	}

	fclose( fp );
	return 0;
};

int32_t ropp::LoadGATDataFromMemory( const char* pBuffer, GAT_FILE_DATA* pGATData )
{
	if( 0 == pGATData || 0 == pBuffer )
		return -1;

	size_t pos = 0;

	memcpy( &pGATData->nMagicHeader, &pBuffer[pos], sizeof( uint32_t ) );
	pos += sizeof( uint32_t );
	if( 'TARG' != pGATData->nMagicHeader )
	{
		return -1; // invalid GAT file
	}
	memcpy( &pGATData->nVersionMajor, &pBuffer[pos], sizeof( uint8_t ) );
	pos += sizeof( uint8_t );
	memcpy( &pGATData->nVersionMinor, &pBuffer[pos], sizeof( uint8_t ) );
	pos += sizeof( uint8_t );
	if( 1 != pGATData->nVersionMajor || 2 != pGATData->nVersionMinor )
	{ // unsupported right now
		return -1;
	}
	
	memcpy( &pGATData->nWidth, &pBuffer[pos], sizeof( uint32_t ) );
	pos += sizeof( uint32_t );
	memcpy( &pGATData->nDepth, &pBuffer[pos], sizeof( uint32_t ) );
	pos += sizeof( uint32_t );

	uint32_t i, nTiles = pGATData->nWidth*pGATData->nDepth;
	pGATData->pTiles = (GAT_TILE_DATA*)malloc( sizeof( GAT_TILE_DATA )*nTiles );
	GAT_TILE_DATA* pTile;

	for( i=0; i < nTiles; i++ )
	{
		pTile = &pGATData->pTiles[i];
		memcpy( &pTile->fHeight[0], &pBuffer[pos], sizeof( float )*4 );
		pos += sizeof( float )*4;
		memcpy( &pTile->Type, &pBuffer[pos], sizeof( uint8_t ) );
		pos += sizeof( uint8_t );
		memcpy( &pTile->Unknown[0], &pBuffer[pos], sizeof( uint8_t )*3 ); // I will try to understand this later
		pos += sizeof( uint8_t )*3;
		//fseek( fp, 3, SEEK_CUR ); 
	}
	return 0;
};

int32_t ropp::LoadRSWDataFromFile(	const wchar_t* pFilename, RSW_FILE_DATA_2_1* pRSWData )
{
	if( 0 == pRSWData || 0 == pFilename )
		return -1;
	FILE* fp = _wfopen( pFilename, L"rb" );
	if( 0 == fp )
		return -1;
	fseek( fp, 0, SEEK_SET );
	fread( &pRSWData->nMagicNumber, sizeof( uint32_t ), 1, fp );
	if( pRSWData->nMagicNumber != 'WSRG' )
	{	
		fclose( fp );
		return -1;
	}
	
	fread( &pRSWData->nVersionMajor, sizeof( uint8_t ), 1, fp );
	fread( &pRSWData->nVersionMinor , sizeof( uint8_t ), 1, fp );

	fread( pRSWData->szINIFile, sizeof( char ), 40, fp );// (char[40]) ini file (ignored, used by alpha clients?)
	fread( pRSWData->szGNDFile, sizeof( char ), 40, fp );// gnd file
	if( pRSWData->nVersionMajor > 1 || 
		(pRSWData->nVersionMajor == 1 && pRSWData->nVersionMinor >= 4) )
	{// v >= v1.4 (char[40]) gat file 
		fread( pRSWData->szGATFile, sizeof( char ), 40, fp );
	}
	fread( pRSWData->szSCRFile, sizeof( char ), 40, fp );// (char[40]) scr file 
	if( pRSWData->nVersionMajor > 1 || 
		(pRSWData->nVersionMajor == 1 && pRSWData->nVersionMinor >= 3) )
	{// v >= v1.3 (default ?) 
		fread( &pRSWData->fWaterLevel, sizeof( float ), 1, fp ); 
	};
	
	if( pRSWData->nVersionMajor > 1 || 
		(pRSWData->nVersionMajor == 1 && pRSWData->nVersionMinor >= 8) )
	{// v >= v1.8
		fread( &pRSWData->nWaterType, sizeof( int32_t ), 1, fp );// (default 0)
		fread( &pRSWData->fWaveHeight, sizeof( float ), 1, fp );// (default 1.0)
		fread( &pRSWData->fWaveSpeed, sizeof( float ), 1, fp );// (default 2.0)
		fread( &pRSWData->fWavePitch, sizeof( float ), 1, fp ); //(default 50.0) 
	}
	if( pRSWData->nVersionMajor > 1 || 
		(pRSWData->nVersionMajor == 1 && pRSWData->nVersionMinor >= 8) )
	{// v >= v1.9
		fread( &pRSWData->nWaterAnimSpeed, sizeof( int32_t ), 1, fp );
	}
	if( pRSWData->nVersionMajor > 1 || 
		(pRSWData->nVersionMajor == 1 && pRSWData->nVersionMinor >= 5) )
	{// >= v1.5
		fread( &pRSWData->nLightLongitude, sizeof( int32_t ), 1, fp ); // angle in degrees (default 45)
		fread( &pRSWData->nLightLatitude, sizeof( int32_t ), 1, fp ); // angle in degrees (default 45)
		fread( &pRSWData->vColorDiffuse.x, sizeof( float ), 3, fp );// RGB diffuse light (default white) 
		fread( &pRSWData->vColorAmbient.x, sizeof( float ), 3, fp );// RGB ambient light vector3d(0.3,0.3,0.3)
	}
	if( pRSWData->nVersionMajor > 1 || 
		(pRSWData->nVersionMajor == 1 && pRSWData->nVersionMinor >= 7) )
	{// >= v1.7
		fread( &pRSWData->Unknown, sizeof( float ), 1, fp ); // ???	(ignored)
	}

	if( pRSWData->nVersionMajor > 1 || 
		(pRSWData->nVersionMajor == 1 && pRSWData->nVersionMinor >= 6) )
	{// >= v1.6
		fread( &pRSWData->nGroundTop, sizeof( int32_t ), 1, fp ); // (default -500)
		fread( &pRSWData->nGroundBottom, sizeof( int32_t ), 1, fp ); // (default 500)
		fread( &pRSWData->nGroundLeft, sizeof( int32_t ), 1, fp ); //(default -500)
		fread( &pRSWData->nGroundRight, sizeof( int32_t ), 1, fp ); // (default 500)
	}
	fread( &pRSWData->nWorldObjectCount, sizeof( uint32_t ), 1, fp ); // 
	// create array of objects
	uint32_t nObjectCount = pRSWData->nWorldObjectCount;
	pRSWData->pWorldObjectData = (RSW_WORLDOBJECT_DATA*)malloc(sizeof( RSW_WORLDOBJECT_DATA )*nObjectCount);
	memset( &pRSWData->pWorldObjectData[0], 0, sizeof( RSW_WORLDOBJECT_DATA )*nObjectCount );

	uint32_t i;
	int32_t nType;
	RSW_LIGHT_DATA* pLightData=0;
	RSW_MODEL_DATA_1_3* pModelData=0;
	RSW_EFFECT_DATA* pEffectData=0;
	RSW_SOUND_DATA_2_0* pSoundData=0;
	RSW_WORLDOBJECT_DATA* pObjectData=0;
	for( i=0; i< nObjectCount; i++ )
	{
		pObjectData = &pRSWData->pWorldObjectData[i];
		fread( &nType, sizeof( int32_t ), 1, fp );
		pObjectData->nObjectType = nType;
		if( 1 == nType ) // model 
		{
			pModelData = (RSW_MODEL_DATA_1_3*)malloc( sizeof( RSW_MODEL_DATA_1_3 ) );
			if( pRSWData->nVersionMajor > 1 || 
				(pRSWData->nVersionMajor  == 1 && pRSWData->nVersionMinor >= 3) )
			{// ------------------- v >= 1.3
				fread( pModelData->szName, sizeof( char ), 40, fp );	// (char[40])
				fread( &pModelData->nAnimationType, sizeof( int32_t ), 1, fp );	// (int) (default 0)
				fread( &pModelData->nAnimationSpeed, sizeof( int32_t ), 1, fp );	// (float) (default 1.0, must be in range [0,100.0[)
				fread( &pModelData->nBlockType, sizeof( int32_t ), 1, fp );	// (int) (default 0)ç
			}
			// ------------------- v < 1.3
			fread( pModelData->szModelName, sizeof( char ), 80, fp );	// (char[80])
			fread( pModelData->szNodeName, sizeof( char ), 80, fp );	// (char[80])
			fread( &pModelData->vPosition.x, sizeof( float ), 3, fp );	// (vector3d)
			fread( &pModelData->vRotation.x, sizeof( float ), 3, fp );	// (vector3d)
			fread( &pModelData->vScale.x, sizeof( float ), 3, fp );	
			// done loading object
			pObjectData->pObject = pModelData;
			continue;
		}
		else if( 2 == nType ) // light 
		{
			pLightData = (RSW_LIGHT_DATA*)malloc( sizeof( RSW_LIGHT_DATA ) );
			fread( &pLightData->szName[0], sizeof( char ), 80, fp );
			fread( &pLightData->vPosition.x, sizeof( float ), 3, fp );;//			(vector3d)
			fread( &pLightData->fRed, sizeof( float ), 3, fp );
			fread( &pLightData->fRange, sizeof( float ), 1, fp );
			// done
			pObjectData->pObject = pLightData;
			continue;
		}
		else if( 3 == nType ) // sound
		{
			pSoundData = (RSW_SOUND_DATA_2_0*)malloc( sizeof( RSW_SOUND_DATA_2_0 ) );
			fread( &pSoundData->szName[0], sizeof( char ), 80, fp );
			fread( &pSoundData->szWaveName, sizeof( char ), 80, fp );
			fread( &pSoundData->vPosition, sizeof( float ), 3, fp );
			fread( &pSoundData->fVolume, sizeof( float ), 1, fp );
			fread( &pSoundData->nWidth, sizeof( int32_t ), 1, fp );
			fread( &pSoundData->nDepth, sizeof( int32_t ), 1, fp );
			fread( &pSoundData->fRange, sizeof( float ), 1, fp );
			if( pRSWData->nVersionMajor >= 2 )
			{// v >= 2.0
				fread( &pSoundData->fCycle, sizeof( float ), 1, fp ); // (default 4.0)
			}
			// done
			pObjectData->pObject = pSoundData;
			continue;
		}
		else if( 4 == nType ) // effect
		{
			pEffectData = (RSW_EFFECT_DATA*)malloc( sizeof( RSW_EFFECT_DATA ) );
			fread( &pEffectData->szName[0], sizeof( char ), 80, fp );
			fread( &pEffectData->vPosition.x, sizeof( float ), 3, fp );
			fread( &pEffectData->nType, sizeof( int32_t ), 1, fp );
			fread( &pEffectData->fEmissionSpeed, sizeof( float ), 1, fp );
			fread( &pEffectData->fParam, sizeof( float ), 4, fp );
			// done
			pObjectData->pObject = pEffectData;
			continue;
		}
		else 
		{ 

#if defined( _DEBUG ) || defined( DEBUG )
#ifdef _WIN32
//			MessageBox( NULL, L"Warning! Unknown type found on map!\n"
//				L"File loading aborted", L"LoadRSWFile", MB_ICONERROR | MB_OK ); 
#endif
#endif
			fclose( fp );
			return -1;
		}
	}
	// Here can be added some code for loading the quadtree (v >= 2.1)
	fclose( fp );
	return 0;
};

int32_t ropp::LoadRSWDataFromFile( const char* pFilename, RSW_FILE_DATA_2_1* pRSWData )
{
	if( 0 == pRSWData || 0 == pFilename )
		return -1;
	FILE* fp = fopen( pFilename, "rb" );
	if( 0 == fp )
		return -1;
	fseek( fp, 0, SEEK_SET );
	fread( &pRSWData->nMagicNumber, sizeof( uint32_t ), 1, fp );
	if( pRSWData->nMagicNumber != 'WSRG' )
	{	
		fclose( fp );
		return -1;
	}
	
	fread( &pRSWData->nVersionMajor, sizeof( uint8_t ), 1, fp );
	fread( &pRSWData->nVersionMinor , sizeof( uint8_t ), 1, fp );

	fread( pRSWData->szINIFile, sizeof( char ), 40, fp );// (char[40]) ini file (ignored, used by alpha clients?)
	fread( pRSWData->szGNDFile, sizeof( char ), 40, fp );// gnd file
	if( pRSWData->nVersionMajor > 1 || 
		(pRSWData->nVersionMajor == 1 && pRSWData->nVersionMinor >= 4) )
	{// v >= v1.4 (char[40]) gat file 
		fread( pRSWData->szGATFile, sizeof( char ), 40, fp );
	}
	fread( pRSWData->szSCRFile, sizeof( char ), 40, fp );// (char[40]) scr file 
	if( pRSWData->nVersionMajor > 1 || 
		(pRSWData->nVersionMajor == 1 && pRSWData->nVersionMinor >= 3) )
	{// v >= v1.3 (default ?) 
		fread( &pRSWData->fWaterLevel, sizeof( float ), 1, fp ); 
	};
	
	if( pRSWData->nVersionMajor > 1 || 
		(pRSWData->nVersionMajor == 1 && pRSWData->nVersionMinor >= 8) )
	{// v >= v1.8
		fread( &pRSWData->nWaterType, sizeof( int32_t ), 1, fp );// (default 0)
		fread( &pRSWData->fWaveHeight, sizeof( float ), 1, fp );// (default 1.0)
		fread( &pRSWData->fWaveSpeed, sizeof( float ), 1, fp );// (default 2.0)
		fread( &pRSWData->fWavePitch, sizeof( float ), 1, fp ); //(default 50.0) 
	}
	if( pRSWData->nVersionMajor > 1 || 
		(pRSWData->nVersionMajor == 1 && pRSWData->nVersionMinor >= 8) )
	{// v >= v1.9
		fread( &pRSWData->nWaterAnimSpeed, sizeof( int32_t ), 1, fp );
	}
	if( pRSWData->nVersionMajor > 1 || 
		(pRSWData->nVersionMajor == 1 && pRSWData->nVersionMinor >= 5) )
	{// >= v1.5
		fread( &pRSWData->nLightLongitude, sizeof( int32_t ), 1, fp ); // angle in degrees (default 45)
		fread( &pRSWData->nLightLatitude, sizeof( int32_t ), 1, fp ); // angle in degrees (default 45)
		fread( &pRSWData->vColorDiffuse.x, sizeof( float ), 3, fp );// RGB diffuse light (default white) 
		fread( &pRSWData->vColorAmbient.x, sizeof( float ), 3, fp );// RGB ambient light vector3d(0.3,0.3,0.3)
	}
	if( pRSWData->nVersionMajor > 1 || 
		(pRSWData->nVersionMajor == 1 && pRSWData->nVersionMinor >= 7) )
	{// >= v1.7
		fread( &pRSWData->Unknown, sizeof( float ), 1, fp ); // ???	(ignored)
	}

	if( pRSWData->nVersionMajor > 1 || 
		(pRSWData->nVersionMajor == 1 && pRSWData->nVersionMinor >= 6) )
	{// >= v1.6
		fread( &pRSWData->nGroundTop, sizeof( int32_t ), 1, fp ); // (default -500)
		fread( &pRSWData->nGroundBottom, sizeof( int32_t ), 1, fp ); // (default 500)
		fread( &pRSWData->nGroundLeft, sizeof( int32_t ), 1, fp ); //(default -500)
		fread( &pRSWData->nGroundRight, sizeof( int32_t ), 1, fp ); // (default 500)
	}
	fread( &pRSWData->nWorldObjectCount, sizeof( uint32_t ), 1, fp ); // 
	// create array of objects
	uint32_t nObjectCount = pRSWData->nWorldObjectCount;
	pRSWData->pWorldObjectData = (RSW_WORLDOBJECT_DATA*)malloc(sizeof( RSW_WORLDOBJECT_DATA )*nObjectCount);
	memset( &pRSWData->pWorldObjectData[0], 0, sizeof( RSW_WORLDOBJECT_DATA )*nObjectCount );

	uint32_t i;
	int32_t nType;
	RSW_LIGHT_DATA* pLightData=0;
	RSW_MODEL_DATA_1_3* pModelData=0;
	RSW_EFFECT_DATA* pEffectData=0;
	RSW_SOUND_DATA_2_0* pSoundData=0;
	RSW_WORLDOBJECT_DATA* pObjectData=0;
	for( i=0; i< nObjectCount; i++ )
	{
		pObjectData = &pRSWData->pWorldObjectData[i];
		fread( &nType, sizeof( int32_t ), 1, fp );
		pObjectData->nObjectType = nType;
		if( 1 == nType ) // model 
		{
			pModelData = (RSW_MODEL_DATA_1_3*)malloc( sizeof( RSW_MODEL_DATA_1_3 ) );
			if( pRSWData->nVersionMajor > 1 || 
				(pRSWData->nVersionMajor  == 1 && pRSWData->nVersionMinor >= 3) )
			{// ------------------- v >= 1.3
				fread( pModelData->szName, sizeof( char ), 40, fp );	// (char[40])
				fread( &pModelData->nAnimationType, sizeof( int32_t ), 1, fp );	// (int) (default 0)
				fread( &pModelData->nAnimationSpeed, sizeof( int32_t ), 1, fp );	// (float) (default 1.0, must be in range [0,100.0[)
				fread( &pModelData->nBlockType, sizeof( int32_t ), 1, fp );	// (int) (default 0)ç
			}
			// ------------------- v < 1.3
			fread( pModelData->szModelName, sizeof( char ), 80, fp );	// (char[80])
			fread( pModelData->szNodeName, sizeof( char ), 80, fp );	// (char[80])
			fread( &pModelData->vPosition.x, sizeof( float ), 3, fp );	// (vector3d)
			fread( &pModelData->vRotation.x, sizeof( float ), 3, fp );	// (vector3d)
			fread( &pModelData->vScale.x, sizeof( float ), 3, fp );	
			// done loading object
			pObjectData->pObject = pModelData;
			continue;
		}
		else if( 2 == nType ) // light 
		{
			pLightData = (RSW_LIGHT_DATA*)malloc( sizeof( RSW_LIGHT_DATA ) );
			fread( &pLightData->szName[0], sizeof( char ), 80, fp );
			fread( &pLightData->vPosition.x, sizeof( float ), 3, fp );;//			(vector3d)
			fread( &pLightData->fRed, sizeof( float ), 3, fp );
			fread( &pLightData->fRange, sizeof( float ), 1, fp );
			// done
			pObjectData->pObject = pLightData;
			continue;
		}
		else if( 3 == nType ) // sound
		{
			pSoundData = (RSW_SOUND_DATA_2_0*)malloc( sizeof( RSW_SOUND_DATA_2_0 ) );
			fread( &pSoundData->szName[0], sizeof( char ), 80, fp );
			fread( &pSoundData->szWaveName, sizeof( char ), 80, fp );
			fread( &pSoundData->vPosition, sizeof( float ), 3, fp );
			fread( &pSoundData->fVolume, sizeof( float ), 1, fp );
			fread( &pSoundData->nWidth, sizeof( int32_t ), 1, fp );
			fread( &pSoundData->nDepth, sizeof( int32_t ), 1, fp );
			fread( &pSoundData->fRange, sizeof( float ), 1, fp );
			if( pRSWData->nVersionMajor >= 2 )
			{// v >= 2.0
				fread( &pSoundData->fCycle, sizeof( float ), 1, fp ); // (default 4.0)
			}
			// done
			pObjectData->pObject = pSoundData;
			continue;
		}
		else if( 4 == nType ) // effect
		{
			pEffectData = (RSW_EFFECT_DATA*)malloc( sizeof( RSW_EFFECT_DATA ) );
			fread( &pEffectData->szName[0], sizeof( char ), 80, fp );
			fread( &pEffectData->vPosition.x, sizeof( float ), 3, fp );
			fread( &pEffectData->nType, sizeof( int32_t ), 1, fp );
			fread( &pEffectData->fEmissionSpeed, sizeof( float ), 1, fp );
			fread( &pEffectData->fParam, sizeof( float ), 4, fp );
			// done
			pObjectData->pObject = pEffectData;
			continue;
		}
		else 
		{ 

#if defined( _DEBUG ) || defined( DEBUG )
#ifdef _WIN32
//			MessageBox( NULL, L"Warning! Unknown type found on map!\n"
//				L"File loading aborted", L"LoadRSWFile", MB_ICONERROR | MB_OK ); 
#endif
#endif
			fclose( fp );
			return -1;
		}
	}
	// Here can be added some code for loading the quadtree (v >= 2.1)
	fclose( fp );
	return 0;
};

int32_t ropp::LoadRSWDataFromMemory( const char* pBuffer, RSW_FILE_DATA_2_1* pRSWData )
{
	if( 0 == pRSWData || 0 == pBuffer )
		return -1;

	size_t pos = 0; // buffer cursor pos

	memcpy( &pRSWData->nMagicNumber, &pBuffer[pos], sizeof( uint32_t ) );
	pos += sizeof( uint32_t );
	if( pRSWData->nMagicNumber != 'WSRG' )
	{	
		return -1;
	}
	
	memcpy( &pRSWData->nVersionMajor, &pBuffer[pos], sizeof( uint8_t ) );
	pos += sizeof( uint8_t );
	memcpy( &pRSWData->nVersionMinor , &pBuffer[pos], sizeof( uint8_t ) );
	pos += sizeof( uint8_t );

	memcpy( pRSWData->szINIFile, &pBuffer[pos], sizeof( char )*40 );// (char[40]) ini file (ignored, used by alpha clients?)
	pos += sizeof( char )*40;
	memcpy( pRSWData->szGNDFile, &pBuffer[pos], sizeof( char )*40 );// gnd file
	pos += sizeof( char )*40;
	if( pRSWData->nVersionMajor > 1 || ( pRSWData->nVersionMajor == 1 && pRSWData->nVersionMinor >= 4 ) )
	{// v >= v1.4 (char[40]) gat file 
		memcpy( pRSWData->szGATFile, &pBuffer[pos], sizeof( char )* 40 );
		pos += sizeof( char )*40;
	}
	memcpy( pRSWData->szSCRFile, &pBuffer[pos], sizeof( char )*40 );// (char[40]) scr file 
	pos += sizeof( char )*40;
	if( pRSWData->nVersionMajor > 1 
		|| ( pRSWData->nVersionMajor == 1 && pRSWData->nVersionMinor >= 3 ) )
	{// v >= v1.3 (default ?) 
		memcpy( &pRSWData->fWaterLevel, &pBuffer[pos], sizeof( float ) );
		pos += sizeof( float );
	};
	
	if( pRSWData->nVersionMajor > 1 || ( pRSWData->nVersionMajor == 1 && pRSWData->nVersionMinor >= 8 ) )
	{// v >= v1.8
		memcpy( &pRSWData->nWaterType, &pBuffer[pos], sizeof( int32_t ) );// (default 0)
		pos += sizeof( int32_t );
		memcpy( &pRSWData->fWaveHeight, &pBuffer[pos], sizeof( float ) );// (default 1.0)
		pos += sizeof( float );
		memcpy( &pRSWData->fWaveSpeed, &pBuffer[pos], sizeof( float ) );// (default 2.0)
		pos += sizeof( float );
		memcpy( &pRSWData->fWavePitch, &pBuffer[pos], sizeof( float ) ); //(default 50.0) 
		pos += sizeof( float );
	}
	if( pRSWData->nVersionMajor > 1 || ( pRSWData->nVersionMajor == 1 && pRSWData->nVersionMinor >= 8 ) )
	{// v >= v1.9
		memcpy( &pRSWData->nWaterAnimSpeed, &pBuffer[pos], sizeof( int32_t ) );
		pos += sizeof( int32_t );
	}
	if( pRSWData->nVersionMajor > 1 || ( pRSWData->nVersionMajor == 1 && pRSWData->nVersionMinor >= 5 ) )
	{// >= v1.5
		memcpy( &pRSWData->nLightLongitude, &pBuffer[pos], sizeof( int32_t ) ); // angle in degrees (default 45)
		pos += sizeof( int32_t );
		memcpy( &pRSWData->nLightLatitude, &pBuffer[pos], sizeof( int32_t ) ); // angle in degrees (default 45)
		pos += sizeof( int32_t );
		memcpy( &pRSWData->vColorDiffuse, &pBuffer[pos], sizeof( float )*3 );// RGB diffuse light (default white) 
		pos += sizeof( float )*3;
		memcpy( &pRSWData->vColorAmbient, &pBuffer[pos], sizeof( float )*3 );// RGB ambient light vector3d(0.3,0.3,0.3)
		pos += sizeof( float )*3;
	}
	if( pRSWData->nVersionMajor > 1 || ( pRSWData->nVersionMajor == 1 && pRSWData->nVersionMinor >= 7 ) )
	{// >= v1.7
		memcpy( &pRSWData->Unknown, &pBuffer[pos], sizeof( float ) ); // ???	(ignored)
		pos += sizeof( float );
	}

	if( pRSWData->nVersionMajor > 1 || ( pRSWData->nVersionMajor == 1 && pRSWData->nVersionMinor >= 6 ) )
	{// >= v1.6 // i'm not sure what these values are
		memcpy( &pRSWData->nGroundTop, &pBuffer[pos], sizeof( int32_t ) ); // (default -500)
		pos += sizeof( int32_t );
		memcpy( &pRSWData->nGroundBottom, &pBuffer[pos], sizeof( int32_t ) ); // (default 500)
		pos += sizeof( int32_t );
		memcpy( &pRSWData->nGroundLeft, &pBuffer[pos], sizeof( int32_t ) ); //(default -500)
		pos += sizeof( int32_t );
		memcpy( &pRSWData->nGroundRight, &pBuffer[pos], sizeof( int32_t ) ); // (default 500)
		pos += sizeof( int32_t );
	}

	memcpy( &pRSWData->nWorldObjectCount, &pBuffer[pos], sizeof( uint32_t ) ); // 
	pos += sizeof( uint32_t );
	// create array of objects
	uint32_t nObjectCount = pRSWData->nWorldObjectCount;
	pRSWData->pWorldObjectData = (RSW_WORLDOBJECT_DATA*)malloc(sizeof( RSW_WORLDOBJECT_DATA )*nObjectCount);
	memset( &pRSWData->pWorldObjectData[0], 0, sizeof( RSW_WORLDOBJECT_DATA )*nObjectCount );

	uint32_t i;
	int32_t nType;
	RSW_LIGHT_DATA* pLightData=0;
	RSW_MODEL_DATA_1_3* pModelData=0;
	RSW_EFFECT_DATA* pEffectData=0;
	RSW_SOUND_DATA_2_0* pSoundData=0;
	RSW_WORLDOBJECT_DATA* pObjectData=0;
	for( i=0; i< nObjectCount; i++ )
	{
		pObjectData = &pRSWData->pWorldObjectData[i];
		memcpy( &nType, &pBuffer[pos], sizeof( int32_t ) );
		pos += sizeof( int32_t );
		pObjectData->nObjectType = nType;
		if( 1 == nType ) // model 
		{
			pModelData = (RSW_MODEL_DATA_1_3*)malloc( sizeof( RSW_MODEL_DATA_1_3 ) );
			if( pRSWData->nVersionMajor > 1 || (pRSWData->nVersionMajor  == 1 && pRSWData->nVersionMinor >= 3) )
			{// ------------------- v >= 1.3
				memcpy( pModelData->szName, &pBuffer[pos], sizeof( char )*40 );	// (char[40])
				pos += sizeof( char )*40;	
				memcpy( &pModelData->nAnimationType, &pBuffer[pos], sizeof( int32_t ) );	// (int) (default 0)
				pos += sizeof( int32_t );
				memcpy( &pModelData->nAnimationSpeed, &pBuffer[pos], sizeof( int32_t ) );	// (float) (default 1.0, must be in range [0,100.0[)
				pos += sizeof( int32_t );
				memcpy( &pModelData->nBlockType, &pBuffer[pos], sizeof( int32_t ) );	// (int) (default 0)ç
				pos += sizeof( int32_t );
			}
			// ------------------- v < 1.3
			memcpy( pModelData->szModelName, &pBuffer[pos], sizeof( char )*80 );	// (char[80])
			pos += sizeof( char )*80;	
			memcpy( pModelData->szNodeName, &pBuffer[pos], sizeof( char )*80 );	// (char[80])
			pos += sizeof( char )*80;	
			memcpy( &pModelData->vPosition.x, &pBuffer[pos], sizeof( float ) );	// (vector3d)
			pos += sizeof( float )*3;	
			memcpy( &pModelData->vRotation.x, &pBuffer[pos], sizeof( float )*3 );	// (vector3d)
			pos += sizeof( float )*3;	
			memcpy( &pModelData->vScale.x, &pBuffer[pos], sizeof( float )*3 );	
			pos += sizeof( float )*3;	
			// done loading object
			pObjectData->pObject = pModelData;
			continue;
		}
		else if( 2 == nType ) // light 
		{
			pLightData = (RSW_LIGHT_DATA*)malloc( sizeof( RSW_LIGHT_DATA ) );
			memcpy( &pLightData->szName[0], &pBuffer[pos], sizeof( char )*80 );
			pos += sizeof( char )*80;
			memcpy( &pLightData->vPosition.x, &pBuffer[pos], sizeof( float )*3 );//			(vector3d)
			pos += sizeof( float )*3;	
			memcpy( &pLightData->fRed, &pBuffer[pos], sizeof( float )*3 );
			pos += sizeof( float )*3;	
			memcpy( &pLightData->fRange, &pBuffer[pos], sizeof( float ) );
			pos += sizeof( float );	
			// done
			pObjectData->pObject = pLightData;
			continue;
		}
		else if( 3 == nType ) // sound
		{
			pSoundData = (RSW_SOUND_DATA_2_0*)malloc( sizeof( RSW_SOUND_DATA_2_0 ) );
			memcpy( &pSoundData->szName[0], &pBuffer[pos], sizeof( char )*80 );
			pos += sizeof( char )*80;	
			memcpy( &pSoundData->szWaveName, &pBuffer[pos], sizeof( char )*80 );
			pos += sizeof( char )*80;	
			memcpy( &pSoundData->vPosition, &pBuffer[pos], sizeof( float )*3);
			pos += sizeof( float )*3;	
			memcpy( &pSoundData->fVolume, &pBuffer[pos], sizeof( float ) );
			pos += sizeof( float );	
			memcpy( &pSoundData->nWidth, &pBuffer[pos], sizeof( int32_t ) );
			pos += sizeof( int32_t );	
			memcpy( &pSoundData->nDepth, &pBuffer[pos], sizeof( int32_t ) );
			pos += sizeof( int32_t );	
			memcpy( &pSoundData->fRange, &pBuffer[pos], sizeof( float ) );
			pos += sizeof( float );	
			if( pRSWData->nVersionMajor >= 2 )
			{// v >= 2.0
				memcpy( &pSoundData->fCycle, &pBuffer[pos], sizeof( float ) ); // (default 4.0)
				pos += sizeof( float );
			}
			// done
			pObjectData->pObject = pSoundData;
			continue;
		}
		else if( 4 == nType ) // effect
		{
			pEffectData = (RSW_EFFECT_DATA*)malloc( sizeof( RSW_EFFECT_DATA ) );
			memcpy( &pEffectData->szName[0], &pBuffer[pos], sizeof( char )*80 );
			pos += sizeof( char )*80;	
			memcpy( &pEffectData->vPosition.x, &pBuffer[pos], sizeof( float )*3 );
			pos += sizeof( float )*3;	
			memcpy( &pEffectData->nType, &pBuffer[pos], sizeof( int32_t ) );
			pos += sizeof( int32_t );	
			memcpy( &pEffectData->fEmissionSpeed, &pBuffer[pos], sizeof( float ) );
			pos += sizeof( float );	
			memcpy( &pEffectData->fParam, &pBuffer[pos], sizeof( float )*4 );
			pos += sizeof( float )*4;	
			// done
			pObjectData->pObject = pEffectData;
			continue;
		}
		else 
		{ // I commented the windows message because you may want it to be platform-independent or just 
#if defined( _DEBUG ) || defined( DEBUG )
			//MessageBox( NULL, L"Warning! Unknown type found on map!\n"
			//	L"File loading aborted", L"LoadRSWFile", MB_ICONERROR | MB_OK ); 
#endif
			return -1;
		}
	}
	// Here should be added some code for loading the quadtree (v >= 2.1)
	return 0;
};

