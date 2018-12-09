#include "ropp_map.h"
#include <memory.h>
#include <stdlib.h>

using namespace ropp;

// Forward declarations
void CalcNormals( const Vector3* pVertices, const GND_TILE_INDICES* pTileIndices,
				 const GND_TILE_DATA* pTileData, uint32_t nWidth, uint32_t nDepth, 
				 float fHeightScale, Vector3 *out_pNormals );
void AssignIndices( uint32_t nIndexOffset, uint32_t nVertexOffset, void* pIndexBuffer, bool b32Bit );
void AssignTexCoord( uint32_t nVertexOffset, const GND_ATTRIBUTE_DATA* pAttr, Vector2* pTexCBuffer );


void AssignIndices( uint32_t nIndexOffset, uint32_t nVertexOffset, void* pIndexBuffer, bool b32Bit )
{
	//if( b32Bit )
	//{
	//	// set indices
	//	((uint32_t*)pIndexBuffer)[nIndexOffset]		= nVertexOffset;
	//	((uint32_t*)pIndexBuffer)[nIndexOffset+1]	= nVertexOffset+2;
	//	((uint32_t*)pIndexBuffer)[nIndexOffset+2]	= nVertexOffset+1;
	//	((uint32_t*)pIndexBuffer)[nIndexOffset+3]	= nVertexOffset+1;
	//	((uint32_t*)pIndexBuffer)[nIndexOffset+4]	= nVertexOffset+2;
	//	((uint32_t*)pIndexBuffer)[nIndexOffset+5]	= nVertexOffset+3;
	//}
	//else
	//{
	//	// set indices
	//	((uint16_t*)pIndexBuffer)[nIndexOffset]		= nVertexOffset;
	//	((uint16_t*)pIndexBuffer)[nIndexOffset+1]	= nVertexOffset+2;
	//	((uint16_t*)pIndexBuffer)[nIndexOffset+2]	= nVertexOffset+1;
	//	((uint16_t*)pIndexBuffer)[nIndexOffset+3]	= nVertexOffset+1;
	//	((uint16_t*)pIndexBuffer)[nIndexOffset+4]	= nVertexOffset+2;
	//	((uint16_t*)pIndexBuffer)[nIndexOffset+5]	= nVertexOffset+3;
	//}
	if( b32Bit )
	{
		// set indices
		((uint32_t*)pIndexBuffer)[nIndexOffset]		= nVertexOffset;
		((uint32_t*)pIndexBuffer)[nIndexOffset+1]	= nVertexOffset+1;
		((uint32_t*)pIndexBuffer)[nIndexOffset+2]	= nVertexOffset+2;
		((uint32_t*)pIndexBuffer)[nIndexOffset+3]	= nVertexOffset+1;
		((uint32_t*)pIndexBuffer)[nIndexOffset+4]	= nVertexOffset+3;
		((uint32_t*)pIndexBuffer)[nIndexOffset+5]	= nVertexOffset+2;
	}
	else
	{
		// set indices
		((uint16_t*)pIndexBuffer)[nIndexOffset]		= nVertexOffset;
		((uint16_t*)pIndexBuffer)[nIndexOffset+1]	= nVertexOffset+1;
		((uint16_t*)pIndexBuffer)[nIndexOffset+2]	= nVertexOffset+2;
		((uint16_t*)pIndexBuffer)[nIndexOffset+3]	= nVertexOffset+1;
		((uint16_t*)pIndexBuffer)[nIndexOffset+4]	= nVertexOffset+3;
		((uint16_t*)pIndexBuffer)[nIndexOffset+5]	= nVertexOffset+2;
	}	
};

void AssignTexCoord( uint32_t nVertexOffset, const GND_ATTRIBUTE_DATA* pAttr, Vector2* pTexCBuffer )
{
	if( 0 == pTexCBuffer )
		return;
	pTexCBuffer[nVertexOffset+0]	= Vector2( pAttr->u[0], pAttr->v[0] );
	pTexCBuffer[nVertexOffset+1]	= Vector2( pAttr->u[1], pAttr->v[1] );
	pTexCBuffer[nVertexOffset+2]	= Vector2( pAttr->u[2], pAttr->v[2] );
	pTexCBuffer[nVertexOffset+3]	= Vector2( pAttr->u[3], pAttr->v[3] );
}

//-------------------------------------------

int32_t ropp::BuildTerrainGeometryFromGND( const GND_TILE_DATA* lstTileData, uint32_t nTileCount,
			const GND_ATTRIBUTE_DATA* pAttributeData, int32_t nGNDAttributeCount, uint32_t nTextureCount,
			float fHeightScale,
			uint32_t nColumnSubgrids, uint32_t nRowSubgrids,
			uint32_t nSubgridColumnTiles, uint32_t nSubgridRowTiles, 
			Vector3* out_pVertices, uint32_t* out_nVertexCount, void* out_pIndices, bool* b32Bit,
			uint32_t* out_nIndexCount, Vector2* out_pTexCoord, Vector3* out_pNormals, 
			uint32_t* out_pAttributes, uint32_t* out_nMeshAttributeCount, 
			MESH_ATTRIBUTE_DATA* out_pAttributeData, GND_TILE_INDICES* out_lstTileIndices,
			MESH_NODE_DATA* out_pNodeData, uint32_t* nTopTileCount, 
			uint32_t* nFrontTileCount, uint32_t* nRightTileCount )// node count is nRowSubgrids*nColumnSubgrids
{
	const GND_TILE_DATA * pTile = 0, * pTileF = 0, * pTileR = 0;
	const GND_ATTRIBUTE_DATA* pAttr=0;

	GND_TILE_INDICES* pTileIndices=0;

	if( 0 != out_pNormals )
	{
		if( 0 != out_lstTileIndices )
			pTileIndices = out_lstTileIndices;
		else
			pTileIndices = (GND_TILE_INDICES*)malloc( sizeof( GND_TILE_INDICES )*nTileCount );
		if( pTileIndices )
			memset( pTileIndices, 0, sizeof(GND_TILE_INDICES)*nTileCount );
	}

	uint32_t nTotalWidth = nColumnSubgrids*nSubgridColumnTiles;
	uint32_t nTotalDepth = nRowSubgrids*nSubgridRowTiles;

	bool bCountOnly = false;
	if( 0 == out_pVertices )
		bCountOnly = true;
	if( false == bCountOnly )
	{
		if( 0 == out_pIndices )
			return -1;
	}
	if( 0 == nRowSubgrids || 0 == nColumnSubgrids || 0 == nSubgridColumnTiles || 0 == nSubgridRowTiles )
		return -1;
	if( 0 == nTextureCount || 0 == pAttributeData || 0 == lstTileData || 0 == nTileCount )
		return -1;

	uint32_t nTileOffsetX, nTileOffsetZ;
	// i: local row, j: local column, k: TextureIndex, rs: row subgrid, cs: column subgrid
	uint32_t i=0, j=0, k=0, cs=0, rs=0, nGlobalTileIndex=0;
	uint32_t nVertexCount=0, nIndexCount=0, nTriangleCount=0, nAttributeCount=0;
	uint32_t nAttrID, nVertexStart, nAttrVertexCount, nFaceStart, nFaceCount;
	if( out_pNormals )
		memset( out_pNormals, 0, sizeof( Vector3 )*(*out_nVertexCount) );
	uint32_t nGlobalTileX, nGlobalTileZ;

	Vector3 vFinalOffset = Vector3( nTotalWidth*fHeightScale*-.5f, 0.0f, nTotalDepth*fHeightScale*-.5f ); 
	uint32_t nSubgridIndex = 0;

	Vector3 * out_Vert0, * out_Vert1, * out_Vert2, * out_Vert3;
	Vector3 * pvMax, * pvMin;
	for( rs=0; rs<nRowSubgrids; rs++ )
	{
		for( cs=0; cs<nColumnSubgrids; cs++ )
		{
			nSubgridIndex = (rs*nColumnSubgrids)+cs;
			nTileOffsetX = (nSubgridColumnTiles)*cs;
			nTileOffsetZ = (nSubgridRowTiles)*rs;
			if( out_pNodeData )
			{
				pvMax = &out_pNodeData[nSubgridIndex].BoundingVolume.vMax;
				pvMin = &out_pNodeData[nSubgridIndex].BoundingVolume.vMin;
				pvMin->x = (nTileOffsetX)*fHeightScale;
				pvMin->z = (nTileOffsetZ)*fHeightScale;
				pvMax->x = (nTileOffsetX+nSubgridColumnTiles)*fHeightScale;
				pvMax->z = (nTileOffsetZ+nSubgridRowTiles)*fHeightScale;
			}
			for( k=0; k<nTextureCount; k++ )
			{
				if( out_pAttributeData )
				{
					if( nAttributeCount < *out_nMeshAttributeCount )
					{
						out_pAttributeData[nAttributeCount].nAttributeID	= nAttributeCount;
						out_pAttributeData[nAttributeCount].nTextureIndex	= k;
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

				for( i=0; i<nSubgridRowTiles; i++ )
				{
					for( j=0; j<nSubgridColumnTiles; j++ )
					{
						nGlobalTileIndex	= (nTileOffsetZ+i)*nTotalWidth+j+nTileOffsetX;;
						nGlobalTileX		= (j + nTileOffsetX);
						nGlobalTileZ		= (i + nTileOffsetZ);
						//nGlobalTileX	= nGlobalTileIndex % nTotalWidth;
						//nGlobalTileZ	= nGlobalTileIndex / nTotalWidth;

						pTile = &lstTileData[nGlobalTileIndex];
						pTileF = GetFrontTile( lstTileData, nGlobalTileIndex, nTotalWidth, nTotalDepth );
						pTileR = GetRightTile( lstTileData, nGlobalTileIndex, nTotalWidth, nTotalDepth );

						if( ( pTile->TopAttributeIndex >= 0 ) 
							&&  ( pTile->TopAttributeIndex < nGNDAttributeCount ) 
							&& k == pAttributeData[pTile->TopAttributeIndex].nTextureIndex )
						{
							if( false == bCountOnly )
							{
								pAttr = &pAttributeData[pTile->TopAttributeIndex];
								out_Vert0 = &out_pVertices[nVertexCount+0];
								out_Vert1 = &out_pVertices[nVertexCount+1];
								out_Vert2 = &out_pVertices[nVertexCount+2];
								out_Vert3 = &out_pVertices[nVertexCount+3];
								(*out_Vert0) = Vector3( (nGlobalTileX)*fHeightScale,	pTile->fHeight[0], (nGlobalTileZ)*fHeightScale );
								(*out_Vert1) = Vector3( (nGlobalTileX+1)*fHeightScale,	pTile->fHeight[1], (nGlobalTileZ)*fHeightScale );
								(*out_Vert2) = Vector3( (nGlobalTileX)*fHeightScale,	pTile->fHeight[2], (nGlobalTileZ+1)*fHeightScale );
								(*out_Vert3) = Vector3( (nGlobalTileX+1)*fHeightScale,	pTile->fHeight[3], (nGlobalTileZ+1)*fHeightScale );
								(*out_Vert0) += vFinalOffset;
								(*out_Vert1) += vFinalOffset;
								(*out_Vert2) += vFinalOffset;
								(*out_Vert3) += vFinalOffset;
								if( out_pNodeData )
								{
									pvMax->y = max( pvMax->y, out_Vert0->y ); pvMin->y = min( pvMin->y, out_Vert0->y );
									pvMax->y = max( pvMax->y, out_Vert1->y ); pvMin->y = min( pvMin->y, out_Vert1->y );
									pvMax->y = max( pvMax->y, out_Vert2->y ); pvMin->y = min( pvMin->y, out_Vert2->y );
									pvMax->y = max( pvMax->y, out_Vert3->y ); pvMin->y = min( pvMin->y, out_Vert3->y );
								}
								AssignIndices( nIndexCount, nVertexCount, out_pIndices, *b32Bit );
								AssignTexCoord( nVertexCount, pAttr, out_pTexCoord );
								
								if( out_pNormals )
								{
									out_pNormals[nVertexCount+0] = 
									out_pNormals[nVertexCount+1] = 
									out_pNormals[nVertexCount+2] = 
									out_pNormals[nVertexCount+3] = Vector3( 0.0f, 1.0f, 0.0f );
								}
								if( out_pNormals || out_lstTileIndices )
								{
									pTileIndices[nGlobalTileIndex].VerticesTop[0] = nVertexCount+0;
									pTileIndices[nGlobalTileIndex].VerticesTop[1] = nVertexCount+1;
									pTileIndices[nGlobalTileIndex].VerticesTop[2] = nVertexCount+2;
									pTileIndices[nGlobalTileIndex].VerticesTop[3] = nVertexCount+3;
								}
								// set attribute
								if( out_pAttributes )
								{
									out_pAttributes[nTriangleCount]		= nAttributeCount;
									out_pAttributes[nTriangleCount+1]	= nAttributeCount;
								}
							}
							nVertexCount	+= 4;
							nIndexCount		+= 6;
							nTriangleCount	+= 2;
							if( 0 != nTopTileCount )
								(*nTopTileCount)++;
						}
						else if( -1 == pTile->TopAttributeIndex
							&& ( out_pNormals || out_lstTileIndices ) )
						{
							pTileIndices[nGlobalTileIndex].VerticesTop[0] = 
							pTileIndices[nGlobalTileIndex].VerticesTop[1] = 
							pTileIndices[nGlobalTileIndex].VerticesTop[2] = 
							pTileIndices[nGlobalTileIndex].VerticesTop[3] = -1;
						}

						if( ( pTile->FrontAttributeIndex >= 0 ) 
							&&  ( pTile->FrontAttributeIndex < nGNDAttributeCount ) 
							&& k == pAttributeData[pTile->FrontAttributeIndex].nTextureIndex )
						{
							if( false == bCountOnly )
							{
								pAttr = &pAttributeData[pTile->FrontAttributeIndex];
								out_Vert0 = &out_pVertices[nVertexCount+0];
								out_Vert1 = &out_pVertices[nVertexCount+1];
								out_Vert2 = &out_pVertices[nVertexCount+2];
								out_Vert3 = &out_pVertices[nVertexCount+3];								
								(*out_Vert0) = Vector3( (nGlobalTileX+1)*fHeightScale, pTile->fHeight[3], (nGlobalTileZ+1)*fHeightScale );
								(*out_Vert1) = Vector3( (nGlobalTileX+1)*fHeightScale, pTile->fHeight[1], (nGlobalTileZ)*fHeightScale );
								if( pTileF )
								{
									(*out_Vert2) = Vector3( (nGlobalTileX+1)*fHeightScale, pTileF->fHeight[2], (nGlobalTileZ+1)*fHeightScale );
									(*out_Vert3) = Vector3( (nGlobalTileX+1)*fHeightScale, pTileF->fHeight[0], (nGlobalTileZ)*fHeightScale );
								}
								else
								{
									(*out_Vert2) = Vector3( (nGlobalTileX+1)*fHeightScale, 0, (nGlobalTileZ+1)*fHeightScale );
									(*out_Vert3) = Vector3( (nGlobalTileX+1)*fHeightScale, 0, (nGlobalTileZ)*fHeightScale );
								}
								(*out_Vert0) += vFinalOffset;
								(*out_Vert1) += vFinalOffset;
								(*out_Vert2) += vFinalOffset;
								(*out_Vert3) += vFinalOffset;

								AssignIndices( nIndexCount, nVertexCount, out_pIndices, *b32Bit );
								AssignTexCoord( nVertexCount, pAttr, out_pTexCoord );
								if( out_pNormals )
								{
									if( (pTile->fHeight[1]*fHeightScale) 
										> ( (0 != pTileF) ? (pTileF->fHeight[0]*fHeightScale) : 0 ) 
										)
									{
										out_pNormals[nVertexCount+0] = 
										out_pNormals[nVertexCount+1] = 
										out_pNormals[nVertexCount+2] = 
										out_pNormals[nVertexCount+3] = Vector3( -1.0f, 0.0f, 0.0f );
									}
									else
									{
										out_pNormals[nVertexCount+0] = 
										out_pNormals[nVertexCount+1] = 
										out_pNormals[nVertexCount+2] = 
										out_pNormals[nVertexCount+3] = Vector3( 1.0f, 0.0f, 0.0f );
									}
								}

								if( out_lstTileIndices )
								{
									pTileIndices[nGlobalTileIndex].VerticesFront[0] = nVertexCount+0;
									pTileIndices[nGlobalTileIndex].VerticesFront[1] = nVertexCount+1;
									pTileIndices[nGlobalTileIndex].VerticesFront[2] = nVertexCount+2;
									pTileIndices[nGlobalTileIndex].VerticesFront[3] = nVertexCount+3;
								}

								// set attribute
								if( out_pAttributes )
								{
									out_pAttributes[nTriangleCount]		= nAttributeCount;
									out_pAttributes[nTriangleCount+1]	= nAttributeCount;
								}
							}
							nVertexCount	+= 4;
							nIndexCount		+= 6;
							nTriangleCount	+= 2;
							if( 0 != nFrontTileCount )
								(*nFrontTileCount)++;
						}
						else if( -1 == pTile->FrontAttributeIndex )
						{
							if( out_lstTileIndices )
							{
								pTileIndices[nGlobalTileIndex].VerticesFront[0] = 
								pTileIndices[nGlobalTileIndex].VerticesFront[1] = 
								pTileIndices[nGlobalTileIndex].VerticesFront[2] = 
								pTileIndices[nGlobalTileIndex].VerticesFront[3] = -1;
							}
						}
						if( ( pTile->RightAttributeIndex >= 0 ) 
							&&  ( pTile->RightAttributeIndex < nGNDAttributeCount ) 
							&& k == pAttributeData[pTile->RightAttributeIndex].nTextureIndex )
						{
							if( false == bCountOnly )
							{
								pAttr = &pAttributeData[pTile->RightAttributeIndex];
								out_Vert0 = &out_pVertices[nVertexCount+0];
								out_Vert1 = &out_pVertices[nVertexCount+1];
								out_Vert2 = &out_pVertices[nVertexCount+2];
								out_Vert3 = &out_pVertices[nVertexCount+3];
								(*out_Vert0) = Vector3( (nGlobalTileX)*fHeightScale,  pTile->fHeight[2], (nGlobalTileZ+1)*fHeightScale );
								(*out_Vert1) = Vector3( (nGlobalTileX+1)*fHeightScale,pTile->fHeight[3], (nGlobalTileZ+1)*fHeightScale );
								if( pTileR )
								{
									(*out_Vert2) = Vector3( (nGlobalTileX)*fHeightScale,  pTileR->fHeight[0], (nGlobalTileZ+1)*fHeightScale );
									(*out_Vert3) = Vector3( (nGlobalTileX+1)*fHeightScale,pTileR->fHeight[1], (nGlobalTileZ+1)*fHeightScale );
								}
								else
								{
									(*out_Vert2) = Vector3( (nGlobalTileX)*fHeightScale,	0, (nGlobalTileZ+1)*fHeightScale );
									(*out_Vert3) = Vector3( (nGlobalTileX+1)*fHeightScale,	0, (nGlobalTileZ+1)*fHeightScale );
								}
								(*out_Vert0) += vFinalOffset;
								(*out_Vert1) += vFinalOffset;
								(*out_Vert2) += vFinalOffset;
								(*out_Vert3) += vFinalOffset;

								AssignIndices( nIndexCount, nVertexCount, out_pIndices, *b32Bit );
								AssignTexCoord( nVertexCount, pAttr, out_pTexCoord );

								if( out_pNormals )
								{
									if( (pTile->fHeight[2]) 
										> ( (0 != pTileR) ? (pTileR->fHeight[0]) : 0)
										)
									{
										out_pNormals[nVertexCount+0] = 
										out_pNormals[nVertexCount+1] = 
										out_pNormals[nVertexCount+2] = 
										out_pNormals[nVertexCount+3] = Vector3( 0.0f, 0.0f, -1.0f );
									}
									else
									{
										out_pNormals[nVertexCount+0] = 
										out_pNormals[nVertexCount+1] = 
										out_pNormals[nVertexCount+2] = 
										out_pNormals[nVertexCount+3] = Vector3( 0.0f, 0.0f, 1.0f );
									}
								}
								if( out_lstTileIndices )
								{
									pTileIndices[nGlobalTileIndex].VerticesRight[0] = nVertexCount+0;
									pTileIndices[nGlobalTileIndex].VerticesRight[1] = nVertexCount+1;
									pTileIndices[nGlobalTileIndex].VerticesRight[2] = nVertexCount+2;
									pTileIndices[nGlobalTileIndex].VerticesRight[3] = nVertexCount+3;
								}

								// set attribute
								if( out_pAttributes )
								{
									out_pAttributes[nTriangleCount]		= nAttributeCount;
									out_pAttributes[nTriangleCount+1]	= nAttributeCount;
								}
							}
							nVertexCount	+= 4;
							nIndexCount		+= 6;
							nTriangleCount	+= 2;
							if( 0 != nRightTileCount )
								(*nRightTileCount)++;
						} // 
						else if( -1 == pTile->RightAttributeIndex )
						{
							if( out_lstTileIndices )
							{
								pTileIndices[nGlobalTileIndex].VerticesRight[0] = 
								pTileIndices[nGlobalTileIndex].VerticesRight[1] = 
								pTileIndices[nGlobalTileIndex].VerticesRight[2] = 
								pTileIndices[nGlobalTileIndex].VerticesRight[3] = -1;
							}
						}
					} // j=0;
				} // i=0;
				if( out_pAttributeData )
				{
					if( nAttributeCount < *out_nMeshAttributeCount )
					{
						out_pAttributeData[nAttributeCount].nVertexCount = nVertexCount - out_pAttributeData[nAttributeCount].nVertexStart;
						out_pAttributeData[nAttributeCount].nTriangleCount = nTriangleCount - out_pAttributeData[nAttributeCount].nTriangleStart;
						if( out_pAttributeData[nAttributeCount].nVertexCount > 0 )
						{
							if( out_pNodeData )
							{
								out_pNodeData[nSubgridIndex].lstAttributeIndices
									[out_pNodeData[nSubgridIndex].nAttributeCount] = nAttributeCount;
								out_pNodeData[nSubgridIndex].nAttributeCount++;
								if( out_pNodeData[nSubgridIndex].nAttributeCount == 64 )
									throw( "" );
							}
							nAttributeCount++;
						}
						// we skip empty attributes
					}
				}
				else
				{
					nFaceCount = nTriangleCount - nFaceStart;
					nAttrVertexCount = nVertexCount - nVertexStart;
					if( nAttrVertexCount > 0 )
						nAttributeCount++;
				}
			} // k=0
			if( false == bCountOnly && out_pNodeData )
			{
				out_pNodeData[nSubgridIndex].BoundingVolume.vCenter = (*pvMin)+( ((*pvMax)-(*pvMin))*.5f );
			}
		} // cs=0;
	}// rs=0;
	if( nVertexCount > 0xFFFF )
		*b32Bit = true;
	else 
		*b32Bit = false;

	if( (false == bCountOnly) && out_pNormals )
		CalcNormals( out_pVertices, pTileIndices, lstTileData, nTotalWidth, nTotalDepth, fHeightScale, out_pNormals );

	*out_nVertexCount	= nVertexCount;
	*out_nIndexCount	= nIndexCount;
	if( out_nMeshAttributeCount )
		*out_nMeshAttributeCount = nAttributeCount;

	if( 0 != out_pNormals && 0 == out_lstTileIndices )
		free( pTileIndices );

	return 0;
};


void CalcNormals( const Vector3* pVertices, const GND_TILE_INDICES* pTileIndices,
				 const GND_TILE_DATA* pTileData, uint32_t nWidth, uint32_t nDepth, 
				 float fHeightScale, Vector3 *out_pNormals )
{
	float fScale = .1f;
	if( fHeightScale != 0.0f )
		fScale = 1.0f/fHeightScale;
	uint32_t i, 
		nTotalTiles = nWidth*nDepth;
	const GND_TILE_INDICES* pIdxCurrent=0, * pIdxFront=0, * pIdxRight=0, * pIdxFrontRight=0;
	const GND_TILE_DATA* pTileCurrent=0, * pTileFront=0, * pTileRight=0, * pTileFrontRight=0;

	Vector3 BlendNormalFinal;
	Vector3 BlendNormal0;
	Vector3 BlendNormal1;
	Vector3 BlendNormal2;
	Vector3 BlendNormal3;

	// first pass builds triangle normals independently (no normal smoothing)
	for( i=0; i<nTotalTiles; i++ )
	{
		pIdxCurrent		= &pTileIndices[i];
		pTileCurrent	= &pTileData[i];
		if( -1 == pIdxCurrent->VerticesTop[0] )
			continue;
		////-------------------------------
		//BlendNormal0 = ( (pVertices[pIdxCurrent->VerticesTop[2]] - pVertices[pIdxCurrent->VerticesTop[0]]) )
		//	.Cross( (pVertices[pIdxCurrent->VerticesTop[1]] - pVertices[pIdxCurrent->VerticesTop[0]]) );

		//BlendNormal1 = ( (pVertices[pIdxCurrent->VerticesTop[3]] - pVertices[pIdxCurrent->VerticesTop[1]]) )
		//	.Cross( -(pVertices[pIdxCurrent->VerticesTop[0]] - pVertices[pIdxCurrent->VerticesTop[1]]) );

		//BlendNormal2 = ( (pVertices[pIdxCurrent->VerticesTop[3]] - pVertices[pIdxCurrent->VerticesTop[2]]) )
		//	.Cross( (pVertices[pIdxCurrent->VerticesTop[0]] - pVertices[pIdxCurrent->VerticesTop[2]]) );

		//BlendNormal3 = ( (pVertices[pIdxCurrent->VerticesTop[2]] - pVertices[pIdxCurrent->VerticesTop[3]]) )
		//	.Cross( -(pVertices[pIdxCurrent->VerticesTop[1]] - pVertices[pIdxCurrent->VerticesTop[3]]) );

		BlendNormal0 = ( Vector3(0.0f, pTileCurrent->fHeight[2]*fScale, 1.0f) - Vector3(0.0f, pTileCurrent->fHeight[0]*fScale, 0.0f) )
			.Cross( -(Vector3(1.0f, pTileCurrent->fHeight[1]*fScale, 0.0f) - Vector3(0.0f, pTileCurrent->fHeight[0]*fScale, 0.0f)) );

		BlendNormal1 = ( Vector3(1.0f, pTileCurrent->fHeight[3]*fScale, 1.0f) - Vector3(1.0f, pTileCurrent->fHeight[1]*fScale, 0.0f) )
			.Cross( (Vector3(0.0f, pTileCurrent->fHeight[0]*fScale, 0.0f) - Vector3(1.0f, pTileCurrent->fHeight[1]*fScale, 0.0f)) );

		BlendNormal2 = ( Vector3(1.0f, pTileCurrent->fHeight[3]*fScale, 1.0f) - Vector3(0.0f, pTileCurrent->fHeight[2]*fScale, 1.0f) )
			.Cross( -(Vector3(0.0f, pTileCurrent->fHeight[0]*fScale, 0.0f) - Vector3(0.0f, pTileCurrent->fHeight[2]*fScale, 1.0f)) );

		BlendNormal3 = ( Vector3(0.0f, pTileCurrent->fHeight[2]*fScale, 1.0f) - Vector3(1.0f, pTileCurrent->fHeight[3]*fScale, 1.0f) )
			.Cross( (Vector3(1.0f, pTileCurrent->fHeight[1]*fScale, 0.0f) - Vector3(1.0f, pTileCurrent->fHeight[3]*fScale, 1.0f)) );

		out_pNormals[pIdxCurrent->VerticesTop[0]] = 
		out_pNormals[pIdxCurrent->VerticesTop[1]] = 
		out_pNormals[pIdxCurrent->VerticesTop[2]] = 
		out_pNormals[pIdxCurrent->VerticesTop[3]] = (BlendNormal3+BlendNormal1+BlendNormal2+BlendNormal0).Normalize();
	}

	Vector3 preBlendFrontRight; 
	// iterate again to perform the normal blending
	for( i=0; i<nTotalTiles; i++ )
	{
		pIdxCurrent		= &pTileIndices[i];
		pTileCurrent	= &pTileData[i];
		if( -1 == pIdxCurrent->VerticesTop[0] )
			continue;

		if( i < (nTotalTiles-1) )
		{
			pIdxFront	= &pTileIndices[i+1];
			pTileFront	= &pTileData[i+1];
			if( -1 == pIdxFront->VerticesTop[0] )
			{ 
				pTileFront = 0; 
				pIdxFront = 0;
			}

			if( i < (nTotalTiles-nWidth) )
			{
				pIdxRight		= &pTileIndices[i+nWidth];
				pTileRight		= &pTileData[i+nWidth];
				if( -1 == pIdxRight->VerticesTop[0] )
				{ 
					pTileRight = 0; 
					pIdxRight = 0;
				}

				if( i < (nTotalTiles-nWidth-1) )
				{
					pIdxFrontRight	= &pTileIndices[i+nWidth+1];
					pTileFrontRight	= &pTileData[i+nWidth+1];
					if( -1 == pIdxFrontRight->VerticesTop[0] )
					{ 
						pTileFrontRight = 0; 
						pIdxFrontRight	= 0;
					}
				}
				else
				{
					pIdxFrontRight	= 0;
					pTileFrontRight	= 0;
				}
			}
			else
			{
				pIdxRight		= 0;
				pIdxFrontRight	= 0;
				pTileRight		= 0;
				pTileFrontRight	= 0;
			};
		}
		else
		{
			pIdxFront		= 0;
			pIdxRight		= 0;
			pIdxFrontRight	= 0;
			pTileFront		= 0;
			pTileRight		= 0;
			pTileFrontRight	= 0;
		};
//

		if( -1 != pIdxCurrent->VerticesTop[0] )
		{
			preBlendFrontRight = out_pNormals[pIdxCurrent->VerticesTop[3]]*.5f;

//			preBlendFrontRight += out_pNormals[pIdxCurrent->VerticesTop[0]]*.25f;
			preBlendFrontRight += out_pNormals[pIdxCurrent->VerticesTop[1]]*.25f;
			preBlendFrontRight += out_pNormals[pIdxCurrent->VerticesTop[2]]*.25f;
			preBlendFrontRight.Normalize();
		}
		else
			preBlendFrontRight.x = preBlendFrontRight.y = preBlendFrontRight.z = 0;

		if( pIdxFront 
			&& (fabs(pTileCurrent->fHeight[3] - pTileFront->fHeight[2]) < 0.0001f) )
		{
			preBlendFrontRight += out_pNormals[pIdxFront->VerticesTop[2]]*.25f;

			preBlendFrontRight += out_pNormals[pIdxFront->VerticesTop[0]]*.125f;;
//			preBlendFrontRight += out_pNormals[pIdxFront->VerticesTop[1]]*.125f;
			preBlendFrontRight += out_pNormals[pIdxFront->VerticesTop[3]]*.125f;
		}
		if( pIdxRight 
			&& (fabs(pTileCurrent->fHeight[3] - pTileRight->fHeight[1]) < 0.0001f) )
		{
			preBlendFrontRight += out_pNormals[pIdxRight->VerticesTop[1]]*.25f;

			preBlendFrontRight += out_pNormals[pIdxRight->VerticesTop[0]]*.125f;
//			preBlendFrontRight += out_pNormals[pIdxRight->VerticesTop[2]]*.25f;
			preBlendFrontRight += out_pNormals[pIdxRight->VerticesTop[3]]*.125f;
		}
		if( pIdxFrontRight 
			&& (fabs(pTileCurrent->fHeight[3] - pTileFrontRight->fHeight[0]) < 0.0001f) )
		{
			preBlendFrontRight += out_pNormals[pIdxFrontRight->VerticesTop[0]]*.25f;

			preBlendFrontRight += out_pNormals[pIdxFrontRight->VerticesTop[1]]*.125f;
			preBlendFrontRight += out_pNormals[pIdxFrontRight->VerticesTop[2]]*.125f;
//			preBlendFrontRight += out_pNormals[pIdxFrontRight->VerticesTop[3]]*.125f;
		}

		preBlendFrontRight.Normalize();

		// ---------------------- We average the current tile normal with the front-right tiles 
		out_pNormals[pIdxCurrent->VerticesTop[3]]			= (preBlendFrontRight);
		out_pNormals[pIdxCurrent->VerticesTop[3]].Normalize();
		if( pIdxFront 
			&& (fabs(pTileCurrent->fHeight[3] - pTileFront->fHeight[2]) < 0.0001f) )
		{
			out_pNormals[pIdxFront->VerticesTop[2]]			= (preBlendFrontRight);
			out_pNormals[pIdxFront->VerticesTop[2]].Normalize();
		}
		if( pIdxRight 
			&& (fabs(pTileCurrent->fHeight[3] - pTileRight->fHeight[1]) < 0.0001f) )
		{
			out_pNormals[pIdxRight->VerticesTop[1]]			= (preBlendFrontRight);
			out_pNormals[pIdxRight->VerticesTop[1]].Normalize();
		}
		if( pIdxFrontRight 
			&& (fabs(pTileCurrent->fHeight[3] - pTileFrontRight->fHeight[0]) < 0.0001f) )
		{
			out_pNormals[pIdxFrontRight->VerticesTop[0]]	= (preBlendFrontRight);
			out_pNormals[pIdxFrontRight->VerticesTop[0]].Normalize();
		}

	};

};

GND_TILE_DATA* ropp::GetFrontTile( GND_TILE_DATA* in_lstTiles, uint32_t nCurrentTileIndex, uint16_t nWidth, uint16_t nHeight )
{
	uint32_t nTotalTiles = nWidth*nHeight;
	if( nCurrentTileIndex < nTotalTiles-1 )
		return &in_lstTiles[nCurrentTileIndex+1];
	return 0;
};

GND_TILE_DATA* ropp::GetBackTile( GND_TILE_DATA* in_lstTiles, uint32_t nCurrentTileIndex, uint16_t nWidth, uint16_t nHeight )
{
	if( nCurrentTileIndex > 0 )
		return &in_lstTiles[nCurrentTileIndex-1];
	return 0;
};

GND_TILE_DATA* ropp::GetRightTile( GND_TILE_DATA* in_lstTiles, uint32_t nCurrentTileIndex, uint16_t nWidth, uint16_t nHeight )
{
	uint32_t nTotalTiles = nWidth*nHeight;
	if( nCurrentTileIndex < nTotalTiles-nWidth )
		return &in_lstTiles[nCurrentTileIndex+nWidth];
	return 0;
};
GND_TILE_DATA* ropp::GetLeftTile( GND_TILE_DATA* in_lstTiles, uint32_t nCurrentTileIndex, uint16_t nWidth, uint16_t nHeight )
{
	if( nCurrentTileIndex > nWidth )
		return &in_lstTiles[nCurrentTileIndex-nWidth];
	return 0;
};

GND_TILE_DATA* ropp::GetFrontRightTile( GND_TILE_DATA* in_lstTiles, uint32_t nCurrentTileIndex, uint16_t nWidth, uint16_t nHeight )
{
	uint32_t nTotalTiles = nWidth*nHeight;
	if( nCurrentTileIndex < nTotalTiles-(nWidth+1) )
		return &in_lstTiles[nCurrentTileIndex+(nWidth+1)];
	return 0;
};

GND_TILE_DATA* ropp::GetBackRightTile( GND_TILE_DATA* in_lstTiles, uint32_t nCurrentTileIndex, uint16_t nWidth, uint16_t nHeight )
{
	uint32_t nTotalTiles = nWidth*nHeight;
	if( nCurrentTileIndex < nTotalTiles-(nWidth-1) )
		return &in_lstTiles[nCurrentTileIndex+(nWidth-1)];
	return 0;
};
GND_TILE_DATA* ropp::GetFrontLeftTile( GND_TILE_DATA* in_lstTiles, uint32_t nCurrentTileIndex, uint16_t nWidth, uint16_t nHeight )
{
	if( nCurrentTileIndex > nWidth-1 )
		return &in_lstTiles[nCurrentTileIndex-(nWidth-1)];
	return 0;
};

GND_TILE_DATA* ropp::GetBackLeftTile( GND_TILE_DATA* in_lstTiles, uint32_t nCurrentTileIndex, uint16_t nWidth, uint16_t nHeight )
{
	if( nCurrentTileIndex > nWidth+1 )
		return &in_lstTiles[nCurrentTileIndex-(nWidth+1)];
	return 0;
};

//------------------------------------------------------------------------------ Same but const
const GND_TILE_DATA* ropp::GetFrontTile( const GND_TILE_DATA* in_lstTiles, uint32_t nCurrentTileIndex, uint16_t nWidth, uint16_t nHeight )
{
	uint32_t nTotalTiles = nWidth*nHeight;
	if( nCurrentTileIndex < nTotalTiles-1 )
		return &in_lstTiles[nCurrentTileIndex+1];
	return 0;
};

const GND_TILE_DATA* ropp::GetBackTile( const GND_TILE_DATA* in_lstTiles, uint32_t nCurrentTileIndex, uint16_t nWidth, uint16_t nHeight )
{
	if( nCurrentTileIndex > 0 )
		return &in_lstTiles[nCurrentTileIndex-1];
	return 0;
};

const GND_TILE_DATA* ropp::GetRightTile( const GND_TILE_DATA* in_lstTiles, uint32_t nCurrentTileIndex, uint16_t nWidth, uint16_t nHeight )
{
	uint32_t nTotalTiles = nWidth*nHeight;
	if( nCurrentTileIndex < nTotalTiles-nWidth )
		return &in_lstTiles[nCurrentTileIndex+nWidth];
	return 0;
};
const GND_TILE_DATA* ropp::GetLeftTile( const GND_TILE_DATA* in_lstTiles, uint32_t nCurrentTileIndex, uint16_t nWidth, uint16_t nHeight )
{
	if( nCurrentTileIndex > nWidth )
		return &in_lstTiles[nCurrentTileIndex-nWidth];
	return 0;
};

const GND_TILE_DATA* ropp::GetFrontRightTile( const GND_TILE_DATA* in_lstTiles, uint32_t nCurrentTileIndex, uint16_t nWidth, uint16_t nHeight )
{
	uint32_t nTotalTiles = nWidth*nHeight;
	if( nCurrentTileIndex < nTotalTiles-(nWidth+1) )
		return &in_lstTiles[nCurrentTileIndex+(nWidth+1)];
	return 0;
};

const GND_TILE_DATA* ropp::GetBackRightTile( const GND_TILE_DATA* in_lstTiles, uint32_t nCurrentTileIndex, uint16_t nWidth, uint16_t nHeight )
{
	uint32_t nTotalTiles = nWidth*nHeight;
	if( nCurrentTileIndex < nTotalTiles-(nWidth-1) )
		return &in_lstTiles[nCurrentTileIndex+(nWidth-1)];
	return 0;
};
const GND_TILE_DATA* ropp::GetFrontLeftTile( const GND_TILE_DATA* in_lstTiles, uint32_t nCurrentTileIndex, uint16_t nWidth, uint16_t nHeight )
{
	if( nCurrentTileIndex > nWidth-1 )
		return &in_lstTiles[nCurrentTileIndex-(nWidth-1)];
	return 0;
};

const GND_TILE_DATA* ropp::GetBackLeftTile( const GND_TILE_DATA* in_lstTiles, uint32_t nCurrentTileIndex, uint16_t nWidth, uint16_t nHeight )
{
	if( nCurrentTileIndex > nWidth+1 )
		return &in_lstTiles[nCurrentTileIndex-(nWidth+1)];
	return 0;
};
