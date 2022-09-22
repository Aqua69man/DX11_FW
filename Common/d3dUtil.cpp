//***************************************************************************************
// d3dUtil.cpp by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#include "d3dUtil.h"
#include "DXTK/DDSTextureLoader.h"  // FarUPD_WinSDK


ID3D11ShaderResourceView* d3dHelper::CreateTexture2DArraySRV(
		ID3D11Device* device, ID3D11DeviceContext* context,
		std::vector<std::wstring>& filenames /*,
		DXGI_FORMAT format,
		UINT filter, 
		UINT mipFilter
		*/
		)
{
	//
	// Load the texture elements individually from file.  These textures
	// won't be used by the GPU (0 bind flags), they are just used to 
	// load the image data from file.  We use the STAGING usage so the
	// CPU can read the resource.
	//

	UINT size = (UINT)filenames.size();

	std::vector<ID3D11Texture2D*> srcTex(size);
	for(UINT i = 0; i < size; ++i)
	{
		// FarUPD_WinSDK
		/*
		D3DX11_IMAGE_LOAD_INFO loadInfo;

        loadInfo.Width  = D3DX11_FROM_FILE;
        loadInfo.Height = D3DX11_FROM_FILE;
        loadInfo.Depth  = D3DX11_FROM_FILE;
        loadInfo.FirstMipLevel = 0;
        loadInfo.MipLevels = D3DX11_FROM_FILE;
        loadInfo.Usage = D3D11_USAGE_STAGING;
        loadInfo.BindFlags = 0;
        loadInfo.CpuAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
        loadInfo.MiscFlags = 0;
        loadInfo.Format = format;
        loadInfo.Filter = filter;
        loadInfo.MipFilter = mipFilter;
		loadInfo.pSrcInfo  = 0;

        HR(D3DX11CreateTextureFromFile(device, filenames[i].c_str(), 
			&loadInfo, 0, (ID3D11Resource**)&srcTex[i], 0));
		*/

		// maxsize - is max mipmaps count. If maxsize = '0', then if the attempt to create the Direct3D 11 
		// resource fails and there are mipmaps present, it will retry assuming a maxsize based on the 
		// device's current feature level.
		// See https://github.com/Microsoft/DirectXTK/wiki/DDSTextureLoader#further-reading for parameters description.
		CreateDDSTextureFromFileEx(
			device,												// ID3D11Device* d3dDevice
			context,											// ID3D11DeviceContext* d3dContext
			filenames[i].c_str(),								// const wchar_t* szFileName
			0,													// size_t maxsize
			D3D11_USAGE_STAGING,								// D3D11_USAGE usage
			0,													// unsigned int bindFlags
			D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ,		// unsigned int cpuAccessFlags
			0,													// unsigned int miscFlags
			false,												// bool forceSRGB
			(ID3D11Resource**)&srcTex[i],						// ID3D11Resource** texture
			nullptr,											// ID3D11ShaderResourceView** textureView
			nullptr												// DDS_ALPHA_MODE* alphaMode = nullptr
		);

		// ~FarUPD_WinSDK
	}

	//
	// Create the texture array.  Each element in the texture 
	// array has the same format/dimensions.
	//

	D3D11_TEXTURE2D_DESC texElementDesc;
	srcTex[0]->GetDesc(&texElementDesc);

	D3D11_TEXTURE2D_DESC texArrayDesc;
	texArrayDesc.Width              = texElementDesc.Width;
	texArrayDesc.Height             = texElementDesc.Height;
	texArrayDesc.MipLevels          = texElementDesc.MipLevels;
	texArrayDesc.ArraySize          = size;
	texArrayDesc.Format             = texElementDesc.Format;
	texArrayDesc.SampleDesc.Count   = 1;
	texArrayDesc.SampleDesc.Quality = 0;
	texArrayDesc.Usage              = D3D11_USAGE_DEFAULT;
	texArrayDesc.BindFlags          = D3D11_BIND_SHADER_RESOURCE;
	texArrayDesc.CPUAccessFlags     = 0;
	texArrayDesc.MiscFlags          = 0;

	ID3D11Texture2D* texArray = 0;
	HR(device->CreateTexture2D( &texArrayDesc, 0, &texArray));

	//
	// Copy individual texture elements into texture array.
	//

	// for each texture element...
	for(UINT texElement = 0; texElement < size; ++texElement)
	{
		// for each mipmap level...
		for(UINT mipLevel = 0; mipLevel < texElementDesc.MipLevels; ++mipLevel)
		{
			D3D11_MAPPED_SUBRESOURCE mappedTex2D;
			HR(context->Map(srcTex[texElement], mipLevel, D3D11_MAP_READ, 0, &mappedTex2D));

			context->UpdateSubresource(texArray,
				D3D11CalcSubresource(mipLevel, texElement, texElementDesc.MipLevels),
				0, mappedTex2D.pData, mappedTex2D.RowPitch, mappedTex2D.DepthPitch);

			context->Unmap(srcTex[texElement], mipLevel);
		}
	}	

	//
	// Create a resource view to the texture array.
	//
	
	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = texArrayDesc.Format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	viewDesc.Texture2DArray.MostDetailedMip = 0;
	viewDesc.Texture2DArray.MipLevels = texArrayDesc.MipLevels;
	viewDesc.Texture2DArray.FirstArraySlice = 0;
	viewDesc.Texture2DArray.ArraySize = size;

	ID3D11ShaderResourceView* texArraySRV = 0;
	HR(device->CreateShaderResourceView(texArray, &viewDesc, &texArraySRV));

	//
	// Cleanup--we only need the resource view.
	//

	ReleaseCOM(texArray);

	for(UINT i = 0; i < size; ++i)
		ReleaseCOM(srcTex[i]);

	return texArraySRV;
}

ID3D11ShaderResourceView* d3dHelper::CreateRandomTexture1DSRV(ID3D11Device* device)
{
	// 
	// Create the random data.
	//
	XMFLOAT4 randomValues[1024];

	for(int i = 0; i < 1024; ++i)
	{
		randomValues[i].x = MathHelper::RandF(-1.0f, 1.0f);
		randomValues[i].y = MathHelper::RandF(-1.0f, 1.0f);
		randomValues[i].z = MathHelper::RandF(-1.0f, 1.0f);
		randomValues[i].w = MathHelper::RandF(-1.0f, 1.0f);
	}

    D3D11_SUBRESOURCE_DATA initData;
    initData.pSysMem = randomValues;
	initData.SysMemPitch = 1024*sizeof(XMFLOAT4);
    initData.SysMemSlicePitch = 0;

	//
	// Create the texture.
	//
    D3D11_TEXTURE1D_DESC texDesc;
    texDesc.Width = 1024;
    texDesc.MipLevels = 1;
    texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    texDesc.Usage = D3D11_USAGE_IMMUTABLE;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    texDesc.CPUAccessFlags = 0;
    texDesc.MiscFlags = 0;
    texDesc.ArraySize = 1;

	ID3D11Texture1D* randomTex = 0;
    HR(device->CreateTexture1D(&texDesc, &initData, &randomTex));

	//
	// Create the resource view.
	//
    D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = texDesc.Format;
    viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
    viewDesc.Texture1D.MipLevels = texDesc.MipLevels;
	viewDesc.Texture1D.MostDetailedMip = 0;
	
	ID3D11ShaderResourceView* randomTexSRV = 0;
    HR(device->CreateShaderResourceView(randomTex, &viewDesc, &randomTexSRV));

	ReleaseCOM(randomTex);

	return randomTexSRV;
}

void ExtractFrustumPlanes(XMFLOAT4 planes[6], CXMMATRIX M)
{
	XMVECTOR r0 = M.r[0];
	XMVECTOR r1 = M.r[1];
	XMVECTOR r2 = M.r[2];
	XMVECTOR r3 = M.r[3];

	float m00, m01, m02, m03,
		  m10, m11, m12, m13,
		  m20, m21, m22, m23,
		  m30, m31, m32, m33;

	m00 = XMVectorGetX(r0) ;
	m01 = XMVectorGetY(r0) ;
	m02 = XMVectorGetZ(r0) ;
	m03 = XMVectorGetW(r0) ;
	m10 = XMVectorGetX(r1) ;
	m11 = XMVectorGetY(r1) ;
	m12 = XMVectorGetZ(r1) ;
	m13 = XMVectorGetW(r1) ;
	m20 = XMVectorGetX(r2) ;
	m21 = XMVectorGetY(r2) ;
	m22 = XMVectorGetZ(r2) ;
	m23 = XMVectorGetW(r2) ;
	m30 = XMVectorGetX(r3) ;
	m31 = XMVectorGetY(r3) ;
	m32 = XMVectorGetZ(r3) ;
	m33 = XMVectorGetW(r3) ;

	//
	// Left
	//
	//planes[0].x = M(0,3) + M(0,0);
	//planes[0].y = M(1,3) + M(1,0);
	//planes[0].z = M(2,3) + M(2,0);
	//planes[0].w = M(3,3) + M(3,0);
	planes[0].x = m03 + m00;
	planes[0].y = m13 + m10;
	planes[0].z = m23 + m20;
	planes[0].w = m33 + m30;

	//
	// Right
	//
	//planes[1].x = M(0,3) - M(0,0);
	//planes[1].y = M(1,3) - M(1,0);
	//planes[1].z = M(2,3) - M(2,0);
	//planes[1].w = M(3,3) - M(3,0);
	planes[1].x = m03 - m00;
	planes[1].y = m13 - m10;
	planes[1].z = m23 - m20;
	planes[1].w = m33 - m30;

	//
	// Bottom
	//
	//planes[2].x = M(0,3) + M(0,1);
	//planes[2].y = M(1,3) + M(1,1);
	//planes[2].z = M(2,3) + M(2,1);
	//planes[2].w = M(3,3) + M(3,1);
	planes[1].x = m03 - m00;
	planes[1].y = m13 - m10;
	planes[1].z = m23 - m20;
	planes[1].w = m33 - m30;

	//
	// Bottom
	//
	//planes[2].x = M03 + M01;
	//planes[2].y = M13 + M11;
	//planes[2].z = M23 + M21;
	//planes[2].w = M33 + M31;
	planes[2].x = m03 + m01;
	planes[2].y = m13 + m11;
	planes[2].z = m23 + m21;
	planes[2].w = m33 + m31;

	//
	// Top
	//
	//planes[3].x = M(0,3) - M(0,1);
	//planes[3].y = M(1,3) - M(1,1);
	//planes[3].z = M(2,3) - M(2,1);
	//planes[3].w = M(3,3) - M(3,1);
	planes[3].x = m03 - m01;
	planes[3].y = m13 - m11;
	planes[3].z = m23 - m21;
	planes[3].w = m33 - m31;

	//
	// Near
	//
	//planes[4].x = M(0,2);
	//planes[4].y = M(1,2);
	//planes[4].z = M(2,2);
	//planes[4].w = M(3,2);
	planes[4].x = m02;
	planes[4].y = m12;
	planes[4].z = m22;
	planes[4].w = m32;

	//
	// Far
	//
	//planes[5].x = M(0,3) - M(0,2);
	//planes[5].y = M(1,3) - M(1,2);
	//planes[5].z = M(2,3) - M(2,2);
	//planes[5].w = M(3,3) - M(3,2);
	planes[5].x = m03 - m02;
	planes[5].y = m13 - m12;
	planes[5].z = m23 - m22;
	planes[5].w = m33 - m32;

	// Normalize the plane equations.
	for(int i = 0; i < 6; ++i)
	{
		XMVECTOR v = XMPlaneNormalize(XMLoadFloat4(&planes[i]));
		XMStoreFloat4(&planes[i], v);
	}
}