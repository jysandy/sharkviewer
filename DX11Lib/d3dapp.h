#pragma once

#include "D3dutil.h"
#include "gametimer.h"
#include "Camera.h"

#include <d3d11.h>
#include <dxgi.h>
#include <string>
#include <cassert>
#include <sstream>
#include <DirectXMath.h>

namespace D3dtut
{
	using namespace D3dutil;
	class D3DApp
	{
	public:
		D3DApp(HINSTANCE hinstance);
		virtual ~D3DApp();

		WPARAM Run();

		virtual bool Init();
		virtual void OnResize();
		virtual void UpdateScene(float dt);
		virtual void DrawScene() = 0;
		virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

		virtual void OnMouseDown(WPARAM btnState, int x, int y);
		virtual void OnMouseUp(WPARAM btnState, int x, int y);
		virtual void OnMouseMove(WPARAM btnState, int x, int y);
		
	protected:
		bool InitMainWindow();
		bool InitDirect3D();
		void CalculateFrameStats();
		void Pause();
		void Resume();
		float AspectRatio() const;

	protected:
		ComPtr<ID3D11Device> device;
		ComPtr<ID3D11DeviceContext> deviceContext;
		ComPtr<IDXGISwapChain> swapChain;
		ComPtr<ID3D11Texture2D> depthStencilBuffer;
		ComPtr<ID3D11RenderTargetView> renderTargetView;
		ComPtr<ID3D11DepthStencilView> depthStencilView;
		D3D11_VIEWPORT screenViewport;

		HWND hwnd;
		HINSTANCE hinstance;

		std::wstring windowCaption;
		D3D_DRIVER_TYPE d3dDriverType;
		int clientWidth;
		int clientHeight;
		bool mxaaEnabled;
		UINT mxaaQuality;
		GameTimer timer;
		Camera camera;

		bool paused;
		bool resizing;

		DirectX::XMFLOAT4X4 projectionMatrix;
	};
}