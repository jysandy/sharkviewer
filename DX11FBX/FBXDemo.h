#pragma once
#include <d3dapp.h>
#include <D3DException.h>
#include <CompiledShader.h>
#include <MeshData.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <Model.h>
#include <GeometryBufferData.h>
#include <vector>
#include "DirectionalLight.h"
#include "LightConstantBufferData.h"
#include "TransformConstantBufferData.h"
#include <Vertex.h>
#include <map>
#include <fbxsdk.h>
#include <string>
#include <stdexcept>
#include <exception>


namespace FBXDemo
{
	class FBXApp : public D3dtut::D3DApp
	{
	public:
		FBXApp(HINSTANCE hinstance);
		virtual ~FBXApp();

		virtual bool Init();
		virtual void OnResize();

		virtual void UpdateScene(float dt);
		virtual void DrawScene();

	private:
		void CreateShaders();
		void CreateBuffers();
		void CreateInputLayouts();
		void CreateLights();
		void LoadModels();

	private:
		D3dtut::ComPtr<ID3D11Buffer> vertexBuffer;
		D3dtut::ComPtr<ID3D11Buffer> indexBuffer;
		D3dtut::ComPtr<ID3D11Buffer> transformConstantBuffer;
		D3dtut::ComPtr<ID3D11Buffer> lightConstantBuffer;
		D3dtut::ComPtr<ID3D11RasterizerState> rasterState;
		D3dtut::ComPtr<ID3D11VertexShader> vertexShader;
		D3dtut::ComPtr<ID3D11PixelShader> pixelShader;
		D3dtut::ComPtr<ID3D11InputLayout> inputLayout;

		D3dtut::CompiledShader compiledVS;
		D3dtut::CompiledShader compiledPS;

		D3dtut::GeometryBufferData geometryBufferData;
		std::vector<D3dtut::Model> models;
		std::map<int, Material> materials;

		TransformConstantBufferData transformData;
		LightConstantBufferData lightData;
	};
}