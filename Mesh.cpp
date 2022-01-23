#include "Mesh.h"
#include<d3d9.h>
#include<memory>

	Geometry::Mesh::Mesh(): VertexBuffer(),
							IndexBuffer(),
							Texture(),
							NumVertices(),
							NumIndices(),
							WorldMatrix(){};


	Geometry::Mesh::~Mesh(){};

	void Geometry::Mesh::SetVertexBuffer(IDirect3DVertexBuffer9 * ptr)
	{
		VertexBuffer = ptr;
	};

	void Geometry::Mesh::SetIndexBuffer(IDirect3DIndexBuffer9 * ptr)
	{
		IndexBuffer = ptr;
	};

	void Geometry::Mesh::SetTexture(IDirect3DTexture9 * ptr)
	{
		Texture = ptr;
	};

	void Geometry::Mesh::SetTM(const POD_Math::Matrix4x4& tm)
	{
		memcpy(&WorldMatrix, &tm, sizeof(POD_Math::Matrix4x4));	
	};

	void Geometry::Mesh::SetNumVertices(const size_t NumV)
	{
		NumVertices=NumV;
	};

	void Geometry::Mesh::SetNumIndices(const size_t NumI)
	{
		NumIndices=NumI;	
	};

	IDirect3DVertexBuffer9 * Geometry::Mesh::GetVertexBuffer()const
	{
		return VertexBuffer;	
	};

	IDirect3DIndexBuffer9  * Geometry::Mesh::GetIndexBuffer()const
	{
		return IndexBuffer;	
	};

	IDirect3DTexture9      * Geometry::Mesh::GetTexture()const
	{
		return Texture;	
	};

	SkeletalAnimation::Skeleton& Geometry::Mesh::GetSkeleton()
	{
		return MeshSkeleton;
	};

	POD_Math::Matrix4x4& Geometry::Mesh::GetTM()
	{
		return WorldMatrix;	
	};

	size_t Geometry::Mesh::GetNumVertices()const
	{
		return NumVertices;
	};

	size_t Geometry::Mesh::GetNumIndices()const
	{
		return NumIndices;	
	};

	void Geometry::Mesh::Destroy()
	{
		VertexBuffer->Release();
		IndexBuffer->Release();
		Texture->Release();
	};