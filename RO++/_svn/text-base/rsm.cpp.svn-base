#include "ropp_model.h"
#include <string.h>
#include <stdio.h>

using namespace ropp;

int32_t ropp::BuildMeshGeometryFromRSM( const RSM_FILE_DATA* in_RSMData, uint32_t* inout_nVertexCount, 
										Vector3* out_pVertices, Vector2* out_pTexCoord, 
										Vector3* out_pNormals, uint32_t* inout_nIndexCount, 
										bool* b32BitIndices, void* out_pIndices, 
										uint32_t* out_pAttributes, uint32_t* inout_nMeshAttributeCount,
										MESH_ATTRIBUTE_DATA* out_pAttributeData, 
										uint32_t* inout_nNodeCount,
										MESH_NODE_DATA* out_pNodeData,
										uint32_t dwFlags )
{
	if( 0 == in_RSMData || 0 == inout_nVertexCount || 0 == inout_nIndexCount )
		return -1;

	bool bCountOnly = false;
	if( 0 == out_pVertices )
		bCountOnly = true;

	uint32_t iNode, iTex, iTri, nTextureCount, 
		nNodeCount=in_RSMData->nNodeCount;
	if( 0 == nNodeCount )
		return 0;

	bool bLocalNodeData = false;
	if( 0 == out_pNodeData ) 
	{
		bLocalNodeData = true;
		out_pNodeData = (MESH_NODE_DATA*)malloc( sizeof( MESH_NODE_DATA )*nNodeCount );
		memset( out_pNodeData, 0, sizeof( MESH_NODE_DATA )*nNodeCount );
	}
	else
		memset( out_pNodeData, 0, sizeof( MESH_NODE_DATA )*(*inout_nNodeCount) );

	uint32_t nTriangleCount=0, nIndexCount=0, nVertexCount=0, nAttributeCount=0, nFinalNodeCount=0;
	uint32_t nAttrID, nVertexStart=0, nAttrVertexCount=0, nFaceStart=0, nFaceCount=0; // <- all these are for counting attributes
	const RSM_NODE_DATA_1_5* pNode;
	const RSM_TRIANGLE_DATA_1_5* pTriangle;

	MESH_NODE_DATA* out_CurrentNode;
	Vector3 * out_Vert0, * out_Vert1, * out_Vert2;

	Quaternion qRot( 0.0f, 0.0f, 0.0f, 1.0f );
	uint32_t rootIndex = 0;
	for( iNode=0; iNode<nNodeCount; iNode++ )
	{
		if( 0 == strcmp( in_RSMData->szMainNode, in_RSMData->pNodes[iNode].szName ) )
		{
			rootIndex = iNode;
			break;
		}
	}

	for( iNode=0; iNode<nNodeCount; iNode++ )
	{
		pNode = &in_RSMData->pNodes[iNode];
		nTextureCount = pNode->nTextureCount;

		if( out_pNodeData )
		{
			out_CurrentNode = &out_pNodeData[nFinalNodeCount];
			if( nNodeCount > 1 )
			{// look for parent
				for( uint32_t iNode2=0; iNode2<nNodeCount; iNode2++ )
				{
					//if( in_RSMData->pNodes[iNode2].szName[0] && pNode->szParentName[0] )
					//if( pNode->szParentName[0] )
					//{
						if( 0 == strcmp( in_RSMData->pNodes[iNode2].szName, pNode->szParentName ) )
						{
							out_CurrentNode->nParentIndex = iNode2;
							break;
						}
					//}
					if( iNode == rootIndex )
						out_CurrentNode->nParentIndex = -1;
					else
						out_CurrentNode->nParentIndex = rootIndex;
				}
			}
			else
				out_CurrentNode->nParentIndex = -1;
		}
		
		for( iTex=0; iTex<nTextureCount; iTex++ )
		{
			if( out_pAttributeData )
			{
				if( nAttributeCount < *inout_nMeshAttributeCount )
				{
					out_pAttributeData[nAttributeCount].nAttributeID	= nAttributeCount;
					out_pAttributeData[nAttributeCount].nTextureIndex	= pNode->lstTextureIndices[iTex];
					out_pAttributeData[nAttributeCount].nVertexStart	= nVertexCount;
					out_pAttributeData[nAttributeCount].nTriangleStart	= nTriangleCount;
				}
			}
			else
			{
				nAttrID			= nAttributeCount;
				nFaceStart		= nTriangleCount;
				nVertexStart	= nVertexCount;
			}
			if( out_pNodeData && 0 == pNode->nTriangleCount )
			{
				out_CurrentNode->BoundingVolume.vCenter	= Vector3( 0.0f, 0.0f, 0.0f );
				out_CurrentNode->BoundingVolume.vMax	= Vector3( 0.01f, 0.01f, 0.01f );
				out_CurrentNode->BoundingVolume.vMin	= Vector3( -0.01f, -0.01f, -0.01f );
			}

			for( iTri=0; iTri<pNode->nTriangleCount; iTri++ )
			{
				pTriangle = &pNode->lstTriangles[iTri];
				if( pTriangle->nTextureIndex == iTex )
				{
					if( false == bCountOnly )
					{
						out_Vert0 = &out_pVertices[nVertexCount+0];
						out_Vert1 = &out_pVertices[nVertexCount+1];
						out_Vert2 = &out_pVertices[nVertexCount+2];
						(*out_Vert0) = pNode->lstVertices[pTriangle->nVertexIndices[0]];
						(*out_Vert1) = pNode->lstVertices[pTriangle->nVertexIndices[1]];
						(*out_Vert2) = pNode->lstVertices[pTriangle->nVertexIndices[2]];
						Matrix3 m = pNode->mTransform;
						(*out_Vert0) *= m;
						(*out_Vert1) *= m;
						(*out_Vert2) *= m;
						(*out_Vert0) += pNode->vOffset;
						(*out_Vert1) += pNode->vOffset;
						(*out_Vert2) += pNode->vOffset;

						if( out_pNodeData )
						{
							out_CurrentNode->BoundingVolume.vMax.x = max( out_Vert0->x, out_CurrentNode->BoundingVolume.vMax.x ); out_CurrentNode->BoundingVolume.vMin.x = min( out_Vert0->x, out_CurrentNode->BoundingVolume.vMin.x );
							out_CurrentNode->BoundingVolume.vMax.y = max( out_Vert0->y, out_CurrentNode->BoundingVolume.vMax.y ); out_CurrentNode->BoundingVolume.vMin.y = min( out_Vert0->y, out_CurrentNode->BoundingVolume.vMin.y );
							out_CurrentNode->BoundingVolume.vMax.z = max( out_Vert0->z, out_CurrentNode->BoundingVolume.vMax.z ); out_CurrentNode->BoundingVolume.vMin.z = min( out_Vert0->z, out_CurrentNode->BoundingVolume.vMin.z );
							out_CurrentNode->BoundingVolume.vMax.x = max( out_Vert1->x, out_CurrentNode->BoundingVolume.vMax.x ); out_CurrentNode->BoundingVolume.vMin.x = min( out_Vert1->x, out_CurrentNode->BoundingVolume.vMin.x );
							out_CurrentNode->BoundingVolume.vMax.y = max( out_Vert1->y, out_CurrentNode->BoundingVolume.vMax.y ); out_CurrentNode->BoundingVolume.vMin.y = min( out_Vert1->y, out_CurrentNode->BoundingVolume.vMin.y );
							out_CurrentNode->BoundingVolume.vMax.z = max( out_Vert1->z, out_CurrentNode->BoundingVolume.vMax.z ); out_CurrentNode->BoundingVolume.vMin.z = min( out_Vert1->z, out_CurrentNode->BoundingVolume.vMin.z );
							out_CurrentNode->BoundingVolume.vMax.x = max( out_Vert2->x, out_CurrentNode->BoundingVolume.vMax.x ); out_CurrentNode->BoundingVolume.vMin.x = min( out_Vert2->x, out_CurrentNode->BoundingVolume.vMin.x );
							out_CurrentNode->BoundingVolume.vMax.y = max( out_Vert2->y, out_CurrentNode->BoundingVolume.vMax.y ); out_CurrentNode->BoundingVolume.vMin.y = min( out_Vert2->y, out_CurrentNode->BoundingVolume.vMin.y );
							out_CurrentNode->BoundingVolume.vMax.z = max( out_Vert2->z, out_CurrentNode->BoundingVolume.vMax.z ); out_CurrentNode->BoundingVolume.vMin.z = min( out_Vert2->z, out_CurrentNode->BoundingVolume.vMin.z );
						}
						if( *b32BitIndices )
						{ // we invert order because we negated y
							((uint32_t*)out_pIndices)[nIndexCount+0]	= nVertexCount+0;
							((uint32_t*)out_pIndices)[nIndexCount+1]	= nVertexCount+2;
							((uint32_t*)out_pIndices)[nIndexCount+2]	= nVertexCount+1;
							if( 0 != pTriangle->nTwoSide )
							{ // indices are useful for 2 sided faces
								((uint32_t*)out_pIndices)[nIndexCount+3]	= nVertexCount+0;
								((uint32_t*)out_pIndices)[nIndexCount+4]	= nVertexCount+1;
								((uint32_t*)out_pIndices)[nIndexCount+5]	= nVertexCount+2;						
							}
						}
						else // 16 bit indices
						{
							((uint16_t*)out_pIndices)[nIndexCount+0]	= nVertexCount+0;
							((uint16_t*)out_pIndices)[nIndexCount+1]	= nVertexCount+2;
							((uint16_t*)out_pIndices)[nIndexCount+2]	= nVertexCount+1;
							if( 0 != pTriangle->nTwoSide )
							{
								((uint16_t*)out_pIndices)[nIndexCount+3]	= nVertexCount+0;
								((uint16_t*)out_pIndices)[nIndexCount+4]	= nVertexCount+1;
								((uint16_t*)out_pIndices)[nIndexCount+5]	= nVertexCount+2;						
							}
						}
						if( out_pTexCoord )
						{ // TEXCOORD ?? 
							out_pTexCoord[nVertexCount+0] = Vector2( pNode->lstAttributes[pTriangle->nAttributeIndices[0]].u,
								pNode->lstAttributes[pTriangle->nAttributeIndices[0]].v );//1.0f-
							out_pTexCoord[nVertexCount+1] =	Vector2( pNode->lstAttributes[pTriangle->nAttributeIndices[1]].u,
								pNode->lstAttributes[pTriangle->nAttributeIndices[1]].v );//
							out_pTexCoord[nVertexCount+2] =	Vector2( pNode->lstAttributes[pTriangle->nAttributeIndices[2]].u,
								pNode->lstAttributes[pTriangle->nAttributeIndices[2]].v );//1.0f-
						}
						if( out_pNormals )
						{ // this may be wrong but it works fine ( I think )
							out_pNormals[nVertexCount+0]	= ((*out_Vert1)-(*out_Vert0)).Cross( ((*out_Vert2)-(*out_Vert0)) );
							out_pNormals[nVertexCount+1]	= ((*out_Vert2)-(*out_Vert1)).Cross( ((*out_Vert0)-(*out_Vert1)) );
							out_pNormals[nVertexCount+2]	= ((*out_Vert0)-(*out_Vert2)).Cross( ((*out_Vert1)-(*out_Vert2)) );
						}
						if( out_pAttributes ) // i don't use this one, it was useful when I relied on D3DX functions for building a mesh
							out_pAttributes[nTriangleCount]	= nAttributeCount;
					};
					nIndexCount +=3; 
					nVertexCount +=3; 
					nTriangleCount++;
					if( 0 != pTriangle->nTwoSide )
					{
						nIndexCount +=3;
						nTriangleCount++;
					}
				}
			}// for( triangles )
			if( out_pAttributeData )
			{
				if( nAttributeCount < *inout_nMeshAttributeCount )
				{
					out_pAttributeData[nAttributeCount].nVertexCount = nVertexCount - out_pAttributeData[nAttributeCount].nVertexStart;
					out_pAttributeData[nAttributeCount].nTriangleCount = nTriangleCount - out_pAttributeData[nAttributeCount].nTriangleStart;
					if( out_pAttributeData[nAttributeCount].nVertexCount > 0 )
					{
						if( out_pNodeData && (nFinalNodeCount < *inout_nNodeCount) )
						{
							qRot = Quaternion( pNode->vRotationAxis.x*sin(pNode->fRotation*.5f),
												pNode->vRotationAxis.y*sin(pNode->fRotation*.5f), 
												pNode->vRotationAxis.z*sin(pNode->fRotation*.5f), 
												cos(pNode->fRotation*.5f) ).Normalize();
							
							out_CurrentNode->lstAttributeIndices
								[out_CurrentNode->nAttributeCount] = nAttributeCount;
							out_CurrentNode->vScale			= pNode->vScale;
							out_CurrentNode->qOrientation	= qRot;
							out_CurrentNode->vPosition		= pNode->vPosition;
							out_CurrentNode->nAttributeCount++;
							if( out_CurrentNode->nAttributeCount == 64 )
								throw( "" );
						}
						nAttributeCount++;
					}
					else
					{
						printf("Warning! Node attribute vertex count is zero!\n");
					}

					// we skip empty attributes
				}
			}
			else
			{
				nFaceCount = nTriangleCount - nFaceStart;
				nAttrVertexCount = nVertexCount - nVertexStart;
				if( nAttrVertexCount > 0 )
				{
					if( out_pNodeData )
					{
						out_pNodeData[nFinalNodeCount].lstAttributeIndices
							[out_pNodeData[nFinalNodeCount].nAttributeCount] = nAttributeCount;
						out_pNodeData[nFinalNodeCount].nAttributeCount++;
					}
					nAttributeCount++;
				}
				else
				{
					printf("Warning! Node attribute vertex count is zero!\n");
				}
			}
		}
		//if( 0 < out_pNodeData[nFinalNodeCount].nAttributeCount ) don't discard any node: it may be just an invisible bone???
		if( false == bCountOnly && out_pNodeData )
		{
			Vector3 dif = (out_CurrentNode->BoundingVolume.vMax-out_CurrentNode->BoundingVolume.vMin);
			out_CurrentNode->BoundingVolume.vCenter = (out_CurrentNode->BoundingVolume.vMax+out_CurrentNode->BoundingVolume.vMin)/2;
				//out_CurrentNode->BoundingVolume.vMin 
				//+ dif;
		}
		nFinalNodeCount++;
	};

	if( nVertexCount > 0xFFFF )
		*b32BitIndices = true;
	else 
		*b32BitIndices = false;

	*inout_nIndexCount	= nIndexCount;
	*inout_nVertexCount	= nVertexCount;
	if( inout_nMeshAttributeCount )
		*inout_nMeshAttributeCount = nAttributeCount;
	if( inout_nNodeCount )
		*inout_nNodeCount	= nFinalNodeCount;

	if( bLocalNodeData )
		free( out_pNodeData );
	return 0;
};

