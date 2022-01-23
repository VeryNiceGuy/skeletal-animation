
#include <Windows.h>
#include <cstdio>

#include "Matrix4x3.h"
#include "Matrix4x4.h"
#include "Quaternion.h"
#include "Vector3.h"
#include <vector>
#include "Camera.h"
#include "Input.h"
#include "Skeleton.h"
#include "Renderer.h"
#include "Mesh.h"
#include <assert.h>
#include <ctime>
#include "ErrorHandler.h"


unsigned long g_dCurTime     = 0;
unsigned long g_fElpasedTime = 0;
unsigned long g_dLastTime    = 0;

//////////////////////////////////////////////////////////////////////////////////////////////

	POD_Math::Matrix4x4 Proj=POD_Math::Matrix4x4();
	POD_Math::Matrix4x4 ViewM=POD_Math::Matrix4x4();
	Camera cam; // Декларирую глобально только для демо

//////////////////////////////////////////////////////////////////////////////////////////////

	
	class Hale : public Input::InputListener
	{ 
		void OnKeyPressed( Input::KEYBOARD_INPUT input)
		{
			switch(input)
			{
				case Input::KEYBOARD_INPUT_UP:    cam.Forward(0.5f);   break;
			    case Input::KEYBOARD_INPUT_DOWN:  cam.Backward(0.5f);  break;
				case Input::KEYBOARD_INPUT_LEFT:  cam.Leftward(0.5f);  break;
				case Input::KEYBOARD_INPUT_RIGHT: cam.Rightward(0.5f); break;
			};
		};

		void OnKeyReleased( Input::KEYBOARD_INPUT input){};

		void OnMouseMove( const long x, const long y)
		{
			cam.Pitch((float)y * 0.001f);
			cam.Yaw((float)x *0.001f);
		};


	    void OnMousePressed( const long x, const long y, Input::MOUSE_INPUT){};
		void OnMouseReleased( const long x, const long y, Input::MOUSE_INPUT){}; 

	};
	
	Hale HaleInst;

//**********************************************************************************


LRESULT CALLBACK WndProc(HWND hWnd,UINT msg,WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{		
		case WM_INPUT:

					Input::InputSignal::GetInputFromQueue(lParam);
		break;

		case WM_DESTROY:
					PostQuitMessage(0);
		break;

		default: return DefWindowProc(hWnd,msg,wParam,lParam);
	};
	
	return 0;
};

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nShowCmd)
{
	WNDCLASSEX wcex  = WNDCLASSEX();
	MSG        msg = MSG();


    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
    wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = L"myClass";
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

	RegisterClassEx(&wcex);

	// Инициализируем ввод, задаем приемник
	Input::InputSignal::Initialize();
	Input::InputSignal::SetListener(&HaleInst);

	// Инстанцируем рендерер, задаем камеру
	D3D9_Renderer r;
	r.SetCamera(&cam);

	HWND hWnd = CreateWindowEx( NULL,
		                        L"myClass", 
                                L"Skeletal Animation Demo 2.0",
						        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
					            0, 0, 640, 480, NULL, NULL, hInstance, NULL );

	//Передаем хандлеру хандл окна
	ErrorHandler::SetHWND(hWnd);
	
	ShowWindow(hWnd,SW_NORMAL);
	UpdateWindow(hWnd);


	ShowCursor(false);

	// Инициализируем рендерер
	r.Initialize(hWnd);

	// Создаем матрицу перспективной проекции
	POD_Math::CreatePerspectiveFovLH(Proj,
									1.0f,
									1000.0f,
									D3DXToRadian(45.0f),
									(float)r.GetWidth()/(float)r.GetHeight());
	// Задаем её камере
	cam.SetProjectionMatrix(Proj);

	Geometry::Mesh *first=r.CreateMeshFromFile(L"first.WOX",L"t_arctic.tga");
    Geometry::Mesh *second=r.CreateMeshFromFile(L"second.WOX",L"ct_sas.tga");


			// Задаем не единичную матрицу второй модели
			POD_Math::Matrix4x4 WorldM={1,0,0,0, 
									    0,1,0,0,
									    0,0,1,0,
									    100.0f,0,0,1};

			second->SetTM(WorldM);

	while( msg.message != WM_QUIT )
	{
		if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
		{
			// Временная мера. Недостает ещё одного класса-обертки для контроля за анимацией.
			// Пока расчитываем на 100 кадров для всех загружаемых моделей
			g_dCurTime     = clock();
			g_fElpasedTime = g_dCurTime - g_dLastTime;

			if(g_fElpasedTime > 100*33.3f)
			g_dLastTime    = g_dCurTime;

			r.BeginScene();
			r.RenderMesh(*first,Proj,0,first->GetSkeleton()); 
			r.RenderMesh(*second,Proj,0,second->GetSkeleton()); 
			r.EndScene();
		}
	}

	first->Destroy();
	second->Destroy();

	delete first;
	delete second;

	r.Destroy();

	return 0;
};