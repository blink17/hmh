#include <windows.h>

#define internal static
#define local_persist static
#define global_variable static

// TODO(jungyoun.la): This is global for now.
global_variable bool g_running = false;

internal void ResizeDIBSection(int width, int height)
{

}

internal void Win32UpdateWindow(HWND hWnd, int x, int y, int width, int height)
{
    HBITMAP CreateDIBSection(
  _In_   HDC        hdc,
  _In_   const BITMAPINFO *pbmi,
  _In_   UINT       iUsage,
  _Out_  VOID       **ppvBits,
  _In_   HANDLE     hSection,
  _In_   DWORD      dwOffset
);
}

LRESULT CALLBACK MainWindowCallback(_In_ HWND   hWnd,
                                    _In_ UINT   uMsg,
                                    _In_ WPARAM wParam,
                                    _In_ LPARAM lParam)
{
    LRESULT result = 0;
    switch (uMsg)
    {
        case WM_SIZE:
        {
            RECT clientRect;
            GetClientRect(hWnd, &clientRect);
            int width = clientRect.right - clientRect.left;
            int height = clientRect.bottom - clientRect.top;
            ResizeDIBSection(width, height);
            OutputDebugStringA("WM_SIZE\n");
            break;  
        } 
        
        case WM_CLOSE:
        {
            // TODO(jungyoun.la): Handle this with a message to the user?
            g_running = false;
            break;  
        } 

        case WM_DESTROY:
        {
            // TODO(jungyoun.la): Handle this as an error - recreate window?
            g_running = false;
            break;   
        }
        
        case WM_ACTIVATEAPP:
        {
            OutputDebugStringA("WM_ACTIVATEAPP\n");
            break;   
        }

        case WM_PAINT:
        {
            PAINTSTRUCT paint;
            HDC deviceContext = BeginPaint(hWnd, &paint);
            int x = paint.rcPaint.left;
            int y = paint.rcPaint.top;
            int width = paint.rcPaint.right - paint.rcPaint.left;
            int height = paint.rcPaint.bottom - paint.rcPaint.top;
            Win32UpdateWindow(hWnd, x, y, width, height);
            EndPaint(hWnd, &paint);
            break;
        }

        default:
        {
            //OutputDebugStringA("default\n");
            result = DefWindowProc(hWnd, uMsg, wParam, lParam);
            break;
        }
    }
    return result;
}


int CALLBACK WinMain(_In_ HINSTANCE hInstance,
                     _In_ HINSTANCE hPrevInstance,
                     _In_ LPSTR     lpCmdLine,
                     _In_ int       nCmdShow)
{
	WNDCLASS windowClass = {};
	
	windowClass.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
	windowClass.lpfnWndProc = MainWindowCallback;
	windowClass.hInstance = hInstance;
	//WindowClass.hIcon;
	//WindowClass.lpszMenuName;
	windowClass.lpszClassName = "HandmadeHeroWindowClass";

    if (RegisterClass(&windowClass))
    {
        HWND windowHandle = CreateWindowExA(
            0,
            windowClass.lpszClassName,
            "Handmade Hero",
            WS_OVERLAPPEDWINDOW|WS_VISIBLE,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            0,
            0,
            hInstance,
            0);

        if (windowHandle)
        {
            g_running = true;
            MSG message;
            bool messageResult;
            while (g_running)
            {
                messageResult = GetMessageA(&message, 0, 0, 0);
                if (messageResult == true)
                {
                    TranslateMessage(&message);
                    DispatchMessageA(&message);
                }
                else
                {
                    break;
                }

            }
        }
        else
        {
            // TODO(jungyoun.la): Logging
        }
    }
    else
    {
        // TODO(jungyoun.la): Logging
    }

	return 0;
}
