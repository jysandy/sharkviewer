#include "Camera.h"

namespace D3dtut
{
	Camera::Camera() : pitch(0), yaw(0), prevX(0), prevY(0) 
	{
		position = DirectX::XMFLOAT3(-100.0f, 100.0f, -10.0f);
	}

	DirectX::XMFLOAT4X4 Camera::getViewMatrix() const
	{
		return this->viewMatrix;
	}

	DirectX::XMFLOAT3 Camera::getCameraPosition() const
	{
		return position;
	}

	DirectX::XMFLOAT3 Camera::getCameraDirection() const
	{
		return lookDirection;
	}

	void Camera::Update(float dt)
	{
		using namespace DirectX;
		using std::min;
	
		SHORT forward = GetAsyncKeyState('W');
		SHORT backward = GetAsyncKeyState('S');
		SHORT left = GetAsyncKeyState('A');
		SHORT right = GetAsyncKeyState('D');

		if (yaw > XM_2PI)
		{
			yaw = XM_2PI - yaw;
		}

		if (pitch > XM_PIDIV2- 0.1f)
		{
			pitch = XM_PIDIV2 - 0.1f;
		}

		if (pitch < -XM_PIDIV2 + 0.1f)
		{
			pitch = -XM_PIDIV2 + 0.1f;
		}

		//Pitch
		float cosp = std::cosf(pitch);
		float sinp = std::sinf(pitch);
		
		//Yaw
		float cosy = std::cosf(yaw);
		float siny = std::sinf(yaw);

		XMFLOAT4 temp = XMFLOAT4(siny * cosp, sinp, cosp * cosy, 1.0f);
		XMVECTOR direction = XMLoadFloat4(&temp);
		direction = XMVector3Normalize(direction);
		XMStoreFloat3(&lookDirection, direction);

		
		temp = XMFLOAT4(0.0, 1.0f, 0.0, 0.0f);
		XMVECTOR yAxis = XMLoadFloat4(&temp);
		
		XMVECTOR rightDirection = XMVector3Cross(yAxis, direction);
		rightDirection = XMVector3Normalize(rightDirection);

		temp = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		XMVECTOR displacement = XMLoadFloat4(&temp);

		if (forward & 0x8000)
		{
			displacement += velocity * dt * direction;
		}
		else if (backward & 0x8000)
		{
			displacement -= velocity * dt * direction;
		}

		if (left & 0x8000)
		{
			displacement -= velocity * dt * rightDirection;
		}
		else if (right & 0x8000)
		{
			displacement += velocity * dt * rightDirection;
		}

		XMVECTOR newPos = XMLoadFloat3(&position);
		newPos += displacement;
		XMStoreFloat3(&position, newPos);

		XMStoreFloat4x4(&viewMatrix, XMMatrixLookToLH(newPos, direction, yAxis));
	}

	void Camera::OnMouseDown(WPARAM btnState, int x, int y)
	{
		mouseCaptured = true;
		prevX = x;
		prevY = y;
	}

	void Camera::OnMouseUp(WPARAM btnState, int x, int y)
	{
		mouseCaptured = false;
	}

	void Camera::OnMouseMove(WPARAM btnState, int x, int y)
	{
		if (mouseCaptured)
		{
			float dx = (x - prevX) * mouseSensitivity;
			float dy = (y - prevY) * mouseSensitivity;

			yaw += dx;
			pitch -= dy;

			prevX = x;
			prevY = y;
		}
	}
}