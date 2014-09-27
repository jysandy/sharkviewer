#include "FBXDemo.h"

namespace FBXDemo
{
	FBXApp::FBXApp(HINSTANCE hinstance) : D3dtut::D3DApp(hinstance), compiledVS("VertexShader.cso"), compiledPS("PixelShader.cso")
	{
		this->windowCaption = L"FBX Model";
		CoInitializeEx(0, 0);
	}

	FBXApp::~FBXApp()
	{
		CoUninitialize();
	}

	bool FBXApp::Init()
	{
		bool foo = D3dtut::D3DApp::Init();

		if (foo)
		{
			CreateShaders();
			LoadModels();
			CreateInputLayouts();
			CreateLights();
			CreateBuffers();
		}

		return foo;
	}

	void FBXApp::CreateShaders()
	{
		using D3dtut::HR;

		HR(
			this->device->CreateVertexShader(compiledVS.get(), compiledVS.getSize(), nullptr, vertexShader.address()));

		HR(
			this->device->CreatePixelShader(compiledPS.get(), compiledPS.getSize(), nullptr, pixelShader.address()));
	}

	void FBXApp::CreateInputLayouts()
	{
		using D3dtut::HR;
		D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{
				"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA, 0
			},
			{
				"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA, 0
			},
			{
				"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA, 0
			},
			{
				"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA, 0
			}
		};

		HR(
			device->CreateInputLayout(vertexDesc, 4, compiledVS.get(), compiledVS.getSize(), inputLayout.address()));
	}

	void FBXApp::CreateLights()
	{
		DirectionalLight dLight;
		dLight.ambient = XMFLOAT4(0.2, 0.2, 0.2, 1.0);
		dLight.diffuse = XMFLOAT4(0.9, 0.9, 0.9, 1.0);
		dLight.specular = XMFLOAT4(0.3, 0.3, 0.3, 1.0);
		XMFLOAT3 dir = XMFLOAT3(1, -1, 1);
		XMVECTOR foo = XMLoadFloat3(&dir);
		foo = XMVector3Normalize(foo);
		XMStoreFloat3(&dLight.direction, foo);
		lightData.dLight = dLight;
	}

	void FBXApp::CreateBuffers()
	{
		using D3dtut::HR;

		D3D11_BUFFER_DESC desc;
		SecureZeroMemory(&desc, sizeof(desc));
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.ByteWidth = geometryBufferData.getVertices().size() * sizeof(D3dtut::Vertex);
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;

		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = geometryBufferData.getVertices().data();
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;

		HR(
			device->CreateBuffer(&desc, &data, vertexBuffer.address()));

		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.ByteWidth = geometryBufferData.getIndices().size() * sizeof(unsigned int);
		data.pSysMem = geometryBufferData.getIndices().data();

		HR(
			device->CreateBuffer(&desc, &data, indexBuffer.address()));

		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.ByteWidth = sizeof(TransformConstantBufferData);
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		HR(
			device->CreateBuffer(&desc, nullptr, transformConstantBuffer.address()));

		desc.ByteWidth = sizeof(LightConstantBufferData);

		HR(
			device->CreateBuffer(&desc, nullptr, lightConstantBuffer.address()));
	}

	void FBXApp::LoadModels()
	{
		std::string filename = "megalodon.FBX";
		const wchar_t* texturename = L"megalodon.png";

		FbxManager* sdkManager = FbxManager::Create();
		FbxIOSettings* ios = FbxIOSettings::Create(sdkManager, IOSROOT);
		sdkManager->SetIOSettings(ios);

		FbxImporter* importer = FbxImporter::Create(sdkManager, "Importer");

		if (!importer->Initialize(filename.c_str(), -1, sdkManager->GetIOSettings()))
		{
			throw std::exception(importer->GetStatus().GetErrorString());
		}

		FbxScene* scene = FbxScene::Create(sdkManager, "Scene");
		importer->Import(scene);
		importer->Destroy();

		//FbxAxisSystem::DirectX.ConvertScene(scene);
		//auto settings = scene->GetGlobalSettings();

		FbxNode* rootNode = scene->GetRootNode();
		if (!rootNode)
		{
			throw D3dtut::D3DException(L"Empty file. No root node present.");
		}

		for (int i = 0; i < rootNode->GetChildCount(); i++)
		{
			FbxNode* child = rootNode->GetChild(i);
			BuildMeshes(child);
		}

		D3dtut::HR(
			DirectX::CreateWICTextureFromFile(
			device.get(),
			texturename,
			reinterpret_cast<ID3D11Resource**>(modelTexture.address()),
			shaderResourceView.address()));

		D3D11_SAMPLER_DESC samplerDesc;
		SecureZeroMemory(&samplerDesc, sizeof(samplerDesc));
		samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		samplerDesc.MaxAnisotropy = 16;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.MaxLOD = FLT_MAX;

		D3dtut::HR(
			device->CreateSamplerState(&samplerDesc, samplerState.address())
			);
	}

	void FBXApp::BuildMeshes(FbxNode* child)
	{
		for (int j = 0; j < child->GetNodeAttributeCount(); j++)
		{
			FbxMesh* mesh = child->GetMesh();
			if (mesh)
			{
				FbxAMatrix& worldMatrix = child->EvaluateGlobalTransform();
				DirectX::XMFLOAT4X4 world;
				mesh->GenerateNormals();
				for (int k = 0; k < 4; k++)
				{
					for (int l = 0; l < 4; l++)
					{
						world.m[k][l] = worldMatrix.Get(k, l);
					}
				}

				//DirectX::XMStoreFloat4x4(&world, DirectX::XMMatrixIdentity());
				XMMATRIX balls = XMLoadFloat4x4(&world);
				balls *= DirectX::XMMatrixRotationX(DirectX::XM_PI);
				//DirectX::XMStoreFloat4x4(&world, balls);

				FbxGeometryElementNormal* normals = mesh->GetElementNormal();
				FbxGeometryElementUV* uvs = mesh->GetElementUV();

				if (normals->GetMappingMode() != FbxLayerElement::eByPolygonVertex)
				{
					throw D3dtut::D3DException(L"Unsupported mapping type of normal layer.");
				}

				if (normals->GetReferenceMode() != FbxLayerElement::eDirect)
				{
					throw D3dtut::D3DException(L"Unsupported reference mode of normal layer.");
				}

				if (!uvs)
				{
					throw D3dtut::D3DException(L"No UV mapping information available.");
				}

				if (uvs->GetMappingMode() != FbxLayerElement::eByPolygonVertex)
				{
					throw D3dtut::D3DException(L"Unsupported mapping type of UV layer.");
				}

				if (uvs->GetReferenceMode() != FbxLayerElement::eIndexToDirect)
				{
					throw D3dtut::D3DException(L"Unsupported reference mode of UV layer.");
				}

				const char* uvSetName = uvs->GetName();

				auto controlPoints = mesh->GetControlPoints();
				auto controlIndices = mesh->GetPolygonVertices();
				auto normalArray = normals->GetDirectArray();
				auto normalIndexArray = normals->GetIndexArray();
				auto uvArray = uvs->GetDirectArray();
				auto uvIndexArray = uvs->GetIndexArray();

				std::vector<D3dtut::Vertex> vertices;
				std::vector<unsigned int> indices;

				unsigned int index = 0;
				for (int polygonIndex = 0; polygonIndex < mesh->GetPolygonCount(); polygonIndex++)
				{
					if (mesh->GetPolygonSize(polygonIndex) != 3)
					{
						throw D3dtut::D3DException(L"Polygon is not a triangle.");
					}

					for (int vertexPosition = 0; vertexPosition < mesh->GetPolygonSize(polygonIndex); vertexPosition++)
					{
						D3dtut::Vertex foo;
						int controlPointIndex = mesh->GetPolygonVertex(polygonIndex, vertexPosition);

						foo.position.x = controlPoints[controlPointIndex][0];
						foo.position.y = controlPoints[controlPointIndex][1];
						foo.position.z = controlPoints[controlPointIndex][2];

						FbxVector4 norm;
						mesh->GetPolygonVertexNormal(polygonIndex, vertexPosition, norm);
						int normalIndex = normalIndexArray[controlPointIndex];
						foo.normal.x = norm[0];
						foo.normal.y = norm[1];
						foo.normal.z = norm[2];

						FbxVector2 tex;
						bool mapped;
						mesh->GetPolygonVertexUV(polygonIndex, vertexPosition, uvSetName, tex, mapped);
						int uvIndex = uvIndexArray[controlPointIndex];
						foo.texCoord.x = tex[0];
						foo.texCoord.y = 1 - tex[1];

						vertices.push_back(foo);
						indices.push_back(index);

						index++;
					}
				}

				D3dtut::MeshData data;
				data.vertices = vertices;
				data.indices = indices;

				//Get the material.
				auto material = child->GetMaterial(0);

				if (!material)
				{
					throw D3dtut::D3DException(L"No material found for a mesh.");
				}

				if (!material->Is<FbxSurfacePhong>())
				{
					throw D3dtut::D3DException(L"Not phong material.");
				}

				try
				{
					FbxSurfacePhong* phongSurface = reinterpret_cast<FbxSurfacePhong*>(material);
					Material ganda;
					ganda.ambient.x = phongSurface->Ambient.Get()[0];
					ganda.ambient.y = phongSurface->Ambient.Get()[1];
					ganda.ambient.z = phongSurface->Ambient.Get()[2];
					ganda.ambient.w = 1;

					ganda.diffuse.x = phongSurface->Diffuse.Get()[0];
					ganda.diffuse.y = phongSurface->Diffuse.Get()[1];
					ganda.diffuse.z = phongSurface->Diffuse.Get()[2];
					ganda.diffuse.w = 1;

					ganda.specular.x = phongSurface->Specular.Get()[0];
					ganda.specular.y = phongSurface->Specular.Get()[1];
					ganda.specular.z = phongSurface->Specular.Get()[2];
					ganda.specular.w = phongSurface->Shininess.Get();

					D3dtut::Model model(data.getId(), "mesh");
					model.world = world;
					models.push_back(model);
					materials[model.getId()] = ganda;
					geometryBufferData.Append(data);
				}
				catch (std::exception&)
				{
					//Do nothing.
				}
			}
		}

		for (int i = 0; i < child->GetChildCount(); i++)
		{
			BuildMeshes(child->GetChild(i));
		}
	}

	void FBXApp::UpdateScene(float dt)
	{
		D3DApp::UpdateScene(dt);
		lightData.cameraPosition = camera.getCameraPosition();
	}

	void FBXApp::DrawScene()
	{
		using D3dtut::HR;

		deviceContext->ClearRenderTargetView(renderTargetView.get(), DirectX::Colors::LightCyan);
		deviceContext->ClearDepthStencilView(depthStencilView.get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		deviceContext->IASetInputLayout(inputLayout.get());
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		unsigned int stride = sizeof(D3dtut::Vertex);
		unsigned int offset = 0;
		deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.address(), &stride, &offset);
		deviceContext->IASetIndexBuffer(indexBuffer.get(), DXGI_FORMAT_R32_UINT, 0);

		deviceContext->VSSetShader(vertexShader.get(), nullptr, 0);
		deviceContext->PSSetShader(pixelShader.get(), nullptr, 0);
		deviceContext->PSSetShaderResources(0, 1, shaderResourceView.address());
		deviceContext->PSSetSamplers(0, 1, samplerState.address());
		for (auto& model : models)
		{
			auto indexData = geometryBufferData.getIndexOffsetData();
			auto modelIndexData = indexData[model.getId()];

			D3D11_MAPPED_SUBRESOURCE mappedResource;
			deviceContext->Map(transformConstantBuffer.get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
			TransformConstantBufferData* tcBufferData = reinterpret_cast<TransformConstantBufferData*>(mappedResource.pData);
			tcBufferData->SetWorldMatrix(model.world);
			tcBufferData->SetViewMatrix(camera.getViewMatrix());
			tcBufferData->SetProjectionMatrix(projectionMatrix);
			deviceContext->Unmap(transformConstantBuffer.get(), 0);

			lightData.material = materials[model.getId()];

			deviceContext->Map(lightConstantBuffer.get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
			LightConstantBufferData* lcBufferData = reinterpret_cast<LightConstantBufferData*>(mappedResource.pData);
			*lcBufferData = lightData;
			deviceContext->Unmap(lightConstantBuffer.get(), 0);

			deviceContext->VSSetConstantBuffers(0, 1, transformConstantBuffer.address());
			deviceContext->PSSetConstantBuffers(1, 1, lightConstantBuffer.address());
			//deviceContext->RSSetState(rasterState.get());
			deviceContext->DrawIndexed(modelIndexData.indexCount, modelIndexData.baseIndexOffset, modelIndexData.baseVertexOffset);
		}

		HR(swapChain->Present(1, 0));
	}
}