#include <Windows.h>
#include <vector>
#include <iostream>
#include <tchar.h>
#include <d3d12.h>
#include <dxgi1_6.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")


#ifdef _DEBUG 
#endif // _DEBUG 

using namespace std;

void DebugOutputFormatString(const char* format, ...)
{
#ifdef _DEBUG
	va_list valist;
	va_start(valist, format);
	  vprintf(format, valist);
	va_end(valist);
#endif // _DEBUG
	
}

LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (msg == WM_DESTROY)
	{
		PostQuitMessage(0);
		return 0;
}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

#ifdef _DEBUG
int main()
{
#else
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
#endif

	ID3D12Device* _dev = nullptr;
	IDXGIFactory6* _dxgiFactory = nullptr;
	IDXGISwapChain4* _swapchain = nullptr;

	HRESULT D3D12CreateDevice(IUnknown * pAdapter,
		D3D_FEATURE_LEVEL MinimumfeatureLevel,
		REFIID riid,
		void** ppDevice);


	// アダプターの選択
	std::vector <IDXGIAdapter*> adapters;//取得できたアダプターを格納
	IDXGIAdapter* tmpAdapter;
	auto result = CreateDXGIFactory1(IID_PPV_ARGS(&_dxgiFactory));
	for (int i = 0; _dxgiFactory->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND; i++)
	{
		adapters.push_back(tmpAdapter);
	}
	for (auto adpt : adapters)
	{
		DXGI_ADAPTER_DESC adesc = {};
		adpt->GetDesc(&adesc);

		std::wstring strDesc = adesc.Description;
		// 取得するだけ：選択画面とか別途必要
	}

	// つかえるフィーチャーレベルを探す
	D3D_FEATURE_LEVEL d3dFeatureLevelLst[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};
	D3D_FEATURE_LEVEL d3dFeatureLevel;

	for (auto level : d3dFeatureLevelLst)
	{
		if (D3D12CreateDevice(nullptr, level, IID_PPV_ARGS(&_dev)) == S_OK)// _dev=null を渡すと自動で取得されるが最適とは限らないらしい
		{
			d3dFeatureLevel = level;
			break;
		}
	}

	// ウインドウの作成
	WNDCLASSEX w = {};
	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProcedure;
	w.lpszClassName = _T("hogehogehogehogehoge");
	w.hInstance = GetModuleHandle(nullptr);

	RegisterClassEx(&w);

	RECT wrc = { 0,0, 100, 200 };

	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	HWND hwnd = CreateWindow(w.lpszClassName,
		_T("titletest"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wrc.right - wrc.left,
		wrc.bottom - wrc.top,
		nullptr,
		nullptr,
		w.hInstance,
		nullptr
	);

	// show window
	ShowWindow(hwnd, SW_SHOW);

	MSG msg = {};
	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (msg.message == WM_QUIT)
		{
			break;
		}
	}

	UnregisterClassW(w.lpszClassName, w.hInstance);

	return 0;
}

