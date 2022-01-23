#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H

#include<windows.h>
#include<d3d9.h>

// Этот класс будет следить за порядком.
// Можно было построить синглтон, но мне кажется и так не плохо.
// Тексты ошибок были взяты из дескрипции MSDN =) 

class ErrorHandler
{

	public:

		ErrorHandler();
		~ErrorHandler();

	static void CheckD3D(const HRESULT);
	static void ThreadLastError();
	static void SetHWND(const HWND); 
	static void ShowWarning(const wchar_t*);
	static HWND WindowHandle;
};

#endif