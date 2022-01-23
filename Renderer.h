#ifndef RENDERER_H
#define RENDERER_H

#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>

#pragma comment(lib,"d3d9")
#pragma comment(lib,"d3dx9")


	class Camera;

	namespace POD_Math
	{
		struct Matrix4x4;
	};

	namespace Geometry 
	{
		class Mesh;
	};

	namespace SkeletalAnimation 
	{
		class Skeleton;
	};

	class D3D9_Renderer
	{
	
	public:

		D3D9_Renderer();
		~D3D9_Renderer();

	    HRESULT Initialize(HWND hWnd);
		IDirect3DDevice9*GetDevice()const; 

		Geometry::Mesh *CreateMeshFromFile(const wchar_t *,const wchar_t *);
		void BeginScene();
		void RenderMesh(Geometry::Mesh&, POD_Math::Matrix4x4&,const unsigned long,SkeletalAnimation::Skeleton&);
		void EndScene();
		void SetCamera(Camera*const);
		void Destroy();

		UINT GetWidth()const;
		UINT GetHeight()const;

		UINT Width;
        UINT Height;
 
	protected:

		Camera                      * myCamera;
		IDirect3D9                  * d3d;
		IDirect3DDevice9            * device;
		IDirect3DVertexShader9      * vshader;
		IDirect3DPixelShader9       * pshader;
		IDirect3DVertexDeclaration9 * decl;
 
	};



#endif