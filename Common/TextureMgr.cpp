#include "TextureMgr.h"
#include "DXTK/DDSTextureLoader.h"  // FarUPD_WinSDK

TextureMgr::TextureMgr() : md3dDevice(0)
{
}

TextureMgr::~TextureMgr()
{
	for(auto it = mTextureSRV.begin(); it != mTextureSRV.end(); ++it)
    {
		ReleaseCOM(it->second);
    }

	mTextureSRV.clear();
}

void TextureMgr::Init(ID3D11Device* device)
{
	md3dDevice = device;
}

ID3D11ShaderResourceView* TextureMgr::CreateTexture(std::wstring filename)
{
	ID3D11ShaderResourceView* srv = 0;

	// Does it already exist?
	if( mTextureSRV.find(filename) != mTextureSRV.end() )
	{
		srv = mTextureSRV[filename];
	}
	else
	{
		HR(
			CreateDDSTextureFromFile(
				md3dDevice,		   // ID3D11Device* d3dDevice,
				filename.c_str(),  // const wchar_t* szFileName,
				nullptr,		   // ID3D11Resource** texture,
				&srv,			   // ID3D11ShaderResourceView** textureView,
				0,				   // size_t maxsize = 0,
				nullptr			   // DDS_ALPHA_MODE* alphaMode = nullptr
			)
		);

		mTextureSRV[filename] = srv;
	}

	return srv;
}
 
