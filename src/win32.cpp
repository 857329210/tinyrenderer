#include <cassert>
#include <cstdio>
#include <windows.h>
#include "../include/win32.h"

window_t* window = NULL;

#ifdef UNICODE
static const wchar_t* WINDOW_CLASS_NAME = L"Class";
static const wchar_t* WINDOW_ENTRY_NAME = L"Entry";
#else
static const char* WINDOW_CLASS_NAME = "Class";
static const char* WINDOW_ENTRY_NAME = "Entry";
#endif // UNICODE


static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		window->is_close = 1;
		break;
	case WM_KEYDOWN:
		window->keys[wParam & 511] = 1;
		break;
	case WM_KEYUP:
		window->keys[wParam & 511] = 0;
		break;
	case WM_LBUTTONDOWN:
		window->mouse_info.orbit_pos = get_mouse_pos();
		window->buttons[0] = 1; break;
	case WM_LBUTTONUP:
		window->buttons[0] = 0;
		break;
	case WM_RBUTTONDOWN:
		window->mouse_info.fv_pos = get_mouse_pos();
		window->buttons[1] = 1;
		break;
	case WM_RBUTTONUP:
		window->buttons[1] = 0;
		break;
	case WM_MOUSEWHEEL:
		window->mouse_info.wheel_delta = GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
		break;

	default: return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

static void register_class()
{
	static bool initialized = false;
	if (!initialized)
	{
		ATOM class_ATOM;
		WNDCLASS wc;
		wc.style = CS_HREDRAW | CS_VREDRAW;						//���ڷ��
		wc.lpfnWndProc = WindowProc;							//*����* �ص�����
		wc.cbClsExtra = 0;										//�����ڴ�����β����һ�����ռ䣬��������Ϊ0
		wc.cbWndExtra = 0;										//�����ڴ���ʵ��β����һ�����ռ䣬��������Ϊ0
		wc.hInstance = GetModuleHandle(NULL);					//*����* ��ǰʵ�����
		wc.hIcon = NULL;										//ͼ��
		wc.hCursor = NULL;										//���
		wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);	//������ʽ
		wc.lpszMenuName = NULL;									//�˵���
		wc.lpszClassName = WINDOW_CLASS_NAME;					//�ô����������
		class_ATOM = RegisterClass(&wc);
		assert(class_ATOM != 0);
		initialized = true;
	}
}

static void init_bm_header(BITMAPINFOHEADER& bi, int width, int height)
{
	memset(&bi, 0, sizeof(BITMAPINFOHEADER));
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = width;
	bi.biHeight = -height;   //���ϵ���
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = width * height * 4;
}

int window_init(int width, int height, const char* title)
{
	window = (window_t*)malloc(sizeof(window_t));
	memset(window, 0, sizeof(window_t));
	window->is_close = 0;

	RECT rect = { 0,0,width,height };
	int wx, wy, sx, sy;
	LPVOID ptr;
	HDC hDC;
	BITMAPINFOHEADER bi;

	//ע�ᴰ����
	register_class();

	//��������
	window->h_window = CreateWindowEx(
		0,
		WINDOW_CLASS_NAME,
		L"SRender_window", 
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, GetModuleHandle(NULL), NULL
	);
	assert(window->h_window != NULL);

	//��ʼ��λͼͷ��ʽ bitmap header
	init_bm_header(bi, width, height);

	//��ü�����DC
	hDC = GetDC(window->h_window);
	window->mem_dc = CreateCompatibleDC(hDC);
	ReleaseDC(window->h_window, hDC);

	//����λͼ
	window->bm_dib = CreateDIBSection(window->mem_dc, (BITMAPINFO*)&bi, DIB_RGB_COLORS, &ptr, 0, 0); //�����豸�޹ؾ��
	assert(window->bm_dib != NULL);

	window->bm_old = (HBITMAP)SelectObject(window->mem_dc, window->bm_dib);//���´�����λͼ���д��mem_dc
	window->window_fb = (unsigned char*)ptr;

	window->width = width;
	window->height = height;

	AdjustWindowRect(&rect, GetWindowLong(window->h_window, GWL_STYLE), 0);//�������ڴ�С
	wx = rect.right - rect.left;
	wy = rect.bottom - rect.top;
	sx = (GetSystemMetrics(SM_CXSCREEN) - wx) / 2; // GetSystemMetrics(SM_CXSCREEN)��ȡ����Ļ�ķ�Ƭ��
	sy = (GetSystemMetrics(SM_CYSCREEN) - wy) / 2; // ���������λ��
	if (sy < 0) sy = 0;

	SetWindowPos(window->h_window, NULL, sx, sy, wx, wy, (SWP_NOCOPYBITS | SWP_NOZORDER | SWP_SHOWWINDOW));
	SetForegroundWindow(window->h_window);
	ShowWindow(window->h_window, SW_NORMAL);

	//��Ϣ����
	msg_dispatch();

	//��ʼ��keys, window_fbȫΪ0
	memset(window->window_fb, 0, width * height * 4);
	memset(window->keys, 0, sizeof(char) * 512);
	return 0;
}

int window_destroy()
{
	if (window->mem_dc)
	{
		if (window->bm_old)
		{
			SelectObject(window->mem_dc, window->bm_old); // д��ԭ����bitmap�������ͷ�DC��
			window->bm_old = NULL;
		}
		DeleteDC(window->mem_dc);
		window->mem_dc = NULL;
	}
	if (window->bm_dib)
	{
		DeleteObject(window->bm_dib);
		window->bm_dib = NULL;
	}
	if (window->h_window)
	{
		CloseWindow(window->h_window);
		window->h_window = NULL;
	}

	free(window);
	return 0;
}

void msg_dispatch()
{
	MSG msg;
	while (1)
	{
		// Peek��������Get��������PM_NOREMOVE��ʾ�������Ϣ��������������������Get������
		if (!PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) break; //û��Ϣ���ȷ������Ϣ����Get
		if (!GetMessage(&msg, NULL, 0, 0)) break;

		TranslateMessage(&msg);	 //ת����Ϣ ���ⰴ��->�ַ�
		DispatchMessage(&msg); //������Ϣ���ص�,��������WindowProc()
	}
}

static void window_display()
{
	LOGFONT logfont; //�ı��������
	ZeroMemory(&logfont, sizeof(LOGFONT));
	logfont.lfCharSet = ANSI_CHARSET;
	logfont.lfHeight = 20; //��������Ĵ�С
	HFONT hFont = CreateFontIndirect(&logfont);

	HDC hDC = GetDC(window->h_window);
	//Ŀ����е����Ͻ�(x,y), ���ȣ��߶ȣ�������ָ��
	SelectObject(window->mem_dc, hFont);
	SetTextColor(window->mem_dc, RGB(190, 190, 190));
	SetBkColor(window->mem_dc, RGB(80, 80, 80));
	//TextOut(window->mem_dc, 300, 50, "Project Name:SRender", strlen("Project Name:SRender"));
	//TextOut(window->mem_dc, 300, 80, "Author:Lei", strlen("Author:Lei Sun"));
	//TextOut(window->mem_dc, 20, 20,
	//	L"control:hold left buttion to rotate, right button to pan",
	//	strlen("Control:hold left buttion to rotate, right button to pan"));

	// �Ѽ�����DC�����ݴ���������DC��
	BitBlt(hDC, 0, 0, window->width, window->height, window->mem_dc, 0, 0, SRCCOPY);
	ReleaseDC(window->h_window, hDC);

}

void window_draw(unsigned char* framebuffer)
{
	int i, j;
	for (int i = 0; i < window->height; i++)
	{
		for (int j = 0; j < window->width; j++)
		{
			int index = (i * window->width + j) * 4;
			window->window_fb[index] = framebuffer[index + 2];
			window->window_fb[index + 1] = framebuffer[index + 1];
			window->window_fb[index + 2] = framebuffer[index];
		}
	}
	window_display();
}

Vec2f get_mouse_pos()
{
	POINT point;
	GetCursorPos(&point);
	ScreenToClient(window->h_window, &point); // ����Ļ�ռ�ת�����ڿռ�
	return Vec2f((float)point.x, (float)point.y);
}

/* misc platform functions */
static double get_native_time(void) {
	static double period = -1;
	LARGE_INTEGER counter;
	if (period < 0) {
		LARGE_INTEGER frequency;
		QueryPerformanceFrequency(&frequency);
		period = 1 / (double)frequency.QuadPart;
	}
	QueryPerformanceCounter(&counter);
	return counter.QuadPart * period;
}

float platform_get_time(void) {
	static double initial = -1;
	if (initial < 0) {
		initial = get_native_time();
	}
	return (float)(get_native_time() - initial);
}
