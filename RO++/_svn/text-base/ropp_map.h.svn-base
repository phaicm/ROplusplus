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

#ifndef _ROPP_MAP_H
#define _ROPP_MAP_H

namespace ropp
{
#pragma pack( push, 1 )
	//------------------------------------------------
	// I got some descriptions from ximosoft, 
	//
	//0 	Tile 	4 	bytes 	Float 	Left Bottom
	//4 	Tile 	4 	bytes 	Float 	Right Bottom
	//8 	Tile 	4 	bytes 	Float 	Left Top
	//12 	Tile 	4 	bytes 	Float 	Right Top
	//16 	Type 	1 	bytes 	Byte 	See below
	//
	//    0 - No walkable
	//1 - Walkable
	//2 - No walkable (No snipable)
	//3 - Walkable
	//4 - ??
	//5 - No walkable (Snipable)
	//6 - No walkable (No snipable)
	//
	typedef struct _GAT_TILE_DATA  // heightmap cell
	{
		float	fHeight[4]; // west->east, south->north ordering
		uint8_t	Type;	
		uint8_t	Unknown[3];

	}GAT_TILE_DATA;

	// Terrain information file
	//
	// Version 1.2
	typedef struct _GAT_FILE_DATA // 
	{
	/*00*/	int32_t			nMagicHeader; //  GRAT
	/*04*/	uint8_t			nVersionMajor;
	/*05*/	uint8_t			nVersionMinor;
	/*06*/	uint32_t		nWidth; // Size of field
	/*10*/	uint32_t		nDepth; // Size of field
	/*14*/	GAT_TILE_DATA*	pTiles; // Tiles 
	}GAT_FILE_DATA ;

	// 
	typedef struct _LIGHTMAP_COLOR
	{
		unsigned char r, g, b;
	}LIGHTMAP_COLOR;

	// This I will just use for completion but discard as soon as possible.
	typedef struct _GND_LIGHTMAP_DATA 
	{				
		unsigned char Brightness[8][8];
		LIGHTMAP_COLOR ColorRGB[8][8];
	}GND_LIGHTMAP_DATA;

	// U and V TexCoords follow the same order from GAT_TILE_DATA heights
	typedef struct _GND_ATTRIBUTE_DATA 
	{
		float u[4]; //< west->east, south->north ordering; 0=left 1=right
		float v[4]; //< west->east, south->north ordering; 0=up 1=down
		int16_t nTextureIndex; //< -1 for none
		int16_t nLightmapIndex; //< -1 for none?
		uint32_t dwColor;
	} GND_ATTRIBUTE_DATA;

	// v <= 1.5 defines tile attributes with a 16 bit integer instead of 32 bit from v >= 1.7 (I don't know about the v1.6)
	typedef struct _GND_TILE_DATA 
	{
		float fHeight[4]; //< west->east, south->north ordering
		int32_t TopAttributeIndex; //< -1 for none
		int32_t RightAttributeIndex; //< -1 for none
		int32_t FrontAttributeIndex; //< -1 for none
		int16_t Unknown1; // v <= 1.5 // maybe a color key? a terrain property?
	//	int16_t Unknown2; // v <= 1.5 ? it seems this is wrong and that it's just the int16 Unknown1 variable
	} GND_TILE_DATA;

	typedef struct _GND_FILE_DATA
	{
		uint32_t			nMagicHeader;
		uint8_t				nVersionMajor;
		uint8_t				nVersionMinor;
		uint32_t			nWidth;
		uint32_t			nHeight;
		float				fScale; // ?? not sure
		uint32_t			nTextureCount;
		uint32_t			nTextureStringSize;	// ? is this correct? I guess it does
		char**				ppTextureNames;		// char[nTextureStringSize]*nTextures
		uint32_t			nLightmapCount;	
		uint32_t			nLightmapWidth;	
		uint32_t			nLightmapDepth;
		uint32_t			nLightmapTiles;		// ?? 
		GND_LIGHTMAP_DATA*	pLightmapData;		// GND_LIGHTMAP_DATA[nLightmaps]
		uint32_t			nAttributeCount;
		GND_ATTRIBUTE_DATA*	pAttributeData;
		GND_TILE_DATA*		pTileData;
	} GND_FILE_DATA;

	// RSW World objects can be:
	// Model = 1
	// Light = 2
	// Sound = 3
	// Effect = 4
	typedef struct _RSW_MODEL_DATA_1_3
	{
	// ------------------- v >= 1.3
	/*00*/	char szName[40];		// (char[40])
	/*40*/	int32_t nAnimationType;		// (int) (default 0)
	/*44*/	int32_t nAnimationSpeed;	// (float) (default 1.0, must be in range [0,100.0[)
	/*48*/	int32_t nBlockType;	// (int) (default 0)
	// ------------------- v >= ?? (1.0?)
	/*52*/	char szModelName[80];	// (char[80])
	/*132*/	char szNodeName[80];	// (char[80])
	/*212*/	Vector3 vPosition;		// (vector3d)
	/*224*/	Vector3 vRotation;		// (vector3d)
	/*236*/	Vector3 vScale;		// (vector3d)
	// --------------------------
	}RSW_MODEL_DATA_1_3;

	typedef struct _RSW_SOUND_DATA_2_0 
	{
		char szName[80];
		char szWaveName[80];
		Vector3 vPosition; 
		float fVolume;
		int32_t nWidth;
		int32_t nDepth;
		float fRange;
		float fCycle; // v >= 2.0 (default 4.0)
	} RSW_SOUND_DATA_2_0;

	typedef struct _RSW_LIGHT_DATA
	{
		char szName[80];
		Vector3 vPosition;
		float fRed;				//int32_t Red; // INT?? Probably in alpha version maps? 
		float fGreen;			//int32_t Green;
		float fBlue;			//int32_t Blue;
		float fRange;
	} RSW_LIGHT_DATA;

	typedef struct _RSW_EFFECT_DATA
	{
		char			szName[80];
		Vector3			vPosition;
		int				nType;
		float			fEmissionSpeed;
		float			fParam[4];
	}RSW_EFFECT_DATA;

	typedef struct _RSW_WORLDOBJECT_DATA
	{
		uint32_t nObjectType;
		void* pObject;
	}RSW_WORLDOBJECT_DATA;

	static const uint32_t WORLDOBJECT_MODEL = 1;
	static const uint32_t WORLDOBJECT_LIGHT = 2;
	static const uint32_t WORLDOBJECT_SOUND = 3;
	static const uint32_t WORLDOBJECT_EFFECT = 4;

	// 
	// Versions:
	// 1.2 	- base version
	// 1.3 	- waterLevel added to header; 
	// 			- name, animType animSpeed, blockType added to model object
	// 1.4 	- attrFile added to header
	// 1.5 	- lightLongitude, lightLatitude, diffuseCol, ambientCol added to header
	// 1.6 	- groundTop, groundBottom, groundLeft, groundRight added to header
	// 1.7 	- unknown float added to header
	// 1.8 	- waterType, waveHeight, waveSpeed, wavePitch added to header
	// 1.9 	- waterAnimSpeed added to header
	// 2.0 	- cycle added to sound source object
	// 2.1 	- quadtree added
	typedef struct _RSW_FILE_DATA_2_1
	{
		uint32_t	nMagicNumber;		// ('WSRG'?) "GRSW" (0x47 0x52 0x53 0x57)  
		uint8_t	nVersionMajor;		// (uchar) major version
		uint8_t	nVersionMinor;		// (uchar) minor version
		char	szINIFile[40];		// (char[40]) ini file (ignored, used by alpha clients?)
		char	szGNDFile[40];		// (char[40]) gnd file
		char	szGATFile[40];		// v >= v1.4 (char[40]) gat file 
		char	szSCRFile[40];		// scr file (?)
		float	fWaterLevel;		// v >= v1.3 (default ?) 
	//{ v >= v1.8
		int32_t	nWaterType;			// (default 0)
		float	fWaveHeight;		// (default 1.0)
		float	fWaveSpeed;			//  (default 2.0)
		float	fWavePitch;			// (default 50.0) 
	//}
	// >= v1.9
		int32_t	nWaterAnimSpeed;	// (default 3)
	//{ >= v1.5
		int32_t	nLightLongitude;	// angle in degrees (default 45)
		int32_t	nLightLatitude;		// angle in degrees (default 45)
		Vector3	vColorDiffuse;		// (default vector3d(1,1,1)) (RGB?)
		Vector3	vColorAmbient;		// (default vector3d(0.3,0.3,0.3))
	//}
	// >= v1.7
		float Unknown;				// ???	(ignored)
	//{ >= v1.6
		int32_t nGroundTop;			// (default -500)
		int32_t nGroundBottom;		// (default 500)
		int32_t nGroundLeft;			// (default -500)
		int32_t nGroundRight;			// (default 500)
	//}
		uint32_t nWorldObjectCount;	// 
		RSW_WORLDOBJECT_DATA* pWorldObjectData;
		// the quadtree pointer is still missing, sorry, maybe later
	}RSW_FILE_DATA_2_1, RSW_FILE_DATA;

	void CleanGNDData( GND_FILE_DATA& GNDData );
	void CleanGATData( GAT_FILE_DATA& GATData );
	void CleanRSWData( RSW_FILE_DATA_2_1& RSWData );

	// Load file data into c sttructure
	int32_t LoadGNDDataFromFile( const wchar_t* pFilename, GND_FILE_DATA* pGNDData );
	int32_t LoadGATDataFromFile( const wchar_t* pFilename, GAT_FILE_DATA* pGATData );
	int32_t LoadRSWDataFromFile( const wchar_t* pFilename, RSW_FILE_DATA_2_1* pRSWData );
	int32_t LoadGNDDataFromFile( const char* pFilename, GND_FILE_DATA* pGNDData );
	int32_t LoadGATDataFromFile( const char* pFilename, GAT_FILE_DATA* pGATData );
	int32_t LoadRSWDataFromFile( const char* pFilename, RSW_FILE_DATA_2_1* pRSWData );

	// Load file data into c sttructure
	int32_t LoadGNDDataFromMemory( const char* pBuffer, GND_FILE_DATA* pGNDData );
	int32_t LoadGATDataFromMemory( const char* pBuffer, GAT_FILE_DATA* pGATData );
	int32_t LoadRSWDataFromMemory( const char* pBuffer, RSW_FILE_DATA_2_1* pRSWData );

	struct GND_TILE_INDICES
	{
		int VerticesTop[4];
		int VerticesFront[4];
		int VerticesRight[4];
	};

#pragma pack( pop )

	int32_t BuildTerrainGeometryFromGND( const GND_TILE_DATA* lstTileData, uint32_t nTileCount,
			const GND_ATTRIBUTE_DATA* pAttributeData, int32_t nGNDAttributeCount, uint32_t nTextureCount,
			float fHeightScale, uint32_t nColumnSubgrids, uint32_t nRowSubgrids,
			uint32_t nSubgridColumnTiles, uint32_t nSubgridRowTiles, 
			Vector3* out_pVertices, uint32_t* inout_nVertexCount, void* out_pIndices, bool* inout_b32BitIndices,
			uint32_t* inout_nIndexCount, Vector2* out_pTexCoord, Vector3* out_pNormals, 
			uint32_t* out_pAttributes, uint32_t* inout_nMeshAttributeCount, 
			MESH_ATTRIBUTE_DATA* out_lstAttributeData, GND_TILE_INDICES* out_lstTileIndices,
			MESH_NODE_DATA* out_pNodeData=0, uint32_t* out_nTopTileCount=0, 
			uint32_t* out_nFrontTileCount=0, uint32_t* out_nRightTileCount=0 ); // node count is nRowSubgrids*nColumnSubgrids

	
	GND_TILE_DATA* GetFrontTile	( GND_TILE_DATA* in_lstTiles, uint32_t nCurrentTileIndex, uint16_t nWidth, uint16_t nHeight );
	GND_TILE_DATA* GetBackTile	( GND_TILE_DATA* in_lstTiles, uint32_t nCurrentTileIndex, uint16_t nWidth, uint16_t nHeight );
	GND_TILE_DATA* GetRightTile	( GND_TILE_DATA* in_lstTiles, uint32_t nCurrentTileIndex, uint16_t nWidth, uint16_t nHeight );
	GND_TILE_DATA* GetLeftTile	( GND_TILE_DATA* in_lstTiles, uint32_t nCurrentTileIndex, uint16_t nWidth, uint16_t nHeight );
	GND_TILE_DATA* GetFrontRightTile( GND_TILE_DATA* in_lstTiles, uint32_t nCurrentTileIndex, uint16_t nWidth, uint16_t nHeight );
	GND_TILE_DATA* GetFrontLeftTile	( GND_TILE_DATA* in_lstTiles, uint32_t nCurrentTileIndex, uint16_t nWidth, uint16_t nHeight );
	GND_TILE_DATA* GetBackRightTile	( GND_TILE_DATA* in_lstTiles, uint32_t nCurrentTileIndex, uint16_t nWidth, uint16_t nHeight );
	GND_TILE_DATA* GetBackLeftTile	( GND_TILE_DATA* in_lstTiles, uint32_t nCurrentTileIndex, uint16_t nWidth, uint16_t nHeight );

	const GND_TILE_DATA* GetFrontTile	( const GND_TILE_DATA* in_lstTiles, uint32_t nCurrentTileIndex, uint16_t nWidth, uint16_t nHeight );
	const GND_TILE_DATA* GetBackTile	( const GND_TILE_DATA* in_lstTiles, uint32_t nCurrentTileIndex, uint16_t nWidth, uint16_t nHeight );
	const GND_TILE_DATA* GetRightTile	( const GND_TILE_DATA* in_lstTiles, uint32_t nCurrentTileIndex, uint16_t nWidth, uint16_t nHeight );
	const GND_TILE_DATA* GetLeftTile	( const GND_TILE_DATA* in_lstTiles, uint32_t nCurrentTileIndex, uint16_t nWidth, uint16_t nHeight );
	const GND_TILE_DATA* GetFrontRightTile( const GND_TILE_DATA* in_lstTiles, uint32_t nCurrentTileIndex, uint16_t nWidth, uint16_t nHeight );
	const GND_TILE_DATA* GetFrontLeftTile	( const GND_TILE_DATA* in_lstTiles, uint32_t nCurrentTileIndex, uint16_t nWidth, uint16_t nHeight );
	const GND_TILE_DATA* GetBackRightTile	( const GND_TILE_DATA* in_lstTiles, uint32_t nCurrentTileIndex, uint16_t nWidth, uint16_t nHeight );
	const GND_TILE_DATA* GetBackLeftTile	( const GND_TILE_DATA* in_lstTiles, uint32_t nCurrentTileIndex, uint16_t nWidth, uint16_t nHeight );

}// namespace

#endif // _ROPP_MAP_H