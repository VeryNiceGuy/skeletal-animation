#include "ErrorHandler.h"
#include <strsafe.h>

	HWND ErrorHandler::WindowHandle;

	ErrorHandler::ErrorHandler()
	{
		ShowWarning(L"ErrorHandler is Instantiated!");
	};

	ErrorHandler::~ErrorHandler(){};

	void ErrorHandler::ThreadLastError()
	{
		
		void* lpMsgBuf            =  0;
		wchar_t lpDisplayBuf[128] = {0};

		LPTSTR*lpszFunction=0;  

		DWORD dw = GetLastError(); 

		FormatMessage(  FORMAT_MESSAGE_ALLOCATE_BUFFER | 
						FORMAT_MESSAGE_FROM_SYSTEM     |
						FORMAT_MESSAGE_IGNORE_INSERTS,
						NULL,
						dw,
						MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
						(wchar_t*) &lpMsgBuf,
						0,
						NULL );


		StringCchPrintf((wchar_t*)lpDisplayBuf, 
						sizeof(lpDisplayBuf) / sizeof(wchar_t),
						L"%s failed with error %d: %s", 
						lpszFunction,
						dw,
						lpMsgBuf); 

		ShowWarning(lpDisplayBuf);

	};

	void ErrorHandler::CheckD3D(const HRESULT value)
	{
		if(!WindowHandle) { ShowWarning(L"HWND isn't set"); return;};

		switch(value)
		{
			case D3DERR_DEVICELOST:
								ShowWarning(L"The device has been lost but cannot be reset at this time.        \
											Therefore, rendering is not possible.A Direct 3D device object    \
											other than the one that returned this code caused the hardware    \
											adapter to be reset by the OS. Delete all video memory objects    \
											(surfaces, textures, state blocks) and call Reset() to return the \
											device to a default state. If the application continues rendering \
											without a reset, the rendering calls will succeed.");
				break;

			case D3DERR_DEVICENOTRESET:
								ShowWarning(L"The device has been lost but can be reset at this time.");
				break;

			case D3DERR_CONFLICTINGRENDERSTATE:
								ShowWarning(L"The currently set render states cannot be used together.");
				break;

			case D3DERR_INVALIDCALL:
								ShowWarning(L"The method call is invalid. For example, a method's parameter may not be a valid pointer.");
				break;

			case E_OUTOFMEMORY:
								ShowWarning(L"Direct3D could not allocate sufficient memory to complete the call.");
				break;

			case E_INVALIDARG:
								ShowWarning(L"An invalid parameter was passed to the returning function.");
				break;

			case D3DERR_OUTOFVIDEOMEMORY:
								ShowWarning(L"Direct3D does not have enough display memory to perform the      \
											operation. The device is using more resources in a single scene    \
											than can fit simultaneously into video memory. Present, PresentEx, \
											or CheckDeviceState can return this error. Recovery is similar to  \
											D3DERR_DEVICEHUNG, though the application may want to reduce its   \
											per-frame memory usage as well to avoid having the error recur.");
				break;

			case D3DERR_NOTAVAILABLE:
								ShowWarning(L"This device does not support the queried technique.");
				break;
		};
	};

	void ErrorHandler::SetHWND(const HWND hWnd)
	{
		WindowHandle=hWnd;	
	}; 

	void ErrorHandler::ShowWarning(const wchar_t*text)
	{
		MessageBox(WindowHandle,
					text,
					L"Internal Warning",
					MB_OK);
	};