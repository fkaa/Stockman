#include "Engine.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		//DirectX::Keyboard::ProcessMessage(msg, wparam, lparam);
		break;

	case WM_ACTIVATEAPP:
	case WM_INPUT:
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_MOUSEHOVER:
		//DirectX::Mouse::ProcessMessage(msg, wparam, lparam);
		break;

	default:
		return DefWindowProc(hwnd, msg, wparam, lparam);
		break;
	}

	return 0;
}

Engine::Engine(HINSTANCE hInstance, int width, int height)
{
	this->height = height;
	this->width = width;
	this->hInstance = hInstance;
	this->initializeWindow();
}

Engine::~Engine()
{
	this->mDevice->Release();
	this->mContext->Release();
	this->mSwapChain->Release();
	this->mBackBufferRTV->Release();

	//Enable this to get additional information about live objects
	//this->mDebugDevice->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	this->mDebugDevice->Release();
}

void Engine::initializeWindow()
{
	WNDCLASS wc = { 0 };

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = this->hInstance;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszClassName = "Basic test";

	if (!RegisterClass(&wc))
	{
		MessageBox(this->window, "registerClass failed", "Error", MB_OK);
	}

	this->window = CreateWindow(
		"Basic test",
		"Stort spel",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		this->width,
		this->height,
		0,
		0,
		this->hInstance,
		0);

	if (this->window == 0)
	{
		MessageBox(this->window, "window creation failed", "Error", MB_OK);
	}



	ShowWindow(this->window, SW_SHOWDEFAULT);
	UpdateWindow(this->window);

}

HRESULT Engine::createSwapChain()
{
	
	DXGI_SWAP_CHAIN_DESC desc;
	ZeroMemory(&desc, sizeof(DXGI_SWAP_CHAIN_DESC));

	desc.BufferCount = 1;
	desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.OutputWindow = this->window;
	desc.SampleDesc.Count = 1;
	desc.Windowed = true;
	desc.BufferDesc.Height = this->height;
	desc.BufferDesc.Width = this->width;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		D3D11_CREATE_DEVICE_DEBUG,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&desc,
		&mSwapChain,
		&this->mDevice,
		NULL,
		&this->mContext);

	if (SUCCEEDED(hr))
	{
		ID3D11Texture2D* backBuffer = nullptr;
		hr = mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
		if (FAILED(hr))
		{
			MessageBox(0, "swap chain getBuffer failed", "error", MB_OK);
			return hr;
		}

		hr = mDevice->CreateRenderTargetView(backBuffer, NULL, &mBackBufferRTV);
		if (FAILED(hr))
		{
			MessageBox(0, "RTV creation failed", "error", MB_OK);
			return hr;
		}
		backBuffer->Release();


		//Creates a debug device to check for memory leaks etc
		HRESULT hr = this->mDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast <void **>(&mDebugDevice)); 
		if (FAILED(hr))
		{
			MessageBox(0, "debug device creation failed", "error", MB_OK);
		}

	}
	else
	{
		MessageBox(0, "remove debug flag", "error", MB_OK);
	}

	return hr;
}

int Engine::run()
{
	MSG msg = { 0 };

	this->createSwapChain();

	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);

			DispatchMessage(&msg);
		}
		else
		{
		//	system("pause");
		}
	}
	return 0;
}