#include <windows.h>
#include <stdint.h>
#include <Xinput.h>

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

// TODO(jungyoun.la): This is a global for now
global_variable bool GlobalRunning = false;
global_variable win32_offscreen_buffer GlobalBackbuffer;

//~ Begin XInput - 왜 이렇게 추상계층을 뒀을까?
// NOTE(jungyoun.la): XInputGetState
#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE* pState)
typedef X_INPUT_GET_STATE(x_input_get_state);
X_INPUT_GET_STATE(XInputGetStateStub)
{
    return 0;
}
global_variable x_input_get_state* XInputGetState_ = XInputGetStateStub;
#define XInputGetState XInputGetState_

// NOTE(jungyoun.la): XInputSetState
#define X_INPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration)
typedef X_INPUT_SET_STATE(x_input_set_state);
X_INPUT_SET_STATE(XInputSetStateStub)
{
    return 0;
}
global_variable x_input_set_state* XInputSetState_ = XInputSetStateStub;
#define XInputSetState XInputSetState_
//~ End XInput

internal void Win32LoadXInput(void)
{
    HMODULE xInputLibrary = LoadLibraryA("XInput1_4.dll");
    if (xInputLibrary)
    {
        XInputGetState_ = (x_input_get_state*)GetProcAddress(xInputLibrary, "XInputGetState");
        if (!XInputGetState_) { XInputGetState_ = XInputGetStateStub; }

        XInputSetState_ = (x_input_set_state*)GetProcAddress(xInputLibrary, "XInputSetState");
        if (!XInputSetState_) { XInputSetState_ = XInputSetStateStub; }
    }
}

internal win32_window_dimension Win32GetWindowDimension(HWND _Window)
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

internal void Win32DisplayBufferInWindow(win32_offscreen_buffer* _Buffer,
                                         HDC _DeviceContext, int32 _WindowWidth, int32 _WindowHeight)
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
            GlobalRunning = false;
        } break;

        case WM_DESTROY:
        {
            // TODO(jungyoun.la): Handle this as an error - recreate window?
            GlobalRunning = false;
        } break;

        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP:
        {
            uint32 vkCode = _WParam;
            bool wasDown = ((_LParam & (1 << 30)) != 0);
            bool isDown = ((_LParam & (1 << 31)) == 0);
            if (wasDown != isDown)
            {
                if (vkCode == 'W')
                {
                }
                else if (vkCode == 'A')
                {
                }
                else if (vkCode == 'S')
                {
                }
                else if (vkCode == 'D')
                {
                }
                else if (vkCode == 'Q')
                {
                }
                else if (vkCode == 'E')
                {
                }
                else if (vkCode == VK_UP)
                {
                }
                else if (vkCode == VK_LEFT)
                {
                }
                else if (vkCode == VK_DOWN)
                {
                }
                else if (vkCode == VK_RIGHT)
                {
                }
                else if (vkCode == VK_ESCAPE)
                {
                    OutputDebugStringA("ESCAPE: ");
                    if (isDown)
                    {
                        OutputDebugStringA("IsDown ");
                    }
                    if (wasDown)
                    {
                        OutputDebugStringA("WasDown");
                    }
                    OutputDebugStringA("\n");
                }
                else if (vkCode == VK_SPACE)
                {
                }
            }
        } break;
        
        case WM_ACTIVATEAPP:
        {
            OutputDebugStringA("WM_ACTIVATEAPP\n");
        } break;   

        case WM_PAINT:
        {
            PAINTSTRUCT paint;
            HDC deviceContext = BeginPaint(_HWnd, &paint);
            {
                // 아래 두 코드가 없어도 제대로 실행된다. 매 프레임 그리기 때문에
                // 그러나 BeginPaint(), EndPaint()까지 모든 라인을 실행하지 않으면 화면에 그려지는것이 아무것도 없고 먹통이 되어버린다 왜일까??
                win32_window_dimension dimension = Win32GetWindowDimension(_HWnd);
                Win32DisplayBufferInWindow(&GlobalBackbuffer, deviceContext, dimension.Width, dimension.Height);
            }
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
    Win32LoadXInput();

    WNDCLASS windowClass = {};

    Win32ResizeDIBSection(&GlobalBackbuffer, 1280, 720);
	
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
    
    GlobalRunning = true;
    while (GlobalRunning)
    {
        MSG message;
        while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
        {
            if (message.message == WM_QUIT)
            {
                GlobalRunning = false;
            }

            TranslateMessage(&message);
            DispatchMessageA(&message);
        }

        // TODO(jungyoun.la): Should we poll this more frequently.
        for (DWORD controllerIndex = 0; controllerIndex < XUSER_MAX_COUNT; controllerIndex++)
        {
            XINPUT_STATE controllerState;
            ZeroMemory(&controllerState, sizeof(XINPUT_STATE));
            if (XInputGetState(controllerIndex, &controllerState) == ERROR_SUCCESS)
            {
                // Controller is connected 
                XINPUT_GAMEPAD* pad = &controllerState.Gamepad;

                bool up = (pad->wButtons & XINPUT_GAMEPAD_DPAD_UP);
                bool down = (pad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
                bool left = (pad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
                bool right = (pad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
                bool start = (pad->wButtons & XINPUT_GAMEPAD_START);
                bool back = (pad->wButtons & XINPUT_GAMEPAD_BACK);
                bool leftShoulder = (pad->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
                bool rightShoulder = (pad->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
                bool aButton = (pad->wButtons & XINPUT_GAMEPAD_A);
                bool bButton = (pad->wButtons & XINPUT_GAMEPAD_B);
                bool xButton = (pad->wButtons & XINPUT_GAMEPAD_X);
                bool yButton = (pad->wButtons & XINPUT_GAMEPAD_Y);

                int16 stickX = pad->sThumbLX;
                int16 stickY = pad->sThumbLY;

                blueOffset += stickX >> 12;
                greenOffset -= stickY >> 12;                
            }
            else
            {
                // Controller is not connected 
            }
        }

        RenderWeirdGradient(&GlobalBackbuffer, blueOffset, greenOffset);
        
        win32_window_dimension dimension = Win32GetWindowDimension(windowHandle);
        Win32DisplayBufferInWindow(&GlobalBackbuffer, deviceContext, dimension.Width, dimension.Height);

        /*++blueOffset;
        greenOffset += 2;*/
    }

	return 0;
}
