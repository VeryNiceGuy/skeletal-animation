#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H

#include<windows.h>
#include<d3d9.h>

// ���� ����� ����� ������� �� ��������.
// ����� ���� ��������� ��������, �� ��� ������� � ��� �� �����.
// ������ ������ ���� ����� �� ���������� MSDN =) 

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