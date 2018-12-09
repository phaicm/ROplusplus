// Copyright (c) 2011 Pablo Ariel Zorrilla Cepeda
// 
// License: http://www.opensource.org/licenses/mit-license.php
//
#include "ropp_model.h"
#include <cstdio>
#include <iostream>

#ifndef __APPLE__
#include <windows.h>
#endif

#ifdef __APPLE__
#include "utf8.h"
#endif /* __APPLE__ */


using namespace ropp;


void ropp::CleanRSMData( _RSM_FILE_DATA_1_5& pRSMData )
{
	uint32_t i=0;
	if( pRSMData.pTextureNames )
	{
		for( i=0; i<pRSMData.nTextureCount; i++ )
		{
			if( pRSMData.pTextureNames[i] )
			{
				free( pRSMData.pTextureNames[i] );
				pRSMData.pTextureNames[i] = 0;
			}
		}
		free( pRSMData.pTextureNames );
		pRSMData.pTextureNames = 0;
	}
	RSM_NODE_DATA_1_5* pNode=0;
	if( pRSMData.pNodes )
	{
		uint32_t nNodeCount = pRSMData.nNodeCount;
		for( i=0; i<nNodeCount; i++ )
		{
			pNode = &pRSMData.pNodes[i];
			if( pNode->lstVertices )
				free( pNode->lstVertices );
			if( pNode->lstAttributes )
				free( pNode->lstAttributes );
			if( pNode->pPositionKeyframes )
				free( pNode->pPositionKeyframes );
			if( pNode->pRotationKeyframes )
				free( pNode->pRotationKeyframes );
			if( pNode->lstTextureIndices )
				free( pNode->lstTextureIndices );
			if( pNode->lstTriangles )
				free( pNode->lstTriangles );
		}
		free( pRSMData.pNodes );
		pRSMData.pNodes = 0;
	}

	if( pRSMData.pVolumeBoxes )
	{
		free( pRSMData.pVolumeBoxes );
		pRSMData.pVolumeBoxes = 0;
	}
};

int32_t ropp::LoadRSMDataFromFile( const wchar_t* pFilename, _RSM_FILE_DATA_1_5* in_pRSMData )
{
	if( 0 == in_pRSMData )
		return -1;
	FILE* fp = _wfopen( pFilename, L"rb" );
	if( 0 == fp )
		return -1; // file not found

	RSM_FILE_DATA_1_5* pRSMData = in_pRSMData;

	memset( pRSMData, 0, sizeof( RSM_FILE_DATA_1_5 ) );
	uint32_t nPositionCount=0, 
		nTextureCount=0, 
		nVertexCount=0, 
		nNodeCount=0;
		//nRotationCount=0, 
		//nTriangles=0, 

	uint32_t nFileSize=0, i=0, j=0;
	fseek( fp, 0, SEEK_END );
	nFileSize = ftell( fp );
	fseek( fp, 0, SEEK_SET );

	// ----------------- Header and Version
	fread( &pRSMData->nMagicNumber, sizeof( uint32_t ), 1, fp );
	if( 'MSRG' != pRSMData->nMagicNumber )
		return -1; // invalid file
	fread( &pRSMData->nVersionMajor, sizeof( uint8_t ), 1, fp );
	fread( &pRSMData->nVersionMinor, sizeof( uint8_t ), 1, fp );

	fread( &pRSMData->nFrameCount, sizeof( uint32_t ), 1, fp );
	fread( &pRSMData->nShadeType, sizeof( int32_t ), 1, fp );

	// ----------------------------------------- Rendering properties -------------------------------
	if( ( pRSMData->nVersionMajor == 1 && pRSMData->nVersionMinor >= 4 )
		|| ( pRSMData->nVersionMajor > 1 ) )//{ >= v1.4
		fread( &pRSMData->cAlpha, sizeof( uint8_t ), 1, fp ); // base alpha of the model? (default 0xff)
	else 
		pRSMData->cAlpha = 0xFF;

	// fseek( fp, 16, SEEK_CUR );
	fread( pRSMData->Unknown, sizeof( uint8_t ), 16, fp );	//	??? (ignored)
	// ------------------ Textures
	fread( &pRSMData->nTextureCount, sizeof( int32_t ), 1, fp );	//(int) number of textures
	nTextureCount = pRSMData->nTextureCount;
	if( nTextureCount )
	{
		pRSMData->pTextureNames = (char**)malloc( sizeof( char* )*nTextureCount );
		memset( pRSMData->pTextureNames, 0, sizeof( char* )*nTextureCount );
		for( i=0; i<nTextureCount; i++ )
		{
			pRSMData->pTextureNames[i] = (char*)malloc(sizeof( char )*40);
			memset( pRSMData->pTextureNames[i], 0, sizeof( char )*40 );
			fread( pRSMData->pTextureNames[i], sizeof( char ), 40, fp );
		}
	}
	// ------------------ Nodes
	fread( pRSMData->szMainNode, sizeof( uint8_t ), 40, fp );	// name of the main node
	fread( &pRSMData->nNodeCount, sizeof( uint32_t ), 1, fp );		// (int) number of nodes
	nNodeCount = pRSMData->nNodeCount;
	if( nNodeCount )
	{
		pRSMData->pNodes = (RSM_NODE_DATA_1_5*)malloc( sizeof( RSM_NODE_DATA_1_5 )*nNodeCount );
		memset( pRSMData->pNodes, 0, sizeof( RSM_NODE_DATA_1_5 )*nNodeCount );
		RSM_NODE_DATA_1_5* pNode = 0;
		for( i=0; i<nNodeCount; i++ )
		{
			pNode = &pRSMData->pNodes[i];
			fread( pNode->szName, sizeof( uint8_t ), 40, fp ); // name of the node
			fread( pNode->szParentName, sizeof( uint8_t ), 40, fp ); // name of the parent node
			fread( &pNode->nTextureCount, sizeof( uint32_t ), 1, fp ); // number of textures used by this node
			pNode->lstTextureIndices = (uint32_t*)malloc( sizeof( uint32_t )*pNode->nTextureCount );
			fread( &pNode->lstTextureIndices[0], sizeof( uint32_t ), pNode->nTextureCount, fp );
			fread( &pNode->mTransform._11, sizeof( float ), 9, fp ); // 4x4 matrix that identifies the axis and origin of this node, see http://www.gamedev.net/reference/articles/article695.asp
			fread( &pNode->vOffset.x, sizeof( float ), 3, fp ); // translation of the node
			fread( &pNode->vPosition.x, sizeof( float ), 3, fp ); // translation of the node
			fread( &pNode->fRotation, sizeof( float ), 1, fp ); // angle of rotation around the axis in radians
			fread( &pNode->vRotationAxis.x, sizeof( float ), 3, fp ); // axis of rotation
			fread( &pNode->vScale.x, sizeof( float ), 3, fp ); // scale of the node
			// ----------------------- Vertices
			fread( &pNode->nVertexCount, sizeof( uint32_t ), 1, fp ); // number of vertices
			uint32_t nVertexCount = pNode->nVertexCount;
			pNode->lstVertices = (Vector3*)malloc( sizeof( Vector3 )*nVertexCount );
			memset( pNode->lstVertices, 0, sizeof( Vector3 )*nVertexCount );
//			uint32_t totalcoords = nVertexCount*3;
			for( j=0; j<nVertexCount; j++ )
				fread( &pNode->lstVertices[j], sizeof( float ), 3, fp );
			// ----------------------- Texture Coordinates
			fread( &pNode->nAttributeCount, sizeof( uint32_t ), 1, fp ); // number of vertices
			nVertexCount = pNode->nAttributeCount;
			if( nVertexCount )
			{
				pNode->lstAttributes = (RSM_ATTRIBUTE_DATA*)malloc( sizeof( RSM_ATTRIBUTE_DATA )*nVertexCount );
				memset( pNode->lstAttributes, 0, sizeof( RSM_ATTRIBUTE_DATA )*nVertexCount );
				if( ( pRSMData->nVersionMajor == 1 && pRSMData->nVersionMinor >= 2 )
					|| ( pRSMData->nVersionMajor > 1 ) )//{ >= v1.2
				{
					for( j=0; j<nVertexCount; j++ )
					{
						fread( &pNode->lstAttributes[j].dwColor, sizeof( uint32_t ), 1, fp );
						fread( &pNode->lstAttributes[j].u, sizeof( float ), 1, fp );
						fread( &pNode->lstAttributes[j].v, sizeof( float ), 1, fp );
					}
				}
				else 
				{
					for( j=0; j<nVertexCount; j++ )
					{
						fread( &pNode->lstAttributes[j].u, sizeof( float ), 1, fp );
						fread( &pNode->lstAttributes[j].v, sizeof( float ), 1, fp );
						pNode->lstAttributes[j].dwColor = 0xFFFFFFFFL;
					}
				}
			}
			fread( &pNode->nTriangleCount, sizeof( uint32_t ), 1, fp ); // number of faces
			uint32_t nTriangleCount = pNode->nTriangleCount;
			if( nTriangleCount )
			{
				pNode->lstTriangles = (RSM_TRIANGLE_DATA*)malloc( sizeof( RSM_TRIANGLE_DATA )*nTriangleCount );
				memset( &pNode->lstTriangles[0], 0, sizeof( RSM_TRIANGLE_DATA )*nTriangleCount );
				RSM_TRIANGLE_DATA_1_5* pTri=0;
				// ----------------------- Triangles
				for( j = 0; j<nTriangleCount; j++ )
				{
					pTri = &pNode->lstTriangles[j];
					fread( pTri->nVertexIndices, sizeof( uint16_t ), 3, fp );
					fread( pTri->nAttributeIndices, sizeof( uint16_t ), 3, fp );
					fread( &pTri->nTextureIndex, sizeof( uint16_t ), 1, fp );
					fread( &pTri->Padding, sizeof( uint16_t ), 1, fp );  // (padding) ??
					fread( &pTri->nTwoSide, sizeof( int32_t ), 1, fp ); // (int) ??
					if( ( pRSMData->nVersionMajor == 1 && pRSMData->nVersionMinor >= 2 )
						|| ( pRSMData->nVersionMajor > 1 ) )//{ >= v1.2
						fread( &pTri->nSmoothGroup, sizeof( int32_t ), 1, fp ); // (default 0)
					else 
						pTri->nSmoothGroup = 0;
				}
			}
			if( ( pRSMData->nVersionMajor == 1 && pRSMData->nVersionMinor >= 5 )
				|| ( pRSMData->nVersionMajor > 1 ) )//{ >= v1.5
			{
				fread( &pNode->nPositionCount, sizeof( uint32_t ), 1, fp ); // number of position key frames
				nPositionCount = pNode->nPositionCount;
				if( nPositionCount )
				{
					pNode->pPositionKeyframes = (RSM_POSITION_KEYFRAME_DATA_1_5*)
						malloc( sizeof( RSM_POSITION_KEYFRAME_DATA_1_5 )*nPositionCount );
					memset( pNode->pPositionKeyframes, 0, 
						sizeof( RSM_POSITION_KEYFRAME_DATA_1_5 )*nPositionCount );
					RSM_POSITION_KEYFRAME_DATA_1_5* pPosKey;
					for( j = 0; j<nPositionCount; j++ )
					{
						pPosKey = &pNode->pPositionKeyframes[j];
						fread( &pPosKey->nFrame, sizeof( uint32_t ), 1, fp );
						fread( &pPosKey->vPosition[0], sizeof( float ), 3, fp );
					}
				}
			}
			else
			{
				pNode->nPositionCount = 0;
				pNode->pPositionKeyframes = 0;
			}
			fread( &pNode->nRotationCount, sizeof( uint32_t ), 1, fp ); // number of position key frames
			uint32_t nRotationCount = pNode->nRotationCount;
			if( nRotationCount )
			{
				pNode->pRotationKeyframes = (RSM_ROTATION_KEYFRAME_DATA_1_5*)malloc( 
					sizeof( RSM_ROTATION_KEYFRAME_DATA_1_5 )* nRotationCount );
				memset( &pNode->pRotationKeyframes[0], 0, 
					sizeof( RSM_ROTATION_KEYFRAME_DATA_1_5 )*nRotationCount );
				RSM_ROTATION_KEYFRAME_DATA_1_5* pRotKey;
				for( j = 0; j<nRotationCount; j++ )
				{
					pRotKey = &pNode->pRotationKeyframes[j];
					fread( &pRotKey->nFrame, sizeof( uint32_t ), 1, fp );
					fread( &pRotKey->qRotation, sizeof( float ), 4, fp );
				}
			}
		}
	}
	fread( &pRSMData->nVolumeBoxCount, sizeof( uint32_t ), 1, fp );
	uint32_t	nVolumeBoxCount = pRSMData->nVolumeBoxCount;// number of volume boxes
	if( nVolumeBoxCount )
	{
		pRSMData->pVolumeBoxes = (RSM_VOLUME_BOX_1_5*)malloc( 
			sizeof( RSM_VOLUME_BOX_1_5 )*nVolumeBoxCount );
		memset( &pRSMData->pVolumeBoxes[0], 0, 
			sizeof( RSM_VOLUME_BOX_1_5 )*nVolumeBoxCount );
		RSM_VOLUME_BOX_1_5* pBox;
		if( ( pRSMData->nVersionMajor == 1 && pRSMData->nVersionMinor >= 3 )
			|| ( pRSMData->nVersionMajor > 1 ) )//{ >= v1.3
		{
			for( i=0; i< nVolumeBoxCount; i++ )
			{
				pBox = &pRSMData->pVolumeBoxes[i];
				fread( &pBox->vSize.x, sizeof( float ), 3, fp );
				fread( &pBox->vPosition.x, sizeof( float ), 3, fp );
				fread( &pBox->vRotation.x, sizeof( float ), 3, fp );
				fread( &pBox->nFlag, sizeof( uint32_t ), 3, fp );
			}
		}
		else
		{
			for( i=0; i< nVolumeBoxCount; i++ )
			{
				pBox = &pRSMData->pVolumeBoxes[i];
				fread( &pBox->vSize.x, sizeof( float ), 3, fp );
				fread( &pBox->vPosition.x, sizeof( float ), 3, fp );
				fread( &pBox->vRotation.x, sizeof( float ), 3, fp );
				pBox->nFlag	= 0;
			}
		}
	};
	
	
	// uint32_t nPos = ftell( fp );
	//
	//uint32_t garbage;
	//fread( &garbage, sizeof( uint32_t ), 1, fp ); 
	// the file seems to end with a uint32_t with te value 0xcccccccc

	fclose( fp );
	//
	return 0;
};