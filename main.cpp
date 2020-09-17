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
TODO		���ƂŒǉ��A�C������ׂ��@�\������B
HACK		���܂肫�ꂢ����Ȃ��R�[�h�B���t�@�N�^�����O���K�v�B
FIXME		���m�̕s�������R�[�h�B�C�����K�v�B
XXX			�댯�I�������ǂȂ����������킩��Ȃ��B
REVIEW		�Ӑ}�����ʂ�ɓ������A�������K�v������B
OPTIMIZE	���ʂ������A�{�g���l�b�N�ɂȂ��Ă���B
CHANGED		�R�[�h���ǂ̂悤�ɕύX�������B
NOTE		�Ȃ��A�����Ȃ����Ƃ��������c���B
WARNING		���ӂ��K�v�B
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
	// �E�C���h�E�T�C�Y
	UINT window_width = 500;
	UINT window_height = 500;
	// �Ԃ�l�m�F�p//�ʏ�S_OK�΂���̃n�Y
	HRESULT result;

	IDXGISwapChain4* _swapchain = nullptr;

	// �A�_�v�^�[�̑I��
	//TODO: �֐���
	IDXGIAdapter* pAdapter = nullptr;
	IDXGIAdapter* tmpAdapter;
	IDXGIFactory6* _dxgiFactory = nullptr;
	std::vector <IDXGIAdapter*> adapters;//�擾�ł����A�_�v�^�[���i�[
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
		// �擾���邾���F�I����ʂƂ��ʓr�K�v
		// �I�т����A�_�v�^����strDesc �ɂ�������
		// pAdapter = adpt;
	}

	// ������t�B�[�`���[���x����T��
	//TODO: �֐���
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
		//TODO: _dev=null ��n���Ǝ����Ŏ擾����邪�œK�Ƃ͌���Ȃ��̂ŃA�_�v�^�I����ʂ���ݒ肳���邱��
		if (D3D12CreateDevice(pAdapter, level, IID_PPV_ARGS(&_dev)) == S_OK)// 
		{
			d3dFeatureLevel = level;
			break;
		}
	}		

	// �R�}���h���X�g�쐬
	ID3D12CommandAllocator* _cmdAllocator = nullptr;
	ID3D12GraphicsCommandList* _cmdList = nullptr;

	result = _dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_cmdAllocator));
	result = _dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _cmdAllocator, nullptr, IID_PPV_ARGS(&_cmdList));


	// �R�}���h�L���[�쐬
	//TASK: �֐���
	ID3D12CommandQueue* _cmdQueue = nullptr;

	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc;
	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;//?
	cmdQueueDesc.NodeMask = 0;
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	result = _dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&_cmdQueue));

	// �E�C���h�E�̍쐬
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

	// �X���b�v�`�F�[���̍쐬
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


	// �f�B�X�N���v�^�q�[�v(�����_�[�^�[�Q�b�g�r���[)�쐬
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;// �����_�[�^�[�Q�b�g�r���[
	heapDesc.NodeMask = 0;	//TODO: GPU ��2�ȏ゠��ꍇ�̎��ʃr�b�g�t���O�FGPU��I�ԏꍇ0�Œ�͑ʖ�
	heapDesc.NumDescriptors = 2;//�\���Q��
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;//?

	ID3D12DescriptorHeap* rtvHeaps = nullptr;
	result = _dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&rtvHeaps));

	// �X���b�v�`�F�[���̃������ƕR�Â���
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

	// �X���b�v�`�F�[������
	result = _cmdAllocator->Reset();
	UINT bbidx = _swapchain->GetCurrentBackBufferIndex();// swapchainDesc.BufferCount=2 �Ȃ̂� index�Ƃ��� 0or1���Ԃ�

	D3D12_CPU_DESCRIPTOR_HANDLE rtvH = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
	rtvH.ptr += bbidx * _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	_cmdList->OMSetRenderTargets(1, &rtvH, true, nullptr);

	// Render�^�[�Q�b�g�̃N���A
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

