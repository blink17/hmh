#include <windows.h>
#include <stdint.h>

#define internal static
#define local_persist static
#define global_variable static

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

struct win32_offscreen_buffer
{
    BITMAPINFO Info;
    void* Memory;
    int32 Width;
    int32 Height;
    int32 Pitch;
};

struct win32_window_dimension
{
    int32 Width;
    int32 Height;
};

global_variable bool g_running = false;
global_variable win32_offscreen_buffer g_backbuffer;

win32_window_dimension Win32GetWindowDimension(HWND _Window)
{
    win32_window_dimension result;

    RECT clientRect;
    GetClientRect(_Window, &clientRect);
    result.Width = clientRect.right - clientRect.left;
    result.Height = clientRect.bottom - clientRect.top;

    return result;
}

internal void RenderWeirdGradient(win32_offscreen_buffer* _Buffer, int32 _BlueOffset, int32 _GreenOffset)
{
    /*
        Pixel (32-bits)

        Memory Order:   BB GG RR xx
        Loaded In:      xx BB GG RR
        Wanted:         AA RR GG BB
        Memory Order:   BB GG RR AA
    */

    uint8* row = (uint8*)_Buffer->Memory;
    for (int32 y = 0; y < _Buffer->Height; ++y)
    {
        uint32* pixel = (uint32*)row;
        for (int32 x = 0; x < _Buffer->Width; ++x)
        {
            uint8 blue = (x + _BlueOffset);
            uint8 green = (y + _GreenOffset);

            *pixel = (green << 8) | blue;
            ++pixel;
        }

        row += _Buffer->Pitch;
    }
}

internal void Win32ResizeDIBSection(win32_offscreen_buffer* _Buffer, int32 _Width, int32 _Height)
{
    if (_Buffer->Memory)
    {
        VirtualFree(_Buffer->Memory, 0, MEM_RELEASE);
    }

    _Buffer->Width = _Width;
    _Buffer->Height = _Height;
    int32 bytesPerPixel = 4;

    _Buffer->Info.bmiHeader.biSize = sizeof(_Buffer->Info.bmiHeader);
    _Buffer->Info.bmiHeader.biWidth = _Buffer->Width;
    _Buffer->Info.bmiHeader.biHeight = -_Buffer->Height;
    _Buffer->Info.bmiHeader.biPlanes = 1;
    _Buffer->Info.bmiHeader.biBitCount = 32;
    _Buffer->Info.bmiHeader.biCompression = BI_RGB;
    /*_Buffer->Info.bmiHeader.biSizeImage = 0;
    _Buffer->Info.bmiHeader.biXPelsPerMeter = 0;
    _Buffer->Info.bmiHeader.biYPelsPerMeter = 0;
    _Buffer->Info.bmiHeader.biClrUsed = 0;
    _Buffer->Info.bmiHeader.biClrImportant = 0;*/

    int bitmapMemorySize = (_Buffer->Width * _Buffer->Height) * bytesPerPixel;
    _Buffer->Memory = VirtualAlloc(0, bitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
    _Buffer->Pitch = _Buffer->Width * bytesPerPixel;
}

internal void Win32DisplayBufferInWindow(HDC _DeviceContext, 
                                         int32 _WindowWidth, int32 _WindowHeight,
                                         win32_offscreen_buffer* _Buffer)
{
    StretchDIBits(_DeviceContext, 
                  /*
                  x, y, width, height,
                  x, y, width, height,
                  */
                  0, 0, _WindowWidth, _WindowHeight,
                  0, 0, _Buffer->Width, _Buffer->Height,
                  _Buffer->Memory,
                  &_Buffer->Info,
                  DIB_RGB_COLORS, SRCCOPY);
}

LRESULT CALLBACK MainWindowCallback(_In_ HWND   _HWnd,
                                    _In_ UINT   _UMsg,
                                    _In_ WPARAM _WParam,
                                    _In_ LPARAM _LParam)
{
    LRESULT result = 0;
    switch (_UMsg)
    {        
        case WM_CLOSE:
        {
            // TODO(jungyoun.la): Handle this with a message to the user?
            g_running = false;
        } break;

        case WM_DESTROY:
        {
            // TODO(jungyoun.la): Handle this as an error - recreate window?
            g_running = false;
        } break;
        
        case WM_ACTIVATEAPP:
        {
            OutputDebugStringA("WM_ACTIVATEAPP\n");
        } break;   

        case WM_PAINT:
        {
            PAINTSTRUCT paint;
            HDC deviceContext = BeginPaint(_HWnd, &paint);
            win32_window_dimension dimension = Win32GetWindowDimension(_HWnd);

            Win32DisplayBufferInWindow(deviceContext, dimension.Width, dimension.Height, &g_backbuffer);
            EndPaint(_HWnd, &paint);
        } break;

        default:
        {
            //OutputDebugStringA("default\n");
            result = DefWindowProc(_HWnd, _UMsg, _WParam, _LParam);
        } break;
    }
    return result;
}


int CALLBACK WinMain(_In_ HINSTANCE _HInstance,
                     _In_ HINSTANCE _HPrevInstance,
                     _In_ LPSTR     _LpCmdLine,
                     _In_ int       _NCmdShow)
{
	WNDCLASS windowClass = {};

    Win32ResizeDIBSection(&g_backbuffer, 1280, 720);
	
	windowClass.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
	windowClass.lpfnWndProc = MainWindowCallback;
	windowClass.hInstance = _HInstance;
	//WindowClass.hIcon;
	//WindowClass.lpszMenuName;
	windowClass.lpszClassName = "HandmadeHeroWindowClass";

    if (!RegisterClass(&windowClass))
    {
        // TODO(jungyoun.la): Logging
        return 0;
    }

    HWND windowHandle = CreateWindowExA(
        0,
        windowClass.lpszClassName,
        "Handmade Hero",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        0,
        0,
        _HInstance,
        0);

    if (!windowHandle)
    {
        // TODO(jungyoun.la): Logging
        return 0;
    }

    HDC deviceContext = GetDC(windowHandle);

    int32 blueOffset = 0;
    int32 greenOffset = 0;
    
    g_running = true;
    while (g_running)
    {
        MSG message;
        while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
        {
            if (message.message == WM_QUIT)
            {
                g_running = false;
            }

            TranslateMessage(&message);
            DispatchMessageA(&message);
        }

        RenderWeirdGradient(&g_backbuffer, blueOffset, greenOffset);
        
        win32_window_dimension dimension = Win32GetWindowDimension(windowHandle);
        Win32DisplayBufferInWindow(deviceContext, dimension.Width, dimension.Height, &g_backbuffer);

        ++blueOffset;
        greenOffset += 2;
    }

	return 0;
}
