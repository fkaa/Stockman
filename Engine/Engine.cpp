#include <Singletons/Profiler.h>
#include "Engine.h"
#include <Graphics\include\Structs.h>
#include <Graphics\include\Utility\DebugDraw.h>
#include <Graphics\include\Utility\Texture.h>

#include <Windows.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <iostream>
#include <Graphics\include\Structs.h>

#include "Engine.h"
#include "Typing.h"

#include <Singletons/DebugWindow.h>
#include <Graphics\include\Device.h>
#include <Graphics\include\RenderQueue.h>
#include <Graphics\include\MainCamera.h>
#include <Engine\Settings.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

#include "resource.h"

extern LRESULT ImGui_ImplDX11_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
bool settingsFullScreenOverRide = false;
HWND *Engine::g_window = nullptr;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	ImGui_ImplDX11_WndProcHandler(hwnd, msg, wparam, lparam);
	Typing* theChar = Typing::getInstance(); //might need to be deleted
	DebugWindow * debug = DebugWindow::getInstance();
	int key = MapVirtualKey((int)wparam, 0);

	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
		
	case WM_KEYUP:
        if (key == 41)
        {
            debug->toggleDebugToDraw();
            DirectX::Mouse::Get().SetMode(debug->isOpen() ? DirectX::Mouse::MODE_ABSOLUTE : DirectX::Mouse::MODE_RELATIVE);
        }

	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
        if (wparam == VK_RETURN)
            if ((HIWORD(lparam) & KF_ALTDOWN)) {
                settingsFullScreenOverRide = !settingsFullScreenOverRide;
                Settings& setting = Settings::getInstance();
                setting.setWindowed(settingsFullScreenOverRide);
            }
                
	case WM_SYSKEYUP:
		DirectX::Keyboard::ProcessMessage(msg, wparam, lparam);
		break;
    case WM_KILLFOCUS:
        if (Engine::g_window) // make sure window is created
        {
            DirectX::Mouse::Get().SetMode(DirectX::Mouse::MODE_ABSOLUTE);
        }
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
		DirectX::Mouse::ProcessMessage(msg, wparam, lparam);
		break;
	case WM_CHAR:
		theChar->setSymbol(char((wchar_t)wparam));
		break;
	default:
		return DefWindowProc(hwnd, msg, wparam, lparam);
		break;
	}

	return 0;
}

Engine::Engine(HINSTANCE hInstance, int width, int height, LPWSTR *cmdLine, int args)
{
	srand((unsigned int)time(NULL));				// Set random seed
	this->mHeight = height;
	this->mWidth = width;
	this->hInstance = hInstance;
	this->initializeWindow();

	this->mKeyboard = std::make_unique<DirectX::Keyboard>();
	this->mMouse = std::make_unique<DirectX::Mouse>();
    this->mTracker = std::make_unique<DirectX::Keyboard::KeyboardStateTracker>();
	this->mMouse->SetWindow(window);
    Engine::g_window = &window;

    DebugWindow * debug = DebugWindow::getInstance();

    Graphics::Debug::Initialize(mDevice);

    debug->registerCommand("GAME_SET_FULLSCREEN", [&](std::vector<std::string> &args)->std::string
    {
        std::string catcher = "";
        try
        {
            Settings& setting = Settings::getInstance();
            if (args.size() != 0)
            {
                setting.setWindowed(std::stoi(args[0]));

                if (setting.getWindowed())
                    catcher = "Fullscreen enabled!";

                else
                    catcher = "Fullscreen disabled!";
            }
            else
            {
                catcher = "missing argument 0 or 1.";
            }
        }
        catch (const std::exception&)
        {
            catcher = "Argument must be 0 or 1.";
        }

        return catcher;
    });

    debug->registerCommand("CHANGE_NAME", [&](std::vector<std::string> &args)->std::string
    {
        std::string catcher = "";
        try
        {
            Settings setting = Settings::getInstance();
            if (args.size() != 0)
            {
                setting.setName(args[0]);
                catcher = "Named have been changed!";
            }
            else
            {
                catcher = "Please add your new alias too.";
            }
        }
        catch (const std::exception&)
        {
            catcher = "Are you stupid?";
        }

        return catcher;
    });

    // load settings before starting
    Settings& setting = Settings::getInstance();
    setting.readFromFile();

//    game = new Logic::StateMachine(cmdLine, args);
    game = newd Logic::StateMachine();
}

Engine::~Engine()
{
    delete game;
    Typing::releaseInstance();
    mSwapChain->SetFullscreenState(false, NULL);

	ImGui_ImplDX11_Shutdown();
	DebugWindow::releaseInstance();
	delete this->renderer;

	this->mDevice->Release();
	this->mContext->Release();
	this->mSwapChain->Release();
	this->mBackBufferRTV->Release();

	//Enable this to get additional information about live objects
    //this->mDebugDevice->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
    SAFE_RELEASE(mDebugDevice);
}

void Engine::initializeWindow()
{
	WNDCLASS wc = { 0 };

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = this->hInstance;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    #ifdef _DEBUG
	wc.hCursor = LoadCursorFromFile("../Resources/Cursors/cursor.cur");
    #else
    wc.hCursor = LoadCursorFromFile("Resources/Cursors/cursor.cur");
    #endif
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszClassName = "Basic test";

	if (!RegisterClass(&wc))
	{
		MessageBox(this->window, "registerClass failed", "Error", MB_OK);
	}

	RECT rc = { 0, 0, mWidth, mHeight };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	this->window = CreateWindow(
		"Basic test",
		"Stockman",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rc.right - rc.left,
		rc.bottom - rc.top,
		0,
		0,
		this->hInstance,
		0);

	if (this->window == 0)
	{
		MessageBox(this->window, "window creation failed", "Error", MB_OK);
	}

    HICON icon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(101));
    SendMessage(window, WM_SETICON, ICON_BIG, (LPARAM)icon);

    SetWindowLong(window, GWL_STYLE, GetWindowLong(window, GWL_STYLE) & ~WS_SIZEBOX);

    #ifdef _DEBUG
	SetWindowPos(GetConsoleWindow(), 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	SetWindowPos(this->window, 0, 100, 150, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    #endif

	ShowWindow(this->window, SW_SHOWDEFAULT);
	UpdateWindow(this->window);
}

HRESULT Engine::createSwapChain()
{

	DXGI_SWAP_CHAIN_DESC desc;
	ZeroMemory(&desc, sizeof(DXGI_SWAP_CHAIN_DESC));

	desc.BufferCount = 3;
	desc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    desc.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_DISCARD;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.OutputWindow = this->window;
	desc.SampleDesc.Count = 1;
	desc.Windowed = true;
	desc.BufferDesc.Height = this->mHeight;
	desc.BufferDesc.Width = this->mWidth;
	desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	desc.BufferDesc.RefreshRate.Denominator = 1;
	desc.BufferDesc.RefreshRate.Numerator = 60;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
    #ifdef _DEBUG
		D3D11_CREATE_DEVICE_DEBUG,
    #else
        NULL,
    #endif
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&desc,
		&mSwapChain,
		&this->mDevice,
		NULL,
		&this->mContext);

    Global::device = mDevice;
    Global::context = mContext;

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
    #ifdef _DEBUG
		HRESULT hr = this->mDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast <void **>(&mDebugDevice));
		if (FAILED(hr))
		{
			MessageBox(0, "debug device creation failed", "error", MB_OK);
		}
    #else
        mDebugDevice = nullptr;
    #endif
	}
	else
	{
		MessageBox(0, "remove debug flag", "error", MB_OK);
	}

	return hr;
}

long long Engine::timer()
{
	static LARGE_INTEGER frequency;
	static BOOL highResCounter = QueryPerformanceFrequency(&frequency);
	if (highResCounter)
	{
		LARGE_INTEGER now;
		QueryPerformanceCounter(&now);
		//returns time in ms
		return(1000LL * now.QuadPart) / frequency.QuadPart;
	}
	else
	{
		return GetTickCount();
	}
}


int Engine::run()
{
    Settings& setting = Settings::getInstance();
	MSG msg = { 0 };
	this->createSwapChain();
	Global::mainCamera = new Graphics::Camera(mDevice, mWidth, mHeight, 250, setting.getFOV());
    Global::mainCamera->update({ 0,0,-15 }, { 0,0,1 }, mContext);

	ImGui_ImplDX11_Init(window, mDevice, mContext);


    {
        

        Graphics::Shader fullscreen_shader(Resources::Shaders::FullscreenQuad);
        Texture loadscreen(Resources::Textures::Paths.at(Resources::Textures::LaunchScreen));

        static float clearColor[4] = { 0 };
        Global::context->ClearRenderTargetView(mBackBufferRTV, clearColor);
        Global::context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

        DirectX::CommonStates states(mDevice);
        Global::context->OMSetDepthStencilState(states.DepthNone(), 0);
        //Global::context->OMSetBlendState(states.Opaque(), 0, -1);
        Global::context->RSSetState(states.CullNone());

        Global::context->IASetInputLayout(nullptr);
        Global::context->VSSetShader(fullscreen_shader, nullptr, 0);
        Global::context->PSSetShader(fullscreen_shader, nullptr, 0);

        auto sampler = states.LinearWrap();
        Global::context->PSSetSamplers(0, 1, &sampler);

        
        Global::context->PSSetShaderResources(0, 1, loadscreen);
        Global::context->OMSetRenderTargets(1, &mBackBufferRTV, nullptr);

        D3D11_VIEWPORT viewPort = { 0 };
        viewPort.Width = WIN_WIDTH;
        viewPort.Height = WIN_HEIGHT;
        viewPort.MaxDepth = 1.0f;

        Global::context->RSSetViewports(1, &viewPort);

        Global::context->Draw(3, 0);

        Global::context->OMSetRenderTargets(0, Global::nulls, nullptr);
        Global::context->PSSetShaderResources(0, 1, Global::nulls);
        mSwapChain->Present(0, 0);
    }



	this->renderer = newd Graphics::Renderer(mDevice, mContext, mBackBufferRTV, Global::mainCamera);

	long long start = this->timer();
	long long prev = start;
	long long currentTime = 0;
	long long deltaTime = 0; 
    long long totalTime = 0;
	bool showProfiler = false;

    DirectX::SimpleMath::Vector3 oldPos = { 0, 0, 0 };

	bool running = true;

	Profiler::set(newd Profiler(mDevice, mContext));
	g_Profiler->registerThread("Main Thread");
    TbbProfilerObserver observer(g_Profiler);

    DebugWindow * debug = DebugWindow::getInstance();

    Graphics::Debug::Initialize(mDevice);

	while (running)
	{
		currentTime = this->timer();
		deltaTime = currentTime - prev;
		prev = currentTime;
        totalTime += deltaTime;

		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

            if (WM_QUIT == msg.message)
            {
                running = false;
            }
		}

        if (!running) break;

        auto state = mKeyboard->GetState();
        mTracker->Update(state);

        static BOOL test = false;
        mSwapChain->GetFullscreenState(&test, NULL);

        //temp
        SpecialEffectRenderInfo shakeInfo;
        shakeInfo.type = shakeInfo.screenBounce;
        shakeInfo.duration = 0.2f;
        shakeInfo.radius = 160.0f;
        shakeInfo.bounceMax = 15.0f;
        shakeInfo.direction = DirectX::SimpleMath::Vector2(0.0f, 1.0f);
        shakeInfo.affectEveryThing = false;

#ifdef _DEBUG
        if (mTracker->pressed.G)
            QueueRender(shakeInfo);
#endif // _DEBUG
        Settings& setting = Settings::getInstance();
		if (setting.getWindowed() != test )//&& settingsFullScreenOverRide == false)
		{
			mSwapChain->SetFullscreenState(setting.getWindowed(), NULL);
		}



        //static Graphics::ParticleEffect fire = Graphics::FXSystem->getEffect("FireSmoke");
        //Graphics::FXSystem->processEffect(&fire, DirectX::XMMatrixTranslation(3, 0, 3), deltaTime / 1000.f);		          
        static int deCam = 0;
        if (mTracker->pressed.C) {
            deCam++;
            DebugWindow::getInstance()->addLog("#%d\n", deCam);
            auto fwd = Global::mainCamera->getForward();
            auto pos = Global::mainCamera->getPos();
            printf("{ { %.3f, %.3f, %.3f }, { %.3f, %.3f, %.3f } },\n", pos.x, pos.y, pos.z, fwd.x, fwd.y, fwd.z);
        }
		if (mTracker->pressed.F2)
		{
			g_Profiler->capture();
			showProfiler = true;
		}

		if (mTracker->pressed.F3)
		{
            showProfiler = !showProfiler;
		}

        if (state.F10)
            running = false;

		g_Profiler->start();

		PROFILE_BEGINC("ImGui_ImplDX11_NewFrame", EventColor::PinkLight);
		ImGui_ImplDX11_NewFrame();
		PROFILE_END();

		debug->draw();

		PROFILE_BEGINC("Game::update()", EventColor::Magenta);
        if (!debug->isOpen())
            if (game->update(float(deltaTime)))
                running = false;
        PROFILE_END();

        Global::mainCamera->render();

		PROFILE_BEGINC("Game::render()", EventColor::Red);
		game->render();
		PROFILE_END();
        
        PROFILE_BEGINC("Renderer::update()", EventColor::Pear);
        renderer->update(deltaTime / 1000.f);
        PROFILE_END();

        PROFILE_BEGINC("Renderer::render()", EventColor::PinkDark);
        renderer->render();
        PROFILE_END();

		g_Profiler->poll();
//#ifdef _DEBUG
        if (showProfiler) {

            ImGui::SetNextWindowPos(ImVec2(0, 0));
            ImGui::SetNextWindowSize(ImVec2(WIN_WIDTH, 250));
            g_Profiler->render();
    }
//#endif // _DEBUG

// TODO FIX IMGUI
//        Graphics::Debug::Render(Global::mainCamera);
		mContext->OMSetRenderTargets(1, &mBackBufferRTV, nullptr);
		PROFILE_BEGINC("ImGui::Render()", EventColor::PinkLight);
		ImGui::Render();
		PROFILE_END();

		PROFILE_BEGINC("IDXGISwapChain::Present()", EventColor::Cyan);
		mSwapChain->Present(1, 0);
		PROFILE_END();
		g_Profiler->frame();

        RenderQueue::get().clearAllQueues();
    }

    setting.writeToFile();
    delete Global::mainCamera;
    Graphics::Debug::Destroy();
//    mDebugDevice->ReportLiveDeviceObjects(D3D11_RLDO_FLAGS::D3D11_RLDO_DETAIL);
    g_Profiler->end();
	delete g_Profiler;

	return 0;
}
