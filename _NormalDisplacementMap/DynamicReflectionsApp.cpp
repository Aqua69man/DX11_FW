#include "DynamicReflectionsApp.h"


DynamicReflectionsApp::DynamicReflectionsApp(HINSTANCE hInstance) :
	NormalDisplacementMapApp(hInstance),
	mDynamicCubeMapDSV(0),
	mDynamicCubeMapSRV(0)
{
	mMainWndCaption = L"Normal, Displacement, DynReflect Demo";

	BuildCubeFaceCamera(0.0f, 2.0f, 0.0f);
	for (int i = 0; i < 6; ++i)
	{
		mDynamicCubeMapRTV[i] = 0;
	}

	// Skull Mesh
	XMMATRIX skullScale = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	XMMATRIX skullOffset = XMMatrixTranslation(0.0f, 1.0f, 0.0f);
	XMStoreFloat4x4(&SkullMesh.mSkullWorld, XMMatrixMultiply(skullScale, skullOffset));
	// --
	SkullMesh.mSkullMat.Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	SkullMesh.mSkullMat.Diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	SkullMesh.mSkullMat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);
	SkullMesh.mSkullMat.Reflect = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
}


DynamicReflectionsApp::~DynamicReflectionsApp()
{
	NormalDisplacementMapApp::~NormalDisplacementMapApp();

	ReleaseCOM(mDynamicCubeMapDSV);
	ReleaseCOM(mDynamicCubeMapSRV);
	for (int i = 0; i < 6; ++i)
		ReleaseCOM(mDynamicCubeMapRTV[i]);
}

bool DynamicReflectionsApp::Init() 
{
	NormalDisplacementMapApp::Init();
	BuildDynamicCubeMapViews();

	// Skull Mesh
	SkullMesh.Init(md3dDevice);

	return true;
}


void DynamicReflectionsApp::BuildCubeFaceCamera(float x, float y, float z)
{
	// Generate the cube map about the given position.
	XMFLOAT3 center(x, y, z);
	XMFLOAT3 worldUp(0.0f, 1.0f, 0.0f);

	// Look along each coordinate axis.
	XMFLOAT3 targets[6] =
	{
		XMFLOAT3(x + 1.0f, y, z), // +X
		XMFLOAT3(x - 1.0f, y, z), // -X
		XMFLOAT3(x, y + 1.0f, z), // +Y
		XMFLOAT3(x, y - 1.0f, z), // -Y
		XMFLOAT3(x, y, z + 1.0f), // +Z
		XMFLOAT3(x, y, z - 1.0f)  // -Z
	};

	// Use world up vector (0,1,0) for all directions except +Y/-Y.  In these cases, we
	// are looking down +Y or -Y, so we need a different "up" vector.
	XMFLOAT3 ups[6] =
	{
		XMFLOAT3(0.0f, 1.0f, 0.0f),  // +X
		XMFLOAT3(0.0f, 1.0f, 0.0f),  // -X
		XMFLOAT3(0.0f, 0.0f, -1.0f), // +Y
		XMFLOAT3(0.0f, 0.0f, +1.0f), // -Y
		XMFLOAT3(0.0f, 1.0f, 0.0f),	 // +Z
		XMFLOAT3(0.0f, 1.0f, 0.0f)	 // -Z
	};

	for (int i = 0; i < 6; ++i)
	{
		mDynamicCubeMapCamera[i].LookAt(center, targets[i], ups[i]);
		mDynamicCubeMapCamera[i].SetLens(0.5f*XM_PI, 1.0f, 0.1f, 1000.0f);
		mDynamicCubeMapCamera[i].UpdateViewMatrix();
	}

}

void DynamicReflectionsApp::BuildDynamicCubeMapViews() {
	//
	// Cubemap is a special texture array with 6 elements.
	//

	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = DynamicCubeMapSize;
	texDesc.Height = DynamicCubeMapSize;
	texDesc.MipLevels = 0;
	texDesc.ArraySize = 6;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS | D3D11_RESOURCE_MISC_TEXTURECUBE;

	ID3D11Texture2D* cubeTex = 0;
	HR(md3dDevice->CreateTexture2D(&texDesc, 0, &cubeTex));

	//
	// Create a render target view to each cube map face 
	// (i.e., each element in the texture array).
	// 

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	rtvDesc.Format = texDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	rtvDesc.Texture2DArray.ArraySize = 1;
	rtvDesc.Texture2DArray.MipSlice = 0;

	for (int i = 0; i < 6; ++i)
	{
		rtvDesc.Texture2DArray.FirstArraySlice = i;
		HR(md3dDevice->CreateRenderTargetView(cubeTex, &rtvDesc, &mDynamicCubeMapRTV[i]));
	}

	//
	// Create a shader resource view to the cube map.
	//

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.TextureCube.MipLevels = -1;

	HR(md3dDevice->CreateShaderResourceView(cubeTex, &srvDesc, &mDynamicCubeMapSRV));

	ReleaseCOM(cubeTex);

	//
	// We need a depth texture for rendering the scene into the cubemap
	// that has the same resolution as the cubemap faces.  
	//

	D3D11_TEXTURE2D_DESC depthTexDesc;
	depthTexDesc.Width = DynamicCubeMapSize;
	depthTexDesc.Height = DynamicCubeMapSize;
	depthTexDesc.MipLevels = 1;
	depthTexDesc.ArraySize = 1;
	depthTexDesc.SampleDesc.Count = 1;
	depthTexDesc.SampleDesc.Quality = 0;
	depthTexDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
	depthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthTexDesc.CPUAccessFlags = 0;
	depthTexDesc.MiscFlags = 0;

	ID3D11Texture2D* depthTex = 0;
	HR(md3dDevice->CreateTexture2D(&depthTexDesc, 0, &depthTex));

	// Create the depth stencil view for the entire cube
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Format = depthTexDesc.Format;
	dsvDesc.Flags = 0;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	HR(md3dDevice->CreateDepthStencilView(depthTex, &dsvDesc, &mDynamicCubeMapDSV));

	ReleaseCOM(depthTex);

	//
	// Viewport for drawing into cubemap.
	// 

	mDynamicCubeMapViewport.TopLeftX = 0.0f;
	mDynamicCubeMapViewport.TopLeftY = 0.0f;
	mDynamicCubeMapViewport.Width = (float)DynamicCubeMapSize;
	mDynamicCubeMapViewport.Height = (float)DynamicCubeMapSize;
	mDynamicCubeMapViewport.MinDepth = 0.0f;
	mDynamicCubeMapViewport.MaxDepth = 1.0f;
}


void DynamicReflectionsApp::DrawScene(Camera& camera, bool drawCenterSkull)
{
	// Draw NormalDisplacementMapApp
	NormalDisplacementMapApp::DrawScene(camera);

	// ----------------------------------------------------------------------

	XMMATRIX view = camera.View();
	XMMATRIX proj = camera.Proj();
	XMMATRIX viewProj = camera.ViewProj();

	XMMATRIX world;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldViewProj;

	UINT stride = sizeof(Vertex::Basic32);
	UINT offset = 0;

	ID3DX11EffectTechnique* activeSkullTech = Effects::BasicFX->Light3ReflectTech;
	ID3DX11EffectTechnique* activeRotatingSkullTech = Effects::BasicFX->Light1Tech;

	md3dImmediateContext->RSSetState(0);

	md3dImmediateContext->IASetInputLayout(InputLayouts::Basic32);
	md3dImmediateContext->IASetVertexBuffers(0, 1, &SkullMesh.mSkullVB, &stride, &offset);
	md3dImmediateContext->IASetIndexBuffer(SkullMesh.mSkullIB, DXGI_FORMAT_R32_UINT, 0);

	// ------------------------ RotatingSkull ---------------------------
	//					 We can use Instancing for second skull
	// ------------------------------------------------------------------------

	//
	// Animate the skull around the center sphere.
	//
	XMMATRIX skullScale = XMMatrixScaling(0.2f, 0.2f, 0.2f);
	XMMATRIX skullOffset = XMMatrixTranslation(3.0f, 2.0f, 0.0f);
	XMMATRIX skullLocalRotate = XMMatrixRotationY(2.0f*mTimer.TotalTime());
	XMMATRIX skullGlobalRotate = XMMatrixRotationY(0.5f*mTimer.TotalTime());
	XMFLOAT4X4 rotatingSkullWorld;
	XMStoreFloat4x4(&rotatingSkullWorld, skullScale*skullLocalRotate*skullOffset*skullGlobalRotate);

	D3DX11_TECHNIQUE_DESC techDesc;
	activeRotatingSkullTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		world = XMLoadFloat4x4(&rotatingSkullWorld);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world*view*proj;

		Effects::BasicFX->SetWorld(world);
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BasicFX->SetWorldViewProj(worldViewProj);
		Effects::BasicFX->SetTexTransform(XMMatrixIdentity());
		Effects::BasicFX->SetMaterial(SkullMesh.mSkullMat);

		activeRotatingSkullTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(SkullMesh.mSkullIndexCount, 0, 0);
	}

	// --------------------------------------------------------------

	if (drawCenterSkull) {
		activeSkullTech->GetDesc(&techDesc);
		for (UINT p = 0; p < techDesc.Passes; ++p)
		{
			// Draw the skull.
			world = XMLoadFloat4x4(&SkullMesh.mSkullWorld);
			worldInvTranspose = MathHelper::InverseTranspose(world);
			worldViewProj = world*view*proj;

			Effects::BasicFX->SetWorld(world);
			Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::BasicFX->SetWorldViewProj(worldViewProj);
			Effects::BasicFX->SetTexTransform(XMMatrixIdentity());
			Effects::BasicFX->SetMaterial(SkullMesh.mSkullMat);
			Effects::BasicFX->SetDiffuseMap(SkullMesh.mSkullStoneTexSRV);     // !!!!!!!!!!!!!!! Check it with other activeTech
			Effects::BasicFX->SetCubeMap(mDynamicCubeMapSRV);  // TODO

			activeSkullTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
			md3dImmediateContext->DrawIndexed(SkullMesh.mSkullIndexCount, 0, 0);
		}
	}	
}


void DynamicReflectionsApp::DrawScene()
{
	ID3D11RenderTargetView* renderTargets[1];

	// Generate the cube map.
	md3dImmediateContext->RSSetViewports(1, &mDynamicCubeMapViewport);
	for (int i = 0; i < 6; ++i)
	{
		// Clear cube map face and depth buffer.
		md3dImmediateContext->ClearRenderTargetView(mDynamicCubeMapRTV[i], reinterpret_cast<const float*>(&Colors::Silver));
		md3dImmediateContext->ClearDepthStencilView(mDynamicCubeMapDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		// Bind cube map face as render target.
		renderTargets[0] = mDynamicCubeMapRTV[i];
		md3dImmediateContext->OMSetRenderTargets(1, renderTargets, mDynamicCubeMapDSV);

		// Draw the scene with the exception of the center sphere to this cube map face.
		DrawScene(mDynamicCubeMapCamera[i], false);
	}

	// Restore old viewport and render targets.
	md3dImmediateContext->RSSetViewports(1, &mScreenViewport);
	renderTargets[0] = mRenderTargetView;
	md3dImmediateContext->OMSetRenderTargets(1, renderTargets, mDepthStencilView);

	// Have hardware generate lower mipmap levels of cube map (for the next Draw call)
	md3dImmediateContext->GenerateMips(mDynamicCubeMapSRV);			// TODO

	// Now draw the scene as normal, but with the center skull.
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::Silver));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	DrawScene(mCam, true);
	HR(mSwapChain->Present(0, 0));

}