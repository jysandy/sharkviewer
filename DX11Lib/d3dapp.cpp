#include "d3dapp.h"

namespace D3dtut
{
	static D3DApp* appGlobal = 0;

	LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		if (appGlobal)
		{
			return appGlobal->MsgProc(hwnd, msg, wparam, lparam);
		}
		else
		{
			return DefWindowProc(hwnd, msg, wparam, lparam);
		}
	}

	D3DApp::D3DApp(HINSTANCE hinstance) :
		hinstance(hinstance),
		windowCaption(L"D3D11 application"),
		clientWidth(800),
		clientHeight(600),
		hwnd(0),
		mxaaEnabled(true),
		mxaaQuality(0),
		d3dDriverType(D3D_DRIVER_TYPE_HARDWARE)
	{
		SecureZeroMemory(&screenViewport, sizeof(screenViewport));
		appGlobal = this;
	}

	D3DApp::~D3DApp()
	{
		if (deviceContext.get())
		{
			this->deviceContext->ClearState();
		}
	}

	WPARAM D3DApp::Run()
	{
		MSG msg = { 0 };
		this->timer.Reset();

		while (msg.message != WM_QUIT)
		{
			if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				this->timer.Tick();

				if (!paused)
				{
					CalculateFrameStats();
					UpdateScene(this->timer.DeltaTime());
					DrawScene();
				}
				else
				{
					Sleep(100);
				}
			}
		}

		return msg.wParam;
	}

	bool D3DApp::Init()
	{
		return this->InitMainWindow() && this->InitDirect3D();
	}

	bool D3DApp::InitMainWindow()
	{
		const std::wstring className = L"huehuehue";

		WNDCLASS wc;
		SecureZeroMemory(&wc, sizeof(wc));
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = MainWindowProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = this->hinstance;
		wc.hIcon = LoadIcon(0, IDI_APPLICATION);
		wc.hCursor = LoadCursor(0, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
		wc.lpszMenuName = 0;
		wc.lpszClassName = className.c_str();

		if (!RegisterClass(&wc))
		{
			MessageBox(0, L"Register class failed", L"Error", MB_ICONERROR);
			return false;
		}

		RECT R = { 0, 0, this->clientWidth, this->clientHeight };
		AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
		int width = R.right - R.left;
		int height = R.bottom - R.top;

		this->hwnd = CreateWindow(
			className.c_str(),
			this->windowCaption.c_str(),
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			width,
			height,
			NULL,
			NULL,
			this->hinstance,
			NULL
			);

		if (!this->hwnd)
		{
			MessageBox(NULL, L"Failed to create window", L"Error", MB_ICONERROR);
			return false;
		}

		ShowWindow(this->hwnd, SW_SHOW);
		UpdateWindow(this->hwnd);

		return true;
	}

	bool D3DApp::InitDirect3D()
	{
		using D3dutil::HR;

		UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined (_DEBUG)
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
		D3D_FEATURE_LEVEL featureLevel;
		HRESULT hr = D3D11CreateDevice(
			NULL,
			this->d3dDriverType,
			NULL,
			createDeviceFlags,
			0,
			0,
			D3D11_SDK_VERSION,
			this->device.address(),
			&featureLevel,
			this->deviceContext.address());

		if (FAILED(hr))
		{
			MessageBox(NULL, L"Failed to create device", L"Error", MB_ICONERROR);
			return false;
		}

		if (featureLevel != D3D_FEATURE_LEVEL_11_0)
		{
			MessageBox(0, L"Direct3D 11 not supported", L"Error", MB_ICONERROR);
			return false;
		}

		HR(
			this->device->CheckMultisampleQualityLevels(
			DXGI_FORMAT_R8G8B8A8_UNORM,
			4,
			&this->mxaaQuality));

		assert(this->mxaaQuality > 0);

		DXGI_SWAP_CHAIN_DESC sd;
		SecureZeroMemory(&sd, sizeof(sd));
		sd.BufferCount = 1;
		sd.BufferDesc.Width = this->clientWidth;
		sd.BufferDesc.Height = this->clientHeight;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = this->hwnd;
		sd.Windowed = true;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		if (this->mxaaEnabled)
		{
			sd.SampleDesc.Count = 4;
			sd.SampleDesc.Quality = this->mxaaQuality - 1;
		}
		else
		{
			sd.SampleDesc.Count = 1;
			sd.SampleDesc.Quality = 0;
		}

		//Get factory used to create device
		ComPtr<IDXGIDevice> dxgiDevice;
		HR(
			this->device->QueryInterface(
			__uuidof(IDXGIDevice),
			reinterpret_cast<void**>(dxgiDevice.address())));

		ComPtr<IDXGIAdapter> adapter;
		HR(
			dxgiDevice->GetParent(
			__uuidof(IDXGIAdapter),
			reinterpret_cast<void**>(adapter.address())));

		ComPtr<IDXGIFactory> factory;
		HR(
			adapter->GetParent(
			__uuidof(IDXGIFactory),
			reinterpret_cast<void**>(factory.address())));

		//Create swap chain with the factory
		HR(
			factory->CreateSwapChain(
			this->device.get(),
			&sd,
			this->swapChain.address()));

		assert(this->swapChain.get());

		//Remaining procedure is duplicated in the OnResize method
		this->OnResize();

		return true;
	}

	void D3DApp::OnResize()
	{
		assert(device.get());
		assert(swapChain.get());
		assert(deviceContext.get());

		this->renderTargetView.release();
		this->depthStencilView.release();
		this->depthStencilBuffer.release();

		HR(
			this->swapChain->ResizeBuffers(
			1,
			this->clientWidth,
			this->clientHeight,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			0));

		ComPtr<ID3D11Texture2D> backBuffer;
		HR(
			this->swapChain->GetBuffer(
			0,
			__uuidof(ID3D11Texture2D),
			reinterpret_cast<void**>(backBuffer.address())));

		//assert(backBuffer.get());

		HR(
			device->CreateRenderTargetView(
			backBuffer.get(),
			0,
			renderTargetView.address()));

		D3D11_TEXTURE2D_DESC depthStencilDesc;
		depthStencilDesc.Width = this->clientWidth;
		depthStencilDesc.Height = this->clientHeight;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;

		if (this->mxaaEnabled)
		{
			depthStencilDesc.SampleDesc.Count = 4;
			depthStencilDesc.SampleDesc.Quality = this->mxaaQuality - 1;
		}
		else
		{
			depthStencilDesc.SampleDesc.Count = 1;
			depthStencilDesc.SampleDesc.Quality = 0;
		}

		HR(
			this->device->CreateTexture2D(
			&depthStencilDesc,
			0,
			this->depthStencilBuffer.address()));

		HR(
			this->device->CreateDepthStencilView(
			this->depthStencilBuffer.get(),
			0,
			this->depthStencilView.address()));

		this->deviceContext->OMSetRenderTargets(
			1,
			this->renderTargetView.address(),
			this->depthStencilView.get());

		this->screenViewport.TopLeftX = 0;
		screenViewport.TopLeftY = 0;
		screenViewport.Width = static_cast<float>(this->clientWidth);
		screenViewport.Height = static_cast<float>(this->clientHeight);
		screenViewport.MinDepth = 0.0f;
		screenViewport.MaxDepth = 1.0f;

		this->deviceContext->RSSetViewports(1, &this->screenViewport);

		auto foo = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV4, this->AspectRatio(), 1.0f, 1000.0f);
		DirectX::XMStoreFloat4x4(&projectionMatrix, foo);
	}

	LRESULT D3DApp::MsgProc(HWND hwindow, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		switch (msg)
		{
		case WM_ACTIVATE:
			if (LOWORD(wparam) == WA_INACTIVE)
			{
				Pause();
			}
			else
			{
				Resume();
			}
			return 0;

		case WM_ENTERSIZEMOVE:
			this->resizing = true;
			this->Pause();
			return 0;

		case WM_EXITSIZEMOVE:
			this->resizing = false;
			this->Resume();
			this->OnResize();
			return 0;

		case WM_SIZE:
			this->clientWidth = LOWORD(lparam);
			this->clientHeight = HIWORD(lparam);

			if (this->device.get())
			{
				if (wparam == SIZE_MINIMIZED)
				{
					Pause();
				}
				else if (wparam == SIZE_MAXIMIZED)
				{
					Resume();
				}
				else if (wparam == SIZE_RESTORED)
				{
					if (!resizing)
					{
						OnResize();
					}
				}
			}

			return 0;

		case WM_DESTROY:
		case WM_CLOSE:
			PostQuitMessage(0);
			return 0;

		case WM_GETMINMAXINFO:
		{
			MINMAXINFO* info = reinterpret_cast<MINMAXINFO*>(lparam);
			info->ptMinTrackSize.x = 200;
			info->ptMinTrackSize.y = 200;
			return 0;
		}

		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
			OnMouseDown(wparam, GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
			return 0;

		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
			OnMouseUp(wparam, GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
			return 0;

		case WM_MOUSEMOVE:
			OnMouseMove(wparam, GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
			return 0;

		default:
			return DefWindowProc(hwindow, msg, wparam, lparam);
		}
	}

	void D3DApp::CalculateFrameStats()
	{
		static int frameCount = 0;
		static float timeElapsed = 0.0f;

		if (timeElapsed >= 1.0f && (frameCount != 0))
		{
			float fps = static_cast<float>(frameCount);
			float mspf = 1000.0f / fps;

			std::wostringstream out;
			out.precision(6);

			out << windowCaption << L" FPS: " << fps << " Frame Time: " << mspf << L" (ms)";
			SetWindowText(this->hwnd, out.str().c_str());
			frameCount = 0;
			timeElapsed = 0;
		}
		else
		{
			frameCount++;
			timeElapsed += timer.DeltaTime();
		}
	}

	void D3DApp::Pause()
	{
		this->paused = true;
		timer.Stop();
	}

	void D3DApp::Resume()
	{
		this->paused = false;
		timer.Start();
	}

	void D3DApp::UpdateScene(float dt)
	{
		this->camera.Update(dt);
	}

	float D3DApp::AspectRatio() const
	{
		if (clientHeight != 0)
		{
			return clientWidth / static_cast<float>(clientHeight);
		}
		else
		{
			return 0.0f;
		}
	}

	void D3DApp::OnMouseDown(WPARAM btnState, int x, int y)
	{
		SetCapture(this->hwnd);
		this->camera.OnMouseDown(btnState, x, y);
	}

	void D3DApp::OnMouseUp(WPARAM btnState, int x, int y)
	{
		ReleaseCapture();
		this->camera.OnMouseUp(btnState, x, y);
	}

	void D3DApp::OnMouseMove(WPARAM btnState, int x, int y)
	{
		this->camera.OnMouseMove(btnState, x, y);
	}
}