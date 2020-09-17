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

/*
TODO		あとで追加、修正するべき機能がある。
HACK		あまりきれいじゃないコード。リファクタリングが必要。
FIXME		既知の不具合があるコード。修正が必要。
XXX			危険！動くけどなぜうごくかわからない。
REVIEW		意図した通りに動くか、見直す必要がある。
OPTIMIZE	無駄が多く、ボトルネックになっている。
CHANGED		コードをどのように変更したか。
NOTE		なぜ、こうなったという情報を残す。
WARNING		注意が必要。
*/

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
	// ウインドウサイズ
	UINT window_width = 500;
	UINT window_height = 500;
	// 返り値確認用//通常S_OKばかりのハズ
	HRESULT result;

	IDXGISwapChain4* _swapchain = nullptr;

	// アダプターの選択
	//TODO: 関数化
	IDXGIAdapter* pAdapter = nullptr;
	IDXGIAdapter* tmpAdapter;
	IDXGIFactory6* _dxgiFactory = nullptr;
	std::vector <IDXGIAdapter*> adapters;//取得できたアダプターを格納
	result = CreateDXGIFactory1(IID_PPV_ARGS(&_dxgiFactory));
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
		// 選びたいアダプタ名がstrDesc にあったら
		// pAdapter = adpt;
	}

	// つかえるフィーチャーレベルを探す
	//TODO: 関数化
	ID3D12Device* _dev = nullptr;
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
		//TODO: _dev=null を渡すと自動で取得されるが最適とは限らないのでアダプタ選択画面から設定させること
		if (D3D12CreateDevice(pAdapter, level, IID_PPV_ARGS(&_dev)) == S_OK)// 
		{
			d3dFeatureLevel = level;
			break;
		}
	}		

	// コマンドリスト作成
	ID3D12CommandAllocator* _cmdAllocator = nullptr;
	ID3D12GraphicsCommandList* _cmdList = nullptr;

	result = _dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_cmdAllocator));
	result = _dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _cmdAllocator, nullptr, IID_PPV_ARGS(&_cmdList));


	// コマンドキュー作成
	//TASK: 関数化
	ID3D12CommandQueue* _cmdQueue = nullptr;

	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc;
	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;//?
	cmdQueueDesc.NodeMask = 0;
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	result = _dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&_cmdQueue));

	// ウインドウの作成
	WNDCLASSEX w = {};
	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProcedure;
	w.lpszClassName = _T("hogehogehogehogehoge");
	w.hInstance = GetModuleHandle(nullptr);

	RegisterClassEx(&w);

	RECT wrc = { 0,0, (LONG)window_width, (LONG)window_height };

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

	// スワップチェーンの作成
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};
	swapchainDesc.Width = window_width;
	swapchainDesc.Height = window_height;
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//?
	swapchainDesc.Stereo = false;
	swapchainDesc.SampleDesc.Count = 1;//?
	swapchainDesc.SampleDesc.Quality = 0;//?
	swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;//?
	swapchainDesc.BufferCount = 2;
	swapchainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapchainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	result = _dxgiFactory->CreateSwapChainForHwnd(_cmdQueue, hwnd, &swapchainDesc, nullptr, nullptr, (IDXGISwapChain1**)(&_swapchain));


	// ディスクリプタヒープ(レンダーターゲットビュー)作成
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;// レンダーターゲットビュー
	heapDesc.NodeMask = 0;	//TODO: GPU が2つ以上ある場合の識別ビットフラグ：GPUを選ぶ場合0固定は駄目
	heapDesc.NumDescriptors = 2;//表裏２つ
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;//?

	ID3D12DescriptorHeap* rtvHeaps = nullptr;
	result = _dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&rtvHeaps));

	// スワップチェーンのメモリと紐づける
	DXGI_SWAP_CHAIN_DESC swcDesc = {};
	result = _swapchain->GetDesc(&swcDesc);

	std::vector<ID3D12Resource*> _backBuffers(swcDesc.BufferCount);
	for (UINT i = 0; i < swcDesc.BufferCount; i++)
	{
		result = _swapchain->GetBuffer(i, IID_PPV_ARGS(&_backBuffers[i]));
		D3D12_CPU_DESCRIPTOR_HANDLE h = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
		_dev->CreateRenderTargetView(_backBuffers[i], nullptr, h);

		if (i +1 == swcDesc.BufferCount)
			break;
		h.ptr += _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	// スワップチェーン動作
	result = _cmdAllocator->Reset();
	UINT bbidx = _swapchain->GetCurrentBackBufferIndex();// swapchainDesc.BufferCount=2 なので indexとして 0or1が返る

	D3D12_CPU_DESCRIPTOR_HANDLE rtvH = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
	rtvH.ptr += bbidx * _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	_cmdList->OMSetRenderTargets(1, &rtvH, true, nullptr);

	// Renderターゲットのクリア
	float clearColor[] = { 1.0f,1.0f,0.0,1.0f };
	_cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);

	_cmdList->Close();

	ID3D12CommandList* cmdlists[] = { _cmdList };
	_cmdQueue->ExecuteCommandLists(1, cmdlists);

	_cmdAllocator->Reset();
	_cmdList->Reset(_cmdAllocator, nullptr);

	_swapchain->Present(1, 0);


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

