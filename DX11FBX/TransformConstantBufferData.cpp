#include "TransformConstantBufferData.h"

namespace FBXDemo
{
	void TransformConstantBufferData::SetWorldMatrix(DirectX::XMFLOAT4X4 in)
	{
		DirectX::XMMATRIX temp = DirectX::XMLoadFloat4x4(&in);
		temp = DirectX::XMMatrixTranspose(temp);
		DirectX::XMStoreFloat4x4(&world, temp);
	}

	void TransformConstantBufferData::SetViewMatrix(DirectX::XMFLOAT4X4 in)
	{
		DirectX::XMMATRIX temp = DirectX::XMLoadFloat4x4(&in);
		temp = DirectX::XMMatrixTranspose(temp);
		DirectX::XMStoreFloat4x4(&view, temp);
	}

	void TransformConstantBufferData::SetProjectionMatrix(DirectX::XMFLOAT4X4 in)
	{
		DirectX::XMMATRIX temp = DirectX::XMLoadFloat4x4(&in);
		temp = DirectX::XMMatrixTranspose(temp);
		DirectX::XMStoreFloat4x4(&projection, temp);
	}
}