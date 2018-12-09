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
// Description by: ximosoft rolaboratory (http://rolaboratory.ximosoft.com/file-format) 
// RSM : Resource Mesh File
// --------------------------------------------------------------------------------
// Versions:
// 1.1 - base version
// 1.2 - color added to texture vertices; smoothGroup added to faces
// 1.3 - flag added to volume boxes
// 1.4 - alpha added to header
// 1.5 - position animation for each node instead of global(?)
//
// the file seems to end with a uint32_t with te value 0xcccccccc
#include "ropp_defines.h"

#ifndef _ROPP_MESH_H
#define _ROPP_MESH_H

namespace ropp
{
#pragma pack( push, 1 )
	typedef struct _RSM_POSITION_KEYFRAME_DATA_1_5
	{
		uint32_t	nFrame;
		float		vPosition[3];
	} RSM_POSITION_KEYFRAME_DATA_1_5, RSM_POSITION_KEYFRAME_DATA;


	typedef struct _RSM_ROTATION_KEYFRAME_DATA_1_5
	{
		uint32_t	nFrame;
		Quaternion	qRotation;
	} RSM_ROTATION_KEYFRAME_DATA_1_5, RSM_ROTATION_KEYFRAME_DATA;

	typedef struct _RSM_TRIANGLE_DATA_1_5
	{
		uint16_t	nVertexIndices[3]; // vertex indices
		uint16_t	nAttributeIndices[3]; // texcoord indices
		uint16_t	nTextureIndex; // texture index
		uint16_t	Padding;  // (padding) ??
		int32_t		nTwoSide; // (int) ?? is this the culling mode for the current triangle??
	//{ >= v1.2
		int32_t	nSmoothGroup; // (default 0)
	}RSM_TRIANGLE_DATA_1_5, RSM_TRIANGLE_DATA;

	typedef struct _RSM_ATTRIBUTE_DATA_1_5
	{
		float u;
		float v;
	//{ >= v1.2
		uint32_t dwColor;
	}RSM_ATTRIBUTE_DATA_1_5, RSM_ATTRIBUTE_DATA;

	typedef struct _RSM_NODE_DATA_1_5
	{
		char				szName[40]; // name of the node
		char				szParentName[40]; // name of the parent node
		uint32_t			nTextureCount; // number of textures used by this node
		uint32_t*			lstTextureIndices; // uint32_t[nTextureCount] index of the texture
		Matrix3				mTransform; // 3x3 matrix that identifies the rotation and scale of this node
		Vector3				vOffset; // translation of the node
		Vector3				vPosition; // translation of the node
		float				fRotation; // angle of rotation around the axis in radians
		Vector3				vRotationAxis; // axis of rotation
		Vector3				vScale; // scale of the node
		uint32_t			nVertexCount; // number of vertices
		Vector3*			lstVertices; // float[3*nVertexCount] // vertex/point coordinate
		uint32_t			nAttributeCount;
		RSM_ATTRIBUTE_DATA*	lstAttributes; // v < 1.2 uses 2d texture coordinates // (color=0xFFFFFFFF)?!??! NEED TO TEST
		uint32_t			nTriangleCount; // number of faces
		RSM_TRIANGLE_DATA*	lstTriangles;
	// { >= v1.5
		uint32_t			nPositionCount; // number of position key frames
		RSM_POSITION_KEYFRAME_DATA_1_5* pPositionKeyframes;
	// }
		uint32_t			nRotationCount; // number of rotation key frames
		RSM_ROTATION_KEYFRAME_DATA_1_5* pRotationKeyframes;
	} RSM_NODE_DATA_1_5;

	typedef struct _RSM_VOLUME_BOX_1_5
	{
		Vector3		vSize;
		Vector3		vPosition;
		Vector3		vRotation;
		// v > 1.3 
		uint32_t nFlag; // (default 0) // if( flag != 0 ) size += vector3d(3,0,3);
	} RSM_VOLUME_BOX_1_5;

	// Shading (this is actually the way normals are calculated and if they should be used or not):
	// 0 - no shading
	// 1 - flat shading
	// 2 - smooth shading
	// 3 - black
	typedef struct _RSM_FILE_DATA_1_5
	{
		uint32_t			nMagicNumber; // "GRSM" 'MSRG'
		uint8_t				nVersionMajor;
		uint8_t				nVersionMinor;
		uint32_t			nFrameCount;
		int32_t				nShadeType;
	//{ >= v1.4
		uint8_t				cAlpha;//	alpha of the model? (default 0xff)
	//}
		uint8_t				Unknown[16];//	??? (ignored)
		uint32_t			nTextureCount;//(int) number of textures
		char**				pTextureNames;// (char[40]) filepath of a texture
		char				szMainNode[40];// name of the main node
		uint32_t			nNodeCount; // (int) number of nodes
		RSM_NODE_DATA_1_5*	pNodes;
		uint32_t			nVolumeBoxCount; // number of volume boxes
		RSM_VOLUME_BOX_1_5*	pVolumeBoxes; 
	} RSM_FILE_DATA_1_5, RSM_FILE_DATA;
#pragma pack( pop )
	int32_t LoadRSMDataFromFile( const wchar_t* pFilename, RSM_FILE_DATA_1_5* pRSMData );
	void	CleanRSMData( RSM_FILE_DATA_1_5& pRSMData );

	int32_t BuildMeshGeometryFromRSM( const RSM_FILE_DATA* in_pRSMData, uint32_t* inout_nVertexCount, 
										Vector3* out_pVertices, Vector2* out_pTexCoord, 
										Vector3* out_pNormals, uint32_t* inout_nIndexCount, 
										bool* b32BitIndices, void* out_pIndices, 
										uint32_t* out_pAttributes, uint32_t* inout_nMeshAttributeCount,
										MESH_ATTRIBUTE_DATA* out_pAttributeData, 
										uint32_t* inout_nNodeCount,
										MESH_NODE_DATA* out_pNodeData,
										uint32_t dwFlags );

	int32_t BuildMeshGeometryFromRSMNode( const RSM_NODE_DATA_1_5* in_pNodeData, 
										uint32_t* inout_nVertexCount, Vector3* out_pVertices, 
										Vector2* out_pTexCoord, uint32_t* inout_nIndexCount, 
										bool* b32BitIndices, uint32_t* out_pIndices, 
										uint32_t* out_pAttributes, uint32_t* out_nMeshAttributeCount,
										MESH_ATTRIBUTE_DATA* out_pAttributeData, 
										uint32_t* inout_nNodeCount, MESH_NODE_DATA* out_pNodeData,
										uint32_t dwFlags );


}// namespace 
#endif // _ROPP_MESH_H