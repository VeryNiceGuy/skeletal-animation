#include "Renderer.h"
#include "Mesh.h"
#include "Camera.h"
#include <new>
#include "Matrix4x4.h"
#include "ErrorHandler.h"



	D3D9_Renderer::D3D9_Renderer() : d3d(), 
		                             device(),
									 vshader(),
									 decl(),
									 myCamera(),
									 Width(),
									 Height()
	{};

	D3D9_Renderer::~D3D9_Renderer(){};

	HRESULT D3D9_Renderer::Initialize(HWND hWnd)
	{

		D3DDISPLAYMODE d3ddm        =  D3DDISPLAYMODE();
		D3DCAPS9 d3dCaps            =  D3DCAPS9();
		D3DPRESENT_PARAMETERS d3dpp =  D3DPRESENT_PARAMETERS();
		ID3DXBuffer*FunctionVS      =  0;
		ID3DXBuffer*FunctionPS      =  0;


		D3DVERTEXELEMENT9 BlendDecl[] = 
		{
			{0, 0,  D3DDECLTYPE_FLOAT3,    D3DDECLMETHOD_DEFAULT,   D3DDECLUSAGE_POSITION,     0},
			{0, 12, D3DDECLTYPE_FLOAT3,    D3DDECLMETHOD_DEFAULT,   D3DDECLUSAGE_NORMAL,       0},
			{0, 24, D3DDECLTYPE_FLOAT2,    D3DDECLMETHOD_DEFAULT,   D3DDECLUSAGE_TEXCOORD,     0},
			{0, 32, D3DDECLTYPE_UBYTE4,    D3DDECLMETHOD_DEFAULT,   D3DDECLUSAGE_BLENDINDICES, 0},
			{0, 36, D3DDECLTYPE_FLOAT4,    D3DDECLMETHOD_DEFAULT,   D3DDECLUSAGE_BLENDWEIGHT,  0},
			D3DDECL_END()
		};

		// Ещё не решил что с этим делать...
		d3d = Direct3DCreate9( D3D_SDK_VERSION );
		if(!d3d) return 0;

		// Все возвращаемые значения отправляются премяком в хандлер.

		ErrorHandler::CheckD3D(d3d->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm ) );

		ErrorHandler::CheckD3D(d3d->CheckDeviceFormat( D3DADAPTER_DEFAULT,
														D3DDEVTYPE_HAL, 
														d3ddm.Format,
														D3DUSAGE_DEPTHSTENCIL,
														D3DRTYPE_SURFACE,
														D3DFMT_D16 ) );

		ErrorHandler::CheckD3D(d3d->GetDeviceCaps( D3DADAPTER_DEFAULT, 
													D3DDEVTYPE_HAL,
													&d3dCaps ) );


		ErrorHandler::CheckD3D(d3d->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT,
																D3DDEVTYPE_HAL,
																d3ddm.Format,
																false,
																D3DMULTISAMPLE_2_SAMPLES,
																0));

		DWORD dwBehaviorFlags = D3DCREATE_FPU_PRESERVE;

		if( d3dCaps.VertexProcessingCaps != 0 )
		{
			dwBehaviorFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
		    dwBehaviorFlags |= D3DCREATE_PUREDEVICE;
		}

		else dwBehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
		

		Width  = d3ddm.Width;
		Height = d3ddm.Height;

		d3dpp.SwapEffect                 = D3DSWAPEFFECT_DISCARD;
		d3dpp.EnableAutoDepthStencil     = TRUE;
		d3dpp.AutoDepthStencilFormat     = D3DFMT_D16;
		d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;
		d3dpp.Windowed                   = FALSE;
		d3dpp.BackBufferWidth            = d3ddm.Width;
		d3dpp.BackBufferHeight           = d3ddm.Height;
		d3dpp.FullScreen_RefreshRateInHz = d3ddm.RefreshRate;
		d3dpp.BackBufferFormat           = d3ddm.Format; 
		d3dpp.MultiSampleType            = D3DMULTISAMPLE_2_SAMPLES;
  
		ErrorHandler::CheckD3D( d3d->CreateDevice( D3DADAPTER_DEFAULT,
													D3DDEVTYPE_HAL,
													hWnd,
													dwBehaviorFlags,
													&d3dpp,
													&device ));

		// Думаю в конце концов выбросить D3DX
		ErrorHandler::CheckD3D(D3DXCompileShaderFromFileW(L"blender.vsh",
															0,
															0,
															"vs_main",
															"vs_3_0",
															0,
															&FunctionVS,
															0,
															0));

		ErrorHandler::CheckD3D(D3DXCompileShaderFromFileW(L"blender.psh",
															0,
															0,
															"ps_main",
															"ps_3_0",
															0,
															&FunctionPS,
															0,
															0));

		ErrorHandler::CheckD3D(device->CreateVertexDeclaration(BlendDecl, &decl));
		ErrorHandler::CheckD3D(device->CreateVertexShader(static_cast<unsigned long*>(FunctionVS->GetBufferPointer()), &vshader));
		ErrorHandler::CheckD3D(device->CreatePixelShader(static_cast<unsigned long*>(FunctionPS->GetBufferPointer()), &pshader));

		ErrorHandler::CheckD3D(device->SetVertexDeclaration(decl));
		ErrorHandler::CheckD3D(device->SetVertexShader(vshader));
		ErrorHandler::CheckD3D(device->SetPixelShader(pshader));

		FunctionVS->Release();
		FunctionPS->Release();

		ErrorHandler::CheckD3D(device->SetRenderState( D3DRS_LIGHTING, FALSE ));
		ErrorHandler::CheckD3D(device->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS, TRUE ));  

		return S_OK;
	};


	IDirect3DDevice9* D3D9_Renderer::GetDevice()const 
	{
		return device;
	};

	Geometry::Mesh *D3D9_Renderer::CreateMeshFromFile(const wchar_t *filename,
		                                              const wchar_t *tfilename)
	{
		
		IDirect3DVertexBuffer9 *tVB=0;
		IDirect3DIndexBuffer9  *tIB=0;
		IDirect3DTexture9      *tTX=0;

		POD_Math::Matrix4x4 WorldM={1,0,0,0, 
									0,1,0,0,
									0,0,1,0,
									0,0,0,1};
		
		void * vdata=0;
		void * idata=0;

		Geometry::Header head=Geometry::Header();
		unsigned long ReadBytes=0;

		Geometry::Mesh*  mesh = new(std::nothrow) Geometry::Mesh; // Только для демо!

		SkeletalAnimation::Skeleton& Skeleton = mesh->GetSkeleton();

		HANDLE file=CreateFile(filename,
		                   GENERIC_READ,
						   0,
						   0,
						   OPEN_EXISTING,
						   FILE_ATTRIBUTE_NORMAL,
						   0);

	    ReadFile(file,&head,sizeof(Geometry::Header),&ReadBytes,0);

		SkeletalAnimation::vBone*     Bones     = new(std::nothrow) SkeletalAnimation::vBone[head.NumBones]();
		SkeletalAnimation::Keyframe*  Keyframes = new(std::nothrow) SkeletalAnimation::Keyframe[head.NumBones*head.NumKeyframes]();

		ErrorHandler::CheckD3D(device->CreateVertexBuffer(head.NumVerts*sizeof(Geometry::BlendVert),
															D3DUSAGE_WRITEONLY,
															0,
															D3DPOOL_MANAGED,
															&tVB,
															0));

		ErrorHandler::CheckD3D(device->CreateIndexBuffer(head.NumInds*sizeof(Geometry::Face32),
															D3DUSAGE_WRITEONLY,
															D3DFMT_INDEX32,
															D3DPOOL_MANAGED,
															&tIB,
															0));

		ErrorHandler::CheckD3D(tVB->Lock(0,0,&vdata,0));

					if(!ReadFile(file,
							vdata,
							sizeof(Geometry::BlendVert)*head.NumVerts,
							&ReadBytes,
							0))ErrorHandler::ThreadLastError();

		ErrorHandler::CheckD3D(tVB->Unlock()); // А вдруг...

		ErrorHandler::CheckD3D(tIB->Lock(0,0,&idata,0));

					if(!ReadFile(file,
							idata,
							sizeof(Geometry::Face32)*head.NumInds,
							&ReadBytes,
							0))ErrorHandler::ThreadLastError();

		ErrorHandler::CheckD3D(tIB->Unlock());

					if(!ReadFile(file,
							Bones,
							sizeof(SkeletalAnimation::vBone)*head.NumBones,
							&ReadBytes,
							0))ErrorHandler::ThreadLastError();
					
				
					if(!ReadFile(file,
								Keyframes,
								sizeof(SkeletalAnimation::Keyframe)*head.NumKeyframes*head.NumBones,
								&ReadBytes,
								0))ErrorHandler::ThreadLastError();
        
		CloseHandle(file);

		ErrorHandler::CheckD3D(D3DXCreateTextureFromFile(device, tfilename, &tTX));

		ErrorHandler::CheckD3D(GetDevice()->SetStreamSource(0,tVB,0,sizeof(Geometry::BlendVert)));
	    ErrorHandler::CheckD3D(GetDevice()->SetIndices(tIB));
		ErrorHandler::CheckD3D(GetDevice()->SetTexture(0,tTX));

		// А это уже мои)
		mesh->SetVertexBuffer(tVB);
		mesh->SetIndexBuffer(tIB);
		mesh->SetTexture(tTX);
		mesh->SetNumVertices(head.NumVerts);
		mesh->SetNumIndices(head.NumInds);
		mesh->SetTM(WorldM);

		Skeleton.Build(Bones,
			           Keyframes,
					   head.NumBones,
					   head.NumKeyframes);

		

		return mesh;
	};
	
	void D3D9_Renderer::BeginScene()
	{		
		device->Clear( 0,
					NULL,
					D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
					D3DCOLOR_COLORVALUE(0.0f,1.0f,0.0f,1.0f),
					1.0f,
					0 );

        ErrorHandler::CheckD3D(device->BeginScene());
	};

	void D3D9_Renderer::RenderMesh(Geometry::Mesh& mesh,
									POD_Math::Matrix4x4& p,
									const unsigned long t,
									SkeletalAnimation::Skeleton& s)
	{
	
		if(!myCamera) 
		{
			ErrorHandler::ShowWarning(L"Камера не задана");
			return;
		};

		mesh.GetSkeleton().Update(t);

		POD_Math::Matrix4x4 WorldView           = POD_Math::Matrix4x4();
		POD_Math::Matrix4x4 WorldViewProjection = POD_Math::Matrix4x4();

     
		POD_Math::Matrix4x4Mul(WorldView, mesh.GetTM(), myCamera->GetViewMatrix());
	    POD_Math::Matrix4x4Mul(WorldViewProjection, WorldView, myCamera->GetProjectionMatrix());

		
		ErrorHandler::CheckD3D(device->SetVertexShaderConstantF(0,
								reinterpret_cast<float*>(&WorldViewProjection),
								4));
		// Это для нашего не совсем корректного Ламберта
	    ErrorHandler::CheckD3D(device->SetVertexShaderConstantF(4,
								reinterpret_cast<float*>(&mesh.GetTM()),
								4));

		ErrorHandler::CheckD3D(device->SetVertexShaderConstantF(8,
								reinterpret_cast<float*>(mesh.GetSkeleton().GetSharedArray()),
								3 * mesh.GetSkeleton().GetNumBones()));


		ErrorHandler::CheckD3D(device->SetStreamSource(0,mesh.GetVertexBuffer(),0,sizeof(Geometry::BlendVert)));
		ErrorHandler::CheckD3D(device->SetIndices(mesh.GetIndexBuffer()));
		ErrorHandler::CheckD3D(device->SetTexture(0,mesh.GetTexture()));
		
		ErrorHandler::CheckD3D(device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
								0,
								0,
								mesh.GetNumVertices(),
								0,
								mesh.GetNumIndices()));

	};

	void D3D9_Renderer::EndScene()
	{
		ErrorHandler::CheckD3D(device->EndScene());
		ErrorHandler::CheckD3D(device->Present(0,0,0,0)); 
	};
	
	void D3D9_Renderer::SetCamera(Camera*const cam)
	{
		myCamera = cam;
	};

	void D3D9_Renderer::Destroy()
	{
		pshader->Release();
		vshader->Release();
		decl->Release();
		device->Release();
		d3d->Release();
	};

	UINT D3D9_Renderer::GetWidth()const
	{
		return Width;	
	};

	UINT D3D9_Renderer::GetHeight()const
	{
		return Height;	
	};