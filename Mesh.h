#ifndef MESH_H
#define MESH_H

#include "Skeleton.h"
#include "Matrix4x4.h"

	struct IDirect3DVertexBuffer9;
	struct IDirect3DIndexBuffer9;
	struct IDirect3DTexture9;

	namespace Geometry
	{

		#pragma pack(push,1)
		struct Header
		{
			unsigned long  magic;
			unsigned long  NumVerts;
			unsigned long  NumInds;
			unsigned short NumBones;   
			unsigned short NumKeyframes;
		}; 

		struct BlendVert
		{
			POD_Math::Vector3 Position;   
			POD_Math::Vector3 Normal;
	        float             texture[2];
	        unsigned char     inds[4];
	        float             weight[4];
		};

		struct Face32
		{
			unsigned long A,B,C;
		};

		#pragma pack(pop)

		class Mesh
		{

		public:

			Mesh();
			~Mesh();

			void SetVertexBuffer(IDirect3DVertexBuffer9*);
			void SetIndexBuffer(IDirect3DIndexBuffer9*);
			void SetTexture(IDirect3DTexture9*);
			void SetTM(const POD_Math::Matrix4x4&);
			void SetNumVertices(const size_t);
			void SetNumIndices(const size_t);

			IDirect3DVertexBuffer9        *GetVertexBuffer()const;
			IDirect3DIndexBuffer9         *GetIndexBuffer()const;
			IDirect3DTexture9             *GetTexture()const;
			POD_Math::Matrix4x4&           GetTM();
			SkeletalAnimation::Skeleton&   GetSkeleton();
			size_t                         GetNumVertices()const;
			size_t                         GetNumIndices()const;

			void Destroy(); 

		private:

			SkeletalAnimation::Skeleton MeshSkeleton;
			POD_Math::Matrix4x4         WorldMatrix;
			IDirect3DVertexBuffer9      *VertexBuffer;
			IDirect3DIndexBuffer9       *IndexBuffer;
			IDirect3DTexture9           *Texture;
			size_t                      NumVertices;
			size_t                      NumIndices;
		};

	};

#endif 