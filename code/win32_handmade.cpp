#include <windows.h>

#define internal static
#define local_persist static
#define global_variable static

// TODO(jungyoun.la): This is global for now.
global_variable bool g_running = false;
global_variable BITMAPINFO g_bitmapInfo;
global_variable void *g_bitmapMemory;
global_variable HBITMAP g_bitmapHandle;
global_variable HDC g_bitmapDeviceContext;

internal void Win32ResizeDIBSection(int width, int height)
{
    if (g_bitmapHandle)
    {
        DeleteObject(g_bitmapHandle);
    }

    if (g_bitmapDeviceContext != 0)
    {
        g_bitmapDeviceContext = CreateCompatibleDC(0);
    }

    g_bitmapInfo.bmiHeader.biSize = sizeof(g_bitmapInfo.bmiHeader);
    g_bitmapInfo.bmiHeader.biWidth = width;
    g_bitmapInfo.bmiHeader.biHeight = height;
    g_bitmapInfo.bmiHeader.biPlanes = 1;
    g_bitmapInfo.bmiHeader.biBitCount = 32;
    g_bitmapInfo.bmiHeader.biCompression = BI_RGB;
    /*g_bitmapInfo.bmiHeader.biSizeImage = 0;
    g_bitmapInfo.bmiHeader.biXPelsPerMeter = 0;
    g_bitmapInfo.bmiHeader.biYPelsPerMeter = 0;
    g_bitmapInfo.bmiHeader.biClrUsed = 0;
    g_bitmapInfo.bmiHeader.biClrImportant = 0;*/

    g_bitmapHandle = CreateDIBSection(g_bitmapDeviceContext,
                                      &g_bitmapInfo,
                                      DIB_RGB_COLORS,
                                      &g_bitmapMemory,
                                      0, 0);
}

internal void Win32UpdateWindow(HDC deviceContext, int x, int y, int width, int height)
{
    StretchDIBits(deviceContext, 
                  x, y, width, height,
                  x, y, width, height,
                  g_bitmapMemory,
                  &g_bitmapInfo,
                  DIB_RGB_COLORS, SRCCOPY);
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
            Win32ResizeDIBSection(width, height);
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
            Win32UpdateWindow(deviceContext, x, y, width, height);
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
