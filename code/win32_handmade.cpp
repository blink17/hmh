#include <windows.h>

int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow)
{
	WNDCLASS WindowClass = {};
	
	WindowClass.style;
	WindowClass.lpfnWndProc;
	WindowClass.hInstance = hInstance;
	//WindowClass.hIcon;
	//WindowClass.lpszMenuName;
	WindowClass.lpszClassName = "HandmadeHeroWindowClass";

	return(0);
}
