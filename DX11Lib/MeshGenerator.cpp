#include "MeshGenerator.h"

namespace D3dtut
{
	MeshData MeshGenerator::CreateGrid(float lengthX, float lengthZ, int numX, int numZ, const float* color, float(*f)(float, float))
	{
		using namespace DirectX;

		int vertexCount = numX * numZ;
		int triangleCount = 2 * (numX - 1) * (numZ - 1);

		float dx = lengthX / (numX - 1);
		float dz = lengthZ / (numZ - 1);

		float du = 1.0f / (numX - 1);
		float dv = 1.0f / (numZ - 1);

		MeshData meshData;
		meshData.vertices.resize(vertexCount);

		for (int i = 0; i < numZ; i++)
		{
			float z = 0.5 * lengthZ - i * dz;

			for (int j = 0; j < numX; j++)
			{
				float x = -0.5 * lengthX + j * dx;
				assert(i * numX + j < meshData.vertices.size());
				meshData.vertices[i * numX + j].position = XMFLOAT3(x, f(x, z), z);
				meshData.vertices[i * numX + j].normal = XMFLOAT3(0, 1.0f, 0.0);
				meshData.vertices[i * numX + j].texCoord = XMFLOAT2(j * du, i * dv);
				meshData.vertices[i * numX + j].color = XMFLOAT4(color);
			}
		}

		meshData.indices.resize(triangleCount * 3);

		int k = 0;
		for (int i = 0; i < numZ - 1; i++)
		{
			for (int j = 0; j < numX - 1; j++)
			{
				assert(k + 5 < meshData.indices.size());
				meshData.indices[k] = i * numX + j;
				meshData.indices[k + 1] = i * numX + j + 1;
				meshData.indices[k + 2] = (i + 1) * numX + j;
				meshData.indices[k + 3] = (i + 1) * numX + j;
				meshData.indices[k + 4] = i * numX + j + 1;
				meshData.indices[k + 5] = (i + 1) * numX + j + 1;
				k += 6;
			}
		}

		return meshData;
	}

	MeshData MeshGenerator::CreateFrustum(float topRadius, float bottomRadius, int numStacks, int numVerticalSections, float height, const float* color)
	{
		using namespace DirectX;

		MeshData data;

		int numSlices = numStacks + 1;
		int verticesPerSlice = numVerticalSections + 1;
		int numVertices = verticesPerSlice * numSlices;
		int numIndices = 6 * numVerticalSections * numStacks;

		float dh = height / numStacks;

		//Vertex generation
		for (int i = 0; i < numSlices; i++)
		{
			float y = i * dh;
			//Radius of slice
			float r = bottomRadius - (y * (bottomRadius - topRadius)) / height;

			float dTheta = XM_2PI / numVerticalSections;

			//The first and last vertices in each ring are duplicated,
			//because they have different texture coordinates.
			for (int j = 0; j <= numVerticalSections; j++)
			{
				float x = r * cosf(j * dTheta);
				float z = r * sinf(j * dTheta);

				Vertex vertex;
				vertex.color = XMFLOAT4(color);
				vertex.position = XMFLOAT3(x, y, z);
				vertex.texCoord.x = static_cast<float>(j) / numVerticalSections;
				vertex.texCoord.y = 1.0f - static_cast<float>(i) / numStacks;

				XMFLOAT4 temp = XMFLOAT4(x, 0, z, 1.0f);
				XMVECTOR vertexXZ = XMLoadFloat4(&temp);
				vertexXZ = XMVector3Normalize(vertexXZ);
				XMVECTOR radiusTopXZ = topRadius * vertexXZ;
				XMVECTOR radiusBottomXZ = bottomRadius * vertexXZ;
				temp = XMFLOAT4(0, height, 0, 1.0f);
				XMVECTOR axis = XMLoadFloat4(&temp);
				XMVECTOR hue = radiusTopXZ + axis;
				XMVECTOR tangent1 = hue - radiusBottomXZ;
				XMVECTOR tangent2 = XMVector3Cross(radiusBottomXZ, axis);
				XMVECTOR normal = XMVector3Cross(tangent1, tangent2);
				normal = XMVector3Normalize(normal);
				XMStoreFloat3(&vertex.normal, normal);

				data.vertices.push_back(vertex);
			}
		}

		//Index generation
		for (int i = 0; i < (numSlices - 1) * (numVerticalSections + 1); i++)
		{

			data.indices.push_back(i);
			data.indices.push_back(i + (numVerticalSections + 1));
			data.indices.push_back(i + 1);
			data.indices.push_back(i + 1);
			data.indices.push_back(i + (numVerticalSections + 1));
			data.indices.push_back((i + 1) + numVerticalSections + 1);
		}

		//Vertex and index generation for the caps (top and bottom)

		int lastVertex = data.vertices.size() - 1;
		//Bottom cap

		//Center vertex.
		Vertex center;
		center.position = XMFLOAT3(0, 0, 0);
		center.normal = XMFLOAT3(0, -1, 0);
		center.color = XMFLOAT4(color);
		center.texCoord = XMFLOAT2(0.5, 0.5);

		data.vertices.push_back(center);

		int centerIndex = data.vertices.size() - 1;

		//Copy the bottom ring of vertices.
		vector<Vertex> bottomVertices;
		for (int i = 0; i < verticesPerSlice; i++)
		{
			bottomVertices.push_back(data.vertices[i]);
		}

		for (auto& x : bottomVertices)
		{
			XMFLOAT3 normal = XMFLOAT3(0, -1, 0);
			x.normal = normal;
			float u = x.position.x / height + 0.5;
			float v = x.position.z / height + 0.5;
			x.texCoord = XMFLOAT2(u, v);
		}

		data.vertices.insert(data.vertices.end(), bottomVertices.begin(), bottomVertices.end());

		for (int i = centerIndex + 1; i < centerIndex + verticesPerSlice; i++)
		{
			data.indices.push_back(i + 1);
			data.indices.push_back(centerIndex);
			data.indices.push_back(i);
		}

		//Top cap

		//Center vertex
		center.position = XMFLOAT3(0, height, 0);
		center.normal = XMFLOAT3(0, 1, 0);
		center.color = XMFLOAT4(color);
		center.texCoord = XMFLOAT2(0.5, 0.5);

		data.vertices.push_back(center);
		centerIndex = data.vertices.size() - 1;

		//Copy the top ring of vertices.
		vector<Vertex> topVertices;
		int base = lastVertex - verticesPerSlice + 1;

		for (int i = base; i < base + verticesPerSlice; i++)
		{
			topVertices.push_back(data.vertices[i]);
		}

		for (auto& x : topVertices)
		{
			XMFLOAT3 normal = XMFLOAT3(0, 1, 0);
			x.normal = normal;
			float u = x.position.x / height + 0.5;
			float v = x.position.z / height + 0.5;
			x.texCoord = XMFLOAT2(u, v);
		}

		data.vertices.insert(data.vertices.end(), topVertices.begin(), topVertices.end());

		for (int i = centerIndex + 1; i < centerIndex + verticesPerSlice; i++)
		{
			data.indices.push_back(i);
			data.indices.push_back(centerIndex);
			data.indices.push_back(i + 1);
		}

		return data;
	}
	
	MeshData MeshGenerator::CreateSphere(float radius, int numStacks, int numVerticalSections, const float* color)
	{
		using namespace DirectX;
		using std::cosf;
		using std::sinf;

		float dPhi = XM_PI / numStacks;
		float dTheta = XM_2PI / numVerticalSections;
		int numRings = numStacks - 1;
		int numVerticesPerRing = numVerticalSections + 1;

		MeshData data;

		//Build the vertices and indices, excluding the top and bottom stacks.

		float phi = dPhi;
		for (int i = 0; i < numStacks - 2; i++)
		{
			float y = radius * cosf(phi);

			//Repeating the first and last vertex in each stack,
			//because the texture coordinates differ.
			for (int j = 0; j <= numVerticalSections; j++)
			{
				float r = radius * sinf(phi);
				float theta = j * dTheta;
				float x = r * cosf(theta);
				float z = r * sinf(theta);

				Vertex vertex;
				vertex.position = XMFLOAT3(x, y, z);
				XMStoreFloat3(&vertex.normal, XMVector3Normalize(XMLoadFloat3(&vertex.position)));
				vertex.color = XMFLOAT4(color);
				vertex.texCoord = XMFLOAT2(theta / XM_2PI, phi / XM_PI);
				data.vertices.push_back(vertex);
			}

			phi += dPhi;
		}

		for (int i = 0; i < data.vertices.size() - numVerticesPerRing; i++)
		{
			//Add numVerticesPerRing to move to the vertex directly below
			data.indices.push_back(i);
			data.indices.push_back(i + 1);
			data.indices.push_back(i + numVerticesPerRing);
			data.indices.push_back(i + numVerticesPerRing);
			data.indices.push_back(i + 1);
			data.indices.push_back(i + 1 + numVerticesPerRing);
		}
		
		Vertex topCenter;
		topCenter.position = XMFLOAT3(0, radius, 0);
		topCenter.color = XMFLOAT4(color);
		topCenter.normal = XMFLOAT3(0, 1, 0);
		topCenter.texCoord = XMFLOAT2(0.0, 0.0);

		data.vertices.insert(data.vertices.begin(), topCenter);

		int centerIndex = 0;

		for (int i = centerIndex + 1; i < centerIndex + numVerticesPerRing; i++)
		{
			data.indices.push_back(i);
			data.indices.push_back(centerIndex);
			data.indices.push_back(i + 1);
		}

		int baseIndex = data.vertices.size() - numVerticesPerRing;
		
		Vertex bottomCenter;
		bottomCenter.position = XMFLOAT3(0, -radius, 0);
		bottomCenter.color = XMFLOAT4(color);
		bottomCenter.normal = XMFLOAT3(0, -1, 0);
		bottomCenter.texCoord = XMFLOAT2(0, 1);
		
		data.vertices.push_back(bottomCenter);

		centerIndex = data.vertices.size() - 1;

		for (int i = baseIndex; i < centerIndex + numVerticesPerRing; i++)
		{
			data.indices.push_back(i + 1);
			data.indices.push_back(centerIndex);
			data.indices.push_back(i);
		}

		return data;
	}
}