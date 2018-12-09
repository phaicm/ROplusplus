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
#ifndef _ROPP_DEFINES_H
#define _ROPP_DEFINES_H

#ifdef __APPLE__
#define __int8 char
#define __int16 short
#define __int32 int
#define __int64 long
#endif /* __APPLE __ */

typedef __int8		int8_t;
typedef unsigned __int8		uint8_t;
typedef __int16		int16_t;
typedef unsigned __int16	uint16_t;
typedef __int32		int32_t;
typedef unsigned __int32	uint32_t;

#include <math.h>
#include <memory.h>
#include <stdlib.h>

namespace ropp
	{
#pragma pack( push, 1 )
	typedef uint32_t GEOMETRY_FLAG;

	// I'm drunk while coding thesef lags--which I never implemented... maybe later (?)
	static const GEOMETRY_FLAG GEOMETRY_OPTIMIZED	= 0x00000001L;
	static const GEOMETRY_FLAG GEOMETRY_SPLIT		= 0x00000002L;
//9#pragma pack( push, 1 )
	struct Matrix3
	{
		float _11, _12, _13, 
			_21, _22, _23, 
			_31, _32, _33;
	};

#ifndef max
#define max(a, b)((a>b)?a:b)
#define min(a, b)((a<b)?a:b)
#endif


	struct Vector3
	{
		float x, y, z;
		Vector3( void ){};
		Vector3( const Vector3& v ){x = v.x; y=v.y; z=v.z;};
		Vector3( float _x, float _y, float _z )
		{ x = _x; y = _y; z = _z; };
		const Vector3&		operator =( const Vector3& v )
		{ 
			x = v.x; y=v.y; z=v.z;
			return *this;
		};

		Vector3		operator +( const Vector3& other ) const
		{ return Vector3( x+other.x, y+other.y, z+other.z ); };
		Vector3		operator *( float scalar ) const
		{ return Vector3( x*scalar, y*scalar, z*scalar ); };
		Vector3		operator /( float scalar ) const
		{ return Vector3( x/scalar, y/scalar, z/scalar ); };
		const Vector3&	operator*=(float scalar)
		{
			x *= scalar; y *= scalar; z *= scalar;
			return *this;
		}
		const Vector3&	operator*=(const Matrix3 &m)
		{
			float _x, _y, _z;
			_x = x*m._11 + y*m._21 + z*m._31;
			_y = x*m._12 + y*m._22 + z*m._32;
			_z = x*m._13 + y*m._23 + z*m._33;
			x = _x; y = _y; z = _z;
			return *this;
		}
		Vector3			operator*(const Matrix3 &m) const
		{
			Vector3 vcResult;
			vcResult.x = x*m._11 + y*m._21 + z*m._31;
			vcResult.y = x*m._12 + y*m._22 + z*m._32;
			vcResult.z = x*m._13 + y*m._23 + z*m._33;
			return vcResult;
		}
		Vector3				operator -( const Vector3& other ) const
		{ return Vector3( x-other.x, y-other.y, z-other.z ); };
		const Vector3&		operator +=( const Vector3& other )
		{ 
			x+=other.x; y+=other.y; z+=other.z;
			return *this;
		};
		const Vector3&		operator -=( const Vector3& other )
		{ 
			x-=other.x; y-=other.y; z-=other.z;
			return *this;
		};
		Vector3				operator -( void ) const
		{ 
			return Vector3( x*-1, y*-1, z*-1 );
		};

		// 
		Vector3				Cross( const Vector3& other ) const
		{
			return Vector3(	y * other.z - z * other.y, 
							z * other.x - x * other.z, 
							x * other.y - y * other.x );
		};
		float			Dot( const Vector3& other ) const
		{
			return x * other.x + 
							y * other.y + 
							z * other.z ;
		};
		const Vector3&		Normalize( void )
		{
			float l = x*x+y*y+z*z;
			if( l == 0 )
				return *this;
			l = sqrtf( l );
			x = x / l;
			y = y / l;
			z = z / l;
			return *this;
		};

	};

	struct Vector2
	{
		float x, y;
		Vector2( void ){};
		Vector2( float _x, float _y )
		{ x = _x; y = _y; };

		const Vector2&		operator =( const Vector2& v )
		{ 
			x = v.x; y=v.y; 
			return *this;
		};
		Vector2		operator +( const Vector2& other ) const
		{ return Vector2( x+other.x, y+other.y ); };
		Vector2		operator *( float scalar ) const
		{ return Vector2( x*scalar, y*scalar ); };

		Vector2				operator -( const Vector2& other ) const
		{ return Vector2( x-other.x, y-other.y ); };
		const Vector2&		operator +=( const Vector2& other )
		{ 
			x+=other.x; y+=other.y; 
			return *this;
		};
		const Vector2&		operator -=( const Vector2& other )
		{ 
			x-=other.x; y-=other.y; 
			return *this;
		};
		Vector2				operator -( void ) const
		{ 
			return Vector2( x*-1, y*-1 );
		};

	};

	struct Quaternion
	{
		float x, y, z, w;
		Quaternion( void ){};
		Quaternion( float _x, float _y, float _z, float _w )
		{
			x = _x; y=_y; z=_z; w=_w;
		};
		Quaternion		operator*  ( const Vector3 &v ) const
		{
			return Quaternion(  w*v.x + y*v.z - z*v.y,
								w*v.y + z*v.x - x*v.z,
								w*v.z + x*v.y - y*v.x,
								-(x*v.x + y*v.y + z*v.z) );
		}
		Quaternion		operator*  ( const Quaternion &q ) const
		{
			Quaternion r;
			r.w = w*q.w - x*q.x - y*q.y - z*q.z;
			r.x = w*q.x + x*q.w + y*q.z - z*q.y;
			r.y = w*q.y + y*q.w + z*q.x - x*q.z;
			r.z = w*q.z + z*q.w + x*q.y - y*q.x;
			return r;
		};
		Quaternion			operator ~( void ) const { return Quaternion(-x, -y, -z, w); }
		const Quaternion& Normalize( void )
		{
			float m=x*x + y*y + z*z + w*w;
			if( m != 0 )
			{
				m=sqrtf(m);
				x /= m; y /= m; z /= m; w /= m;
			}
			else
				w = 1;
			return *this;
		};
		Vector3				RotateVector( const Vector3 &v ) const
		{ 
			Quaternion t(x, y, z, w);
			Quaternion r = t*v*(~t);
			return Vector3(r.x, r.y, r.z); 
		}
		void GetMatrix(	Matrix3 *out_pMatrix ) const
		{
		   float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

		   //// set matrix to identity
		   //pMat->_14 = pMat->_24 = pMat->_34 = 0;
		   //pMat->_44 = 1.0f;

		   x2 = x + x; 
		   y2 = y + y; 
		   z2 = z + z;

		   xx = x * x2;   
		   xy = x * y2;  
		   xz = x * z2;

		   yy = y * y2;   
		   yz = y * z2;   
		   zz = z * z2;

		   wx = w * x2;   
		   wy = w * y2;   
		   wz = w * z2;

		   //pMat->_12 = xy - wz;
		   //pMat->_13 = xz + wy;
		   out_pMatrix->_11 = 1.0f - (yy + zz);
		   out_pMatrix->_21 = xy - wz;
		   out_pMatrix->_31 = xz + wy;

		   //pMat->_21 = xy + wz;
		   //pMat->_23 = yz - wx;
		   out_pMatrix->_12 = xy + wz;
		   out_pMatrix->_22 = 1.0f - (xx + zz);
		   out_pMatrix->_32 = yz - wx;

		   //pMat->_31 = xz - wy;
		   //pMat->_32 = yz + wx;
		   out_pMatrix->_13 = xz - wy;
		   out_pMatrix->_23 = yz + wx;
		   out_pMatrix->_33 = 1.0f - (xx + yy);
		} // GetMatrix
	};


	// the following structures are meant sto simplify working with meshes
	
	typedef struct _MESH_ATTRIBUTE_DATA
	{
		uint32_t nAttributeID;
		uint32_t nTextureIndex;
		uint32_t nVertexStart;
		uint32_t nVertexCount;
		uint32_t nTriangleStart;
		uint32_t nTriangleCount;
	} MESH_ATTRIBUTE_DATA;
	
	typedef struct _MESH_BOUNDINGVOLUME_DATA
	{
		Vector3		vCenter, vMin, vMax;
	} MESH_BOUNDINGVOLUME_DATA, MESH_BV_DATA;

	// I can hardly imagine a RSM mesh with more than 64 textures in a single node, or with 
	// more than 65535 attributes, so I will limit to 16 bit indices and 64 attributes max
	typedef struct _MESH_NODE_DATA
	{
		uint32_t		nAttributeCount;
		uint16_t		lstAttributeIndices[64]; 
		Vector3			vPosition;
		Quaternion		qOrientation;
		Vector3			vScale;
		MESH_BV_DATA	BoundingVolume;
		uint32_t		nParentIndex;
	} MESH_NODE_DATA;

#pragma pack( pop )
}

#endif // _ROPP_DEFINES_H