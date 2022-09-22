#include <vector>
#include <string>
#include <fstream>

#include "d3dUtil.h"
#include "Vertex.h"
#include "DXTK/DDSTextureLoader.h"  // FarUPD_WinSDK


#include "MeshLoader.h"



SkullAndShapes_MeshLoader::SkullAndShapes_MeshLoader() :
	mSkullVB(0), mSkullIB(0), mSkullIndexCount(0)
{
	// Skull texture SRV
	mSkullStoneTexSRV = nullptr;
}

SkullAndShapes_MeshLoader::~SkullAndShapes_MeshLoader()
{
	ReleaseCOM(mSkullVB);
	ReleaseCOM(mSkullIB);

	// Skull texture SRV
	ReleaseCOM(mSkullStoneTexSRV);
}

void SkullAndShapes_MeshLoader::Init(ID3D11Device * d3dDevice)
{
	CreateSkullTextureSRV(d3dDevice);
	BuildSkullGeometryBuffers(d3dDevice);
}

void SkullAndShapes_MeshLoader::CreateSkullTextureSRV(ID3D11Device * d3dDevice)
{
	HR(
		CreateDDSTextureFromFile(
			d3dDevice,				// ID3D11Device* d3dDevice,
			L"Textures/floor.dds",  // const wchar_t* szFileName,
			nullptr,				// ID3D11Resource** texture,
			&mSkullStoneTexSRV,			// ID3D11ShaderResourceView** textureView,
			0,						// size_t maxsize = 0,
			nullptr					// DDS_ALPHA_MODE* alphaMode = nullptr
		)
	);
}


void SkullAndShapes_MeshLoader::BuildSkullGeometryBuffers(ID3D11Device * d3dDevice)
{
	std::ifstream fin("Models/skull.txt");

	if (!fin)
	{
		MessageBox(0, L"Models/skull.txt not found.", 0, 0);
		return;
	}

	UINT vcount = 0;
	UINT tcount = 0;
	std::string ignore;

	fin >> ignore >> vcount;
	fin >> ignore >> tcount;
	fin >> ignore >> ignore >> ignore >> ignore;

	std::vector<Vertex::Basic32> vertices(vcount);
	for (UINT i = 0; i < vcount; ++i)
	{
		fin >> vertices[i].Pos.x >> vertices[i].Pos.y >> vertices[i].Pos.z;
		fin >> vertices[i].Normal.x >> vertices[i].Normal.y >> vertices[i].Normal.z;
	}

	fin >> ignore;
	fin >> ignore;
	fin >> ignore;

	mSkullIndexCount = 3 * tcount;
	std::vector<UINT> indices(mSkullIndexCount);
	for (UINT i = 0; i < tcount; ++i)
	{
		fin >> indices[i * 3 + 0] >> indices[i * 3 + 1] >> indices[i * 3 + 2];
	}

	fin.close();

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::Basic32) * vcount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	HR(d3dDevice->CreateBuffer(&vbd, &vinitData, &mSkullVB));

	//
	// Pack the indices of all the meshes into one index buffer.
	//

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * mSkullIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
	HR(d3dDevice->CreateBuffer(&ibd, &iinitData, &mSkullIB));
}