#pragma once
#include <DirectXMath.h>
#include <Windows.h>
#include <cmath>
#include <algorithm>

namespace D3dtut
{
	class Camera
	{
	public:
		Camera();

		DirectX::XMFLOAT4X4 getViewMatrix() const;
		DirectX::XMFLOAT3 getCameraPosition() const;
		DirectX::XMFLOAT3 getCameraDirection() const;
		void Update(float dt);

		void OnMouseDown(WPARAM btnState, int x, int y);
		void OnMouseUp(WPARAM btnState, int x, int y);
		void OnMouseMove(WPARAM btnState, int x, int y);

	private:
		const float mouseSensitivity = 0.004f;
		const float velocity = 20.0f;

		DirectX::XMFLOAT4X4 viewMatrix;
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 lookDirection;

		bool mouseCaptured;
		int prevX;
		int prevY;

		float pitch;	//Angle made with X-Z plane
		float yaw;		//Angle made with Y-Z plane
	};
}