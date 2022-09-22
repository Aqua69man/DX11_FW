//***************************************************************************************
//
// Demonstrates normal mapping and displacement mapping.
//
// Controls:
//		Hold the left mouse button down and move the mouse to rotate.
//      Hold the right mouse button down to zoom in and out.
//      Press '1' for wireframe
//      Press '2' for BasicFX
//      Press '3' for Normal mapping
//      Press '4' for Displacement mapping
//
//***************************************************************************************

#include "NormalDisplacementMapApp.h"


// ====================================================================================================
// ====================================================================================================
//		!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//		!!!!! DRAW SKULL wish MeshLoader and Offset the Skull in DynReflect demo to the front !!!!
//		!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// ====================================================================================================
// ====================================================================================================



NormalDisplacementMapApp::NormalDisplacementMapApp(HINSTANCE hInstance) : 
  D3DApp(hInstance), mSky(0), 
  mShapesVB(0), mShapesIB(0), /*mSkullVB(0), mSkullIB(0), */
  mStoneTexSRV(0), mBrickTexSRV(0),
  mStoneNormalTexSRV(0), mBrickNormalTexSRV(0), 
  // -------- DynamicReflection ---------
  // mDynamicCubeMapDSV(0), mDynamicCubeMapSRV(0),
  // ------------------------------------------
  /*mSkullIndexCount(0),*/  mRenderOptions(RenderOptionsNormalMap)
{
	mMainWndCaption = L"Normal- Displacement Map Demo";
	
	mLastMousePos.x = 0;
	mLastMousePos.y = 0;

	mCam.SetPosition(0.0f, 2.0f, -15.0f);

	// -------- DynamicReflection --------
	//BuildCubeFaceCamera(0.0f, 2.0f, 0.0f);
	//for (int i = 0; i < 6; ++i)
	//{
	//	mDynamicCubeMapRTV[i] = 0;
	//}
	// -----------------------------------------

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&mGridWorld, I);

	XMMATRIX boxScale = XMMatrixScaling(3.0f, 1.0f, 3.0f);
	XMMATRIX boxOffset = XMMatrixTranslation(0.0f, 0.5f, 0.0f);
	XMStoreFloat4x4(&mBoxWorld, XMMatrixMultiply(boxScale, boxOffset));

	//XMMATRIX skullScale = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	//XMMATRIX skullOffset = XMMatrixTranslation(0.0f, 1.0f, 0.0f);
	//XMStoreFloat4x4(&mSkullWorld, XMMatrixMultiply(skullScale, skullOffset));

	for(int i = 0; i < 5; ++i)
	{
		XMStoreFloat4x4(&mCylWorld[i*2+0], XMMatrixTranslation(-5.0f, 1.5f, -10.0f + i*5.0f));
		XMStoreFloat4x4(&mCylWorld[i*2+1], XMMatrixTranslation(+5.0f, 1.5f, -10.0f + i*5.0f));

		XMStoreFloat4x4(&mSphereWorld[i*2+0], XMMatrixTranslation(-5.0f, 3.5f, -10.0f + i*5.0f));
		XMStoreFloat4x4(&mSphereWorld[i*2+1], XMMatrixTranslation(+5.0f, 3.5f, -10.0f + i*5.0f));
	}

	mDirLights[0].Ambient  = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[0].Diffuse  = XMFLOAT4(0.7f, 0.7f, 0.6f, 1.0f);
	mDirLights[0].Specular = XMFLOAT4(0.8f, 0.8f, 0.7f, 1.0f);
	mDirLights[0].Direction = XMFLOAT3(0.707f, 0.0f, 0.707f);

	mDirLights[1].Ambient  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[1].Diffuse  = XMFLOAT4(0.40f, 0.40f, 0.40f, 1.0f);
	mDirLights[1].Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[1].Direction = XMFLOAT3(0.0f, -0.707f, 0.707f);

	mDirLights[2].Ambient  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[2].Diffuse  = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[2].Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[2].Direction = XMFLOAT3(-0.57735f, -0.57735f, -0.57735f);

	mGridMat.Ambient  = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	mGridMat.Diffuse  = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mGridMat.Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 16.0f);
	mGridMat.Reflect  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	mCylinderMat.Ambient  = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mCylinderMat.Diffuse  = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mCylinderMat.Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 32.0f);
	mCylinderMat.Reflect  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	mSphereMat.Ambient  = XMFLOAT4(0.2f, 0.3f, 0.4f, 1.0f);
	mSphereMat.Diffuse  = XMFLOAT4(0.2f, 0.3f, 0.4f, 1.0f);
	mSphereMat.Specular = XMFLOAT4(0.9f, 0.9f, 0.9f, 16.0f);
	mSphereMat.Reflect  = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);

	mBoxMat.Ambient  = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mBoxMat.Diffuse  = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mBoxMat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);
	mBoxMat.Reflect  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	//mSkullMat.Ambient  = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	//mSkullMat.Diffuse  = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	//mSkullMat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);
	//mSkullMat.Reflect  = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
}

NormalDisplacementMapApp::~NormalDisplacementMapApp()
{
	SafeDelete(mSky);

	ReleaseCOM(mShapesVB);
	ReleaseCOM(mShapesIB);
	//ReleaseCOM(mSkullVB);
	//ReleaseCOM(mSkullIB);
	ReleaseCOM(mStoneTexSRV);
	ReleaseCOM(mBrickTexSRV);
	ReleaseCOM(mStoneNormalTexSRV);
	ReleaseCOM(mBrickNormalTexSRV);

	// -------- DynamicReflection --------
	//ReleaseCOM(mDynamicCubeMapDSV);
	//ReleaseCOM(mDynamicCubeMapSRV);
	//for (int i = 0; i < 6; ++i)
	//	ReleaseCOM(mDynamicCubeMapRTV[i]);
	// -----------------------------------------

	Effects::DestroyAll();
	InputLayouts::DestroyAll(); 
	RenderStates::DestroyAll();
}

bool NormalDisplacementMapApp::Init()
{
	if(!D3DApp::Init())
		return false;

	// Must init Effects first since InputLayouts depend on shader signatures.
	Effects::InitAll(md3dDevice);
	InputLayouts::InitAll(md3dDevice);
	RenderStates::InitAll(md3dDevice);

	mSky = new Sky(md3dDevice, L"Textures/snowcube1024.dds", 5000.0f);

	// FarUPD_WinSDK
	//HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, 
	//	L"Textures/floor.dds", 0, 0, &mStoneTexSRV, 0 ));

	//HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, 
	//	L"Textures/bricks.dds", 0, 0, &mBrickTexSRV, 0 ));

	//HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, 
	//	L"Textures/floor_nmap.dds", 0, 0, &mStoneNormalTexSRV, 0 ));

	//HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, 
	//	L"Textures/bricks_nmap.dds", 0, 0, &mBrickNormalTexSRV, 0 ));


	HR(
		CreateDDSTextureFromFile(
			md3dDevice,				// ID3D11Device* d3dDevice,
			L"Textures/floor.dds",  // const wchar_t* szFileName,
			nullptr,				// ID3D11Resource** texture,
			&mStoneTexSRV,			// ID3D11ShaderResourceView** textureView,
			0,						// size_t maxsize = 0,
			nullptr					// DDS_ALPHA_MODE* alphaMode = nullptr
		)
	);

	HR(
		CreateDDSTextureFromFile(
			md3dDevice,				// ID3D11Device* d3dDevice,
			L"Textures/bricks.dds", // const wchar_t* szFileName,
			nullptr,				// ID3D11Resource** texture,
			&mBrickTexSRV,			// ID3D11ShaderResourceView** textureView,
			0,						// size_t maxsize = 0,
			nullptr					// DDS_ALPHA_MODE* alphaMode = nullptr
		)
	);

	HR(
		CreateDDSTextureFromFile(
			md3dDevice,					// ID3D11Device* d3dDevice,
			L"Textures/floor_nmap.dds", // const wchar_t* szFileName,
			nullptr,					// ID3D11Resource** texture,
			&mStoneNormalTexSRV,		// ID3D11ShaderResourceView** textureView,
			0,							// size_t maxsize = 0,
			nullptr						// DDS_ALPHA_MODE* alphaMode = nullptr
		)
	);

	HR(
		CreateDDSTextureFromFile(
			md3dDevice,						// ID3D11Device* d3dDevice,
			L"Textures/bricks_nmap.dds",	// const wchar_t* szFileName,
			nullptr,						// ID3D11Resource** texture,
			&mBrickNormalTexSRV,			// ID3D11ShaderResourceView** textureView,
			0,								// size_t maxsize = 0,
			nullptr							// DDS_ALPHA_MODE* alphaMode = nullptr
		)
	);
	// ~FarUPD_WinSDK


	// -------- DynamicReflection --------
	//BuildDynamicCubeMapViews();
	// -----------------------------------------

	BuildShapeGeometryBuffers();
	//BuildSkullGeometryBuffers();

	return true;
}

void NormalDisplacementMapApp::OnResize()
{
	D3DApp::OnResize();

	mCam.SetLens(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
}

void NormalDisplacementMapApp::UpdateScene(float dt)
{
	//
	// Control the camera.
	//
	if( GetAsyncKeyState('W') & 0x8000 )
		mCam.Walk(10.0f*dt);

	if( GetAsyncKeyState('S') & 0x8000 )
		mCam.Walk(-10.0f*dt);

	if( GetAsyncKeyState('A') & 0x8000 )
		mCam.Strafe(-10.0f*dt);

	if( GetAsyncKeyState('D') & 0x8000 )
		mCam.Strafe(10.0f*dt);

	//
	// Switch the rendering effect based on key presses.
	//
	if( GetAsyncKeyState('2') & 0x8000 )
		mRenderOptions = RenderOptionsBasic; 

	if( GetAsyncKeyState('3') & 0x8000 )
		mRenderOptions = RenderOptionsNormalMap; 

	if( GetAsyncKeyState('4') & 0x8000 )
		mRenderOptions = RenderOptionsDisplacementMap; 

}


void NormalDisplacementMapApp::DrawScene(Camera& camera) {
	camera.UpdateViewMatrix();

	XMMATRIX view = camera.View();
	XMMATRIX proj = camera.Proj();
	XMMATRIX viewProj = camera.ViewProj();


	// Set per frame constants.
	Effects::BasicFX->SetDirLights(mDirLights);
	Effects::BasicFX->SetEyePosW(camera.GetPosition());
	Effects::BasicFX->SetCubeMap(mSky->CubeMapSRV());

	Effects::NormalMapFX->SetDirLights(mDirLights);
	Effects::NormalMapFX->SetEyePosW(camera.GetPosition());
	Effects::NormalMapFX->SetCubeMap(mSky->CubeMapSRV());

	Effects::DisplacementMapFX->SetDirLights(mDirLights);
	Effects::DisplacementMapFX->SetEyePosW(camera.GetPosition());
	Effects::DisplacementMapFX->SetCubeMap(mSky->CubeMapSRV());

	// These properties could be set per object if needed.
	Effects::DisplacementMapFX->SetHeightScale(0.07f);
	Effects::DisplacementMapFX->SetMaxTessDistance(1.0f);
	Effects::DisplacementMapFX->SetMinTessDistance(25.0f);
	Effects::DisplacementMapFX->SetMinTessFactor(1.0f);
	Effects::DisplacementMapFX->SetMaxTessFactor(5.0f);

	// Figure out which technique to use for different geometry.

	ID3DX11EffectTechnique* activeTech = Effects::DisplacementMapFX->Light3TexTech;
	ID3DX11EffectTechnique* activeSphereTech = Effects::BasicFX->Light3ReflectTech;
	//ID3DX11EffectTechnique* activeSkullTech = Effects::BasicFX->Light3ReflectTech;
	// -------------------------- RotatingSkull ------------------------------------
	//ID3DX11EffectTechnique* activeRotatingSkullTech = Effects::BasicFX->Light1Tech;
	// -------------------------------------------------------------------------

	switch (mRenderOptions)
	{
	case RenderOptionsBasic:
		activeTech = Effects::BasicFX->Light3TexTech;
		md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		break;
	case RenderOptionsNormalMap:
		activeTech = Effects::NormalMapFX->Light3TexTech;
		md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		break;
	case RenderOptionsDisplacementMap:
		activeTech = Effects::DisplacementMapFX->Light3TexTech;
		md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
		break;
	}

	XMMATRIX world;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldViewProj;

	//
	// Draw the grid, cylinders, and box without any cubemap reflection.
	// 

	UINT stride = sizeof(Vertex::PosNormalTexTan);
	UINT offset = 0;

	md3dImmediateContext->IASetInputLayout(InputLayouts::PosNormalTexTan);
	md3dImmediateContext->IASetVertexBuffers(0, 1, &mShapesVB, &stride, &offset);
	md3dImmediateContext->IASetIndexBuffer(mShapesIB, DXGI_FORMAT_R32_UINT, 0);

	if (GetAsyncKeyState('1') & 0x8000)
		md3dImmediateContext->RSSetState(RenderStates::WireframeRS);

	D3DX11_TECHNIQUE_DESC techDesc;
	activeTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		// Draw the grid.
		world = XMLoadFloat4x4(&mGridWorld);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world*view*proj;

		switch (mRenderOptions)
		{
		case RenderOptionsBasic:
			Effects::BasicFX->SetWorld(world);
			Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::BasicFX->SetWorldViewProj(worldViewProj);
			Effects::BasicFX->SetTexTransform(XMMatrixScaling(8.0f, 10.0f, 1.0f));
			Effects::BasicFX->SetMaterial(mGridMat);
			Effects::BasicFX->SetDiffuseMap(mStoneTexSRV);
			break;
		case RenderOptionsNormalMap:
			Effects::NormalMapFX->SetWorld(world);
			Effects::NormalMapFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::NormalMapFX->SetWorldViewProj(worldViewProj);
			Effects::NormalMapFX->SetTexTransform(XMMatrixScaling(8.0f, 10.0f, 1.0f));
			Effects::NormalMapFX->SetMaterial(mGridMat);
			Effects::NormalMapFX->SetDiffuseMap(mStoneTexSRV);
			Effects::NormalMapFX->SetNormalMap(mStoneNormalTexSRV);
			break;
		case RenderOptionsDisplacementMap:
			Effects::DisplacementMapFX->SetWorld(world);
			Effects::DisplacementMapFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::DisplacementMapFX->SetViewProj(viewProj);
			Effects::DisplacementMapFX->SetWorldViewProj(worldViewProj);
			Effects::DisplacementMapFX->SetTexTransform(XMMatrixScaling(8.0f, 10.0f, 1.0f));
			Effects::DisplacementMapFX->SetMaterial(mGridMat);
			Effects::DisplacementMapFX->SetDiffuseMap(mStoneTexSRV);
			Effects::DisplacementMapFX->SetNormalMap(mStoneNormalTexSRV);
			break;
		}

		activeTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(mGridIndexCount, mGridIndexOffset, mGridVertexOffset);

		// Draw the box.
		world = XMLoadFloat4x4(&mBoxWorld);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world*view*proj;

		switch (mRenderOptions)
		{
		case RenderOptionsBasic:
			Effects::BasicFX->SetWorld(world);
			Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::BasicFX->SetWorldViewProj(worldViewProj);
			Effects::BasicFX->SetTexTransform(XMMatrixScaling(2.0f, 1.0f, 1.0f));
			Effects::BasicFX->SetMaterial(mBoxMat);
			Effects::BasicFX->SetDiffuseMap(mBrickTexSRV);
			break;
		case RenderOptionsNormalMap:
			Effects::NormalMapFX->SetWorld(world);
			Effects::NormalMapFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::NormalMapFX->SetWorldViewProj(worldViewProj);
			Effects::NormalMapFX->SetTexTransform(XMMatrixScaling(2.0f, 1.0f, 1.0f));
			Effects::NormalMapFX->SetMaterial(mBoxMat);
			Effects::NormalMapFX->SetDiffuseMap(mBrickTexSRV);
			Effects::NormalMapFX->SetNormalMap(mBrickNormalTexSRV);
			break;
		case RenderOptionsDisplacementMap:
			Effects::DisplacementMapFX->SetWorld(world);
			Effects::DisplacementMapFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::DisplacementMapFX->SetViewProj(viewProj);
			Effects::DisplacementMapFX->SetWorldViewProj(worldViewProj);
			Effects::DisplacementMapFX->SetTexTransform(XMMatrixScaling(2.0f, 1.0f, 1.0f));
			Effects::DisplacementMapFX->SetMaterial(mBoxMat);
			Effects::DisplacementMapFX->SetDiffuseMap(mBrickTexSRV);
			Effects::DisplacementMapFX->SetNormalMap(mBrickNormalTexSRV);
			break;
		}

		activeTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(mBoxIndexCount, mBoxIndexOffset, mBoxVertexOffset);

		// Draw the cylinders.
		for (int i = 0; i < 10; ++i)
		{
			world = XMLoadFloat4x4(&mCylWorld[i]);
			worldInvTranspose = MathHelper::InverseTranspose(world);
			worldViewProj = world*view*proj;

			switch (mRenderOptions)
			{
			case RenderOptionsBasic:
				Effects::BasicFX->SetWorld(world);
				Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
				Effects::BasicFX->SetWorldViewProj(worldViewProj);
				Effects::BasicFX->SetTexTransform(XMMatrixScaling(1.0f, 2.0f, 1.0f));
				Effects::BasicFX->SetMaterial(mCylinderMat);
				Effects::BasicFX->SetDiffuseMap(mBrickTexSRV);
				break;
			case RenderOptionsNormalMap:
				Effects::NormalMapFX->SetWorld(world);
				Effects::NormalMapFX->SetWorldInvTranspose(worldInvTranspose);
				Effects::NormalMapFX->SetWorldViewProj(worldViewProj);
				Effects::NormalMapFX->SetTexTransform(XMMatrixScaling(1.0f, 2.0f, 1.0f));
				Effects::NormalMapFX->SetMaterial(mCylinderMat);
				Effects::NormalMapFX->SetDiffuseMap(mBrickTexSRV);
				Effects::NormalMapFX->SetNormalMap(mBrickNormalTexSRV);
				break;
			case RenderOptionsDisplacementMap:
				Effects::DisplacementMapFX->SetWorld(world);
				Effects::DisplacementMapFX->SetWorldInvTranspose(worldInvTranspose);
				Effects::DisplacementMapFX->SetViewProj(viewProj);
				Effects::DisplacementMapFX->SetWorldViewProj(worldViewProj);
				Effects::DisplacementMapFX->SetTexTransform(XMMatrixScaling(1.0f, 2.0f, 1.0f));
				Effects::DisplacementMapFX->SetMaterial(mCylinderMat);
				Effects::DisplacementMapFX->SetDiffuseMap(mBrickTexSRV);
				Effects::DisplacementMapFX->SetNormalMap(mBrickNormalTexSRV);
				break;
			}


			activeTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
			md3dImmediateContext->DrawIndexed(mCylinderIndexCount, mCylinderIndexOffset, mCylinderVertexOffset);
		}
	}

	// FX sets tessellation stages, but it does not disable them.  So do that here
	// to turn off tessellation.
	md3dImmediateContext->HSSetShader(0, 0, 0);
	md3dImmediateContext->DSSetShader(0, 0, 0);

	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//
	// Draw the spheres with cubemap reflection.
	//

	activeSphereTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		// Draw the spheres.
		for (int i = 0; i < 10; ++i)
		{
			world = XMLoadFloat4x4(&mSphereWorld[i]);
			worldInvTranspose = MathHelper::InverseTranspose(world);
			worldViewProj = world*view*proj;

			Effects::BasicFX->SetWorld(world);
			Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::BasicFX->SetWorldViewProj(worldViewProj);
			Effects::BasicFX->SetTexTransform(XMMatrixIdentity());
			Effects::BasicFX->SetMaterial(mSphereMat);

			activeSphereTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
			md3dImmediateContext->DrawIndexed(mSphereIndexCount, mSphereIndexOffset, mSphereVertexOffset);
		}
	}

	//// Set states for skull
	//stride = sizeof(Vertex::Basic32);
	//offset = 0;

	//md3dImmediateContext->RSSetState(0);

	//md3dImmediateContext->IASetInputLayout(InputLayouts::Basic32);
	//md3dImmediateContext->IASetVertexBuffers(0, 1, &mSkullVB, &stride, &offset);
	//md3dImmediateContext->IASetIndexBuffer(mSkullIB, DXGI_FORMAT_R32_UINT, 0);

	//// ------------------------ RotatingSkull ---------------------------
	////					 We can use Instancing for second skull
	//// ------------------------------------------------------------------------

	////
	//// Animate the skull around the center sphere.
	////
	//XMMATRIX skullScale = XMMatrixScaling(0.2f, 0.2f, 0.2f);
	//XMMATRIX skullOffset = XMMatrixTranslation(3.0f, 2.0f, 0.0f);
	//XMMATRIX skullLocalRotate = XMMatrixRotationY(2.0f*mTimer.TotalTime());
	//XMMATRIX skullGlobalRotate = XMMatrixRotationY(0.5f*mTimer.TotalTime());
	//XMFLOAT4X4 rotatingSkullWorld;
	//XMStoreFloat4x4(&rotatingSkullWorld, skullScale*skullLocalRotate*skullOffset*skullGlobalRotate);

	//activeRotatingSkullTech->GetDesc(&techDesc);
	//for (UINT p = 0; p < techDesc.Passes; ++p)
	//{
	//	world = XMLoadFloat4x4(&rotatingSkullWorld);
	//	worldInvTranspose = MathHelper::InverseTranspose(world);
	//	worldViewProj = world*view*proj;

	//	Effects::BasicFX->SetWorld(world);
	//	Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
	//	Effects::BasicFX->SetWorldViewProj(worldViewProj);
	//	Effects::BasicFX->SetTexTransform(XMMatrixIdentity());
	//	Effects::BasicFX->SetMaterial(mSkullMat);

	//	activeRotatingSkullTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
	//	md3dImmediateContext->DrawIndexed(mSkullIndexCount, 0, 0);
	//}
	//// --------------------------------------------------------------
	//// --------------------------------------------------------------

	//if (drawCenterSkull) {
	//	activeSkullTech->GetDesc(&techDesc);
	//	for (UINT p = 0; p < techDesc.Passes; ++p)
	//	{
	//		// Draw the skull.
	//		world = XMLoadFloat4x4(&mSkullWorld);
	//		worldInvTranspose = MathHelper::InverseTranspose(world);
	//		worldViewProj = world*view*proj;

	//		Effects::BasicFX->SetWorld(world);
	//		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
	//		Effects::BasicFX->SetWorldViewProj(worldViewProj);
	//		Effects::BasicFX->SetTexTransform(XMMatrixIdentity());
	//		Effects::BasicFX->SetMaterial(mSkullMat);
	//		Effects::BasicFX->SetDiffuseMap(mStoneTexSRV);     // !!!!!!!!!!!!!!! Check it with other activeTech
	//		Effects::BasicFX->SetCubeMap(mDynamicCubeMapSRV);			// TODO

	//		activeSkullTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
	//		md3dImmediateContext->DrawIndexed(mSkullIndexCount, 0, 0);
	//	}
	//}

	mSky->Draw(md3dImmediateContext, camera);

	// restore default states, as the SkyFX changes them in the effect file.
	md3dImmediateContext->RSSetState(0);
	md3dImmediateContext->OMSetDepthStencilState(0, 0);
}

void NormalDisplacementMapApp::DrawScene()
{
	ID3D11RenderTargetView* renderTargets[1];

	//// Generate the cube map.
	//md3dImmediateContext->RSSetViewports(1, &mDynamicCubeMapViewport);
	//for (int i = 0; i < 6; ++i)
	//{
	//	// Clear cube map face and depth buffer.
	//	md3dImmediateContext->ClearRenderTargetView(mDynamicCubeMapRTV[i], reinterpret_cast<const float*>(&Colors::Silver));
	//	md3dImmediateContext->ClearDepthStencilView(mDynamicCubeMapDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//	// Bind cube map face as render target.
	//	renderTargets[0] = mDynamicCubeMapRTV[i];
	//	md3dImmediateContext->OMSetRenderTargets(1, renderTargets, mDynamicCubeMapDSV);

	//	// Draw the scene with the exception of the center sphere to this cube map face.
	//	DrawScene(mDynamicCubeMapCamera[i], false);
	//}

	renderTargets[0] = mRenderTargetView;

	// Restore old viewport and render targets.
	md3dImmediateContext->RSSetViewports(1, &mScreenViewport);
	md3dImmediateContext->OMSetRenderTargets(1, renderTargets, mDepthStencilView);

	// Now draw the scene as normal, but with the center skull.
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::Silver));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	DrawScene(mCam);
	HR(mSwapChain->Present(0, 0));
}


// ----------------------- DynamicReflection -------------------------
//void NormalDisplacementMapApp::BuildCubeFaceCamera(float x, float y, float z)
//{
//	// Generate the cube map about the given position.
//	XMFLOAT3 center(x, y, z);
//	XMFLOAT3 worldUp(0.0f, 1.0f, 0.0f);
//
//	// Look along each coordinate axis.
//	XMFLOAT3 targets[6] =
//	{
//		XMFLOAT3(x + 1.0f, y, z), // +X
//		XMFLOAT3(x - 1.0f, y, z), // -X
//		XMFLOAT3(x, y + 1.0f, z), // +Y
//		XMFLOAT3(x, y - 1.0f, z), // -Y
//		XMFLOAT3(x, y, z + 1.0f), // +Z
//		XMFLOAT3(x, y, z - 1.0f)  // -Z
//	};
//
//	// Use world up vector (0,1,0) for all directions except +Y/-Y.  In these cases, we
//	// are looking down +Y or -Y, so we need a different "up" vector.
//	XMFLOAT3 ups[6] =
//	{
//		XMFLOAT3(0.0f, 1.0f, 0.0f),  // +X
//		XMFLOAT3(0.0f, 1.0f, 0.0f),  // -X
//		XMFLOAT3(0.0f, 0.0f, -1.0f), // +Y
//		XMFLOAT3(0.0f, 0.0f, +1.0f), // -Y
//		XMFLOAT3(0.0f, 1.0f, 0.0f),	 // +Z
//		XMFLOAT3(0.0f, 1.0f, 0.0f)	 // -Z
//	};
//
//	for (int i = 0; i < 6; ++i)
//	{
//		mDynamicCubeMapCamera[i].LookAt(center, targets[i], ups[i]);
//		mDynamicCubeMapCamera[i].SetLens(0.5f*XM_PI, 1.0f, 0.1f, 1000.0f);
//		mDynamicCubeMapCamera[i].UpdateViewMatrix();
//	}
//
//}
//
//void NormalDisplacementMapApp::BuildDynamicCubeMapViews() {
//	//
//	// Cubemap is a special texture array with 6 elements.
//	//
//
//	D3D11_TEXTURE2D_DESC texDesc;
//	texDesc.Width = DynamicCubeMapSize;
//	texDesc.Height = DynamicCubeMapSize;
//	texDesc.MipLevels = 0;
//	texDesc.ArraySize = 6;
//	texDesc.SampleDesc.Count = 1;
//	texDesc.SampleDesc.Quality = 0;
//	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//	texDesc.Usage = D3D11_USAGE_DEFAULT;
//	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
//	texDesc.CPUAccessFlags = 0;
//	texDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS | D3D11_RESOURCE_MISC_TEXTURECUBE;
//
//	ID3D11Texture2D* cubeTex = 0;
//	HR(md3dDevice->CreateTexture2D(&texDesc, 0, &cubeTex));
//
//	//
//	// Create a render target view to each cube map face 
//	// (i.e., each element in the texture array).
//	// 
//
//	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
//	rtvDesc.Format = texDesc.Format;
//	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
//	rtvDesc.Texture2DArray.ArraySize = 1;
//	rtvDesc.Texture2DArray.MipSlice = 0;
//
//	for (int i = 0; i < 6; ++i)
//	{
//		rtvDesc.Texture2DArray.FirstArraySlice = i;
//		HR(md3dDevice->CreateRenderTargetView(cubeTex, &rtvDesc, &mDynamicCubeMapRTV[i]));
//	}
//
//	//
//	// Create a shader resource view to the cube map.
//	//
//
//	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
//	srvDesc.Format = texDesc.Format;
//	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
//	srvDesc.TextureCube.MostDetailedMip = 0;
//	srvDesc.TextureCube.MipLevels = -1;
//
//	HR(md3dDevice->CreateShaderResourceView(cubeTex, &srvDesc, &mDynamicCubeMapSRV));
//
//	ReleaseCOM(cubeTex);
//
//	//
//	// We need a depth texture for rendering the scene into the cubemap
//	// that has the same resolution as the cubemap faces.  
//	//
//
//	D3D11_TEXTURE2D_DESC depthTexDesc;
//	depthTexDesc.Width = DynamicCubeMapSize;
//	depthTexDesc.Height = DynamicCubeMapSize;
//	depthTexDesc.MipLevels = 1;
//	depthTexDesc.ArraySize = 1;
//	depthTexDesc.SampleDesc.Count = 1;
//	depthTexDesc.SampleDesc.Quality = 0;
//	depthTexDesc.Format = DXGI_FORMAT_D32_FLOAT;
//	depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
//	depthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
//	depthTexDesc.CPUAccessFlags = 0;
//	depthTexDesc.MiscFlags = 0;
//
//	ID3D11Texture2D* depthTex = 0;
//	HR(md3dDevice->CreateTexture2D(&depthTexDesc, 0, &depthTex));
//
//	// Create the depth stencil view for the entire cube
//	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
//	dsvDesc.Format = depthTexDesc.Format;
//	dsvDesc.Flags = 0;
//	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
//	dsvDesc.Texture2D.MipSlice = 0;
//	HR(md3dDevice->CreateDepthStencilView(depthTex, &dsvDesc, &mDynamicCubeMapDSV));
//
//	ReleaseCOM(depthTex);
//
//	//
//	// Viewport for drawing into cubemap.
//	// 
//
//	mDynamicCubeMapViewport.TopLeftX = 0.0f;
//	mDynamicCubeMapViewport.TopLeftY = 0.0f;
//	mDynamicCubeMapViewport.Width = (float)DynamicCubeMapSize;
//	mDynamicCubeMapViewport.Height = (float)DynamicCubeMapSize;
//	mDynamicCubeMapViewport.MinDepth = 0.0f;
//	mDynamicCubeMapViewport.MaxDepth = 1.0f;
//}
// ---------------------------------------------------------------------

void NormalDisplacementMapApp::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(mhMainWnd);
}

void NormalDisplacementMapApp::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void NormalDisplacementMapApp::OnMouseMove(WPARAM btnState, int x, int y)
{
	if( (btnState & MK_LBUTTON) != 0 )
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));

		mCam.Pitch(dy);
		mCam.RotateY(dx);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void NormalDisplacementMapApp::BuildShapeGeometryBuffers()
{
	GeometryGenerator::MeshData box;
	GeometryGenerator::MeshData grid;
	GeometryGenerator::MeshData sphere;
	GeometryGenerator::MeshData cylinder;

	GeometryGenerator geoGen;
	geoGen.CreateBox(1.0f, 1.0f, 1.0f, box);
	geoGen.CreateGrid(20.0f, 30.0f, 50, 40, grid);
	geoGen.CreateSphere(0.5f, 20, 20, sphere);
	geoGen.CreateCylinder(0.5f, 0.5f, 3.0f, 15, 15, cylinder);

	// Cache the vertex offsets to each object in the concatenated vertex buffer.
	mBoxVertexOffset      = 0;
	mGridVertexOffset     = box.Vertices.size();
	mSphereVertexOffset   = mGridVertexOffset + grid.Vertices.size();
	mCylinderVertexOffset = mSphereVertexOffset + sphere.Vertices.size();

	// Cache the index count of each object.
	mBoxIndexCount      = box.Indices.size();
	mGridIndexCount     = grid.Indices.size();
	mSphereIndexCount   = sphere.Indices.size();
	mCylinderIndexCount = cylinder.Indices.size();

	// Cache the starting index for each object in the concatenated index buffer.
	mBoxIndexOffset      = 0;
	mGridIndexOffset     = mBoxIndexCount;
	mSphereIndexOffset   = mGridIndexOffset + mGridIndexCount;
	mCylinderIndexOffset = mSphereIndexOffset + mSphereIndexCount;
	
	UINT totalVertexCount = 
		box.Vertices.size() + 
		grid.Vertices.size() + 
		sphere.Vertices.size() +
		cylinder.Vertices.size();

	UINT totalIndexCount = 
		mBoxIndexCount + 
		mGridIndexCount + 
		mSphereIndexCount +
		mCylinderIndexCount;

	//
	// Extract the vertex elements we are interested in and pack the
	// vertices of all the meshes into one vertex buffer.
	//

	std::vector<Vertex::PosNormalTexTan> vertices(totalVertexCount);

	UINT k = 0;
	for(size_t i = 0; i < box.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos      = box.Vertices[i].Position;
		vertices[k].Normal   = box.Vertices[i].Normal;
		vertices[k].Tex      = box.Vertices[i].TexC;
		vertices[k].TangentU = box.Vertices[i].TangentU;
	}

	for(size_t i = 0; i < grid.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos      = grid.Vertices[i].Position;
		vertices[k].Normal   = grid.Vertices[i].Normal;
		vertices[k].Tex      = grid.Vertices[i].TexC;
		vertices[k].TangentU = grid.Vertices[i].TangentU;
	}

	for(size_t i = 0; i < sphere.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos      = sphere.Vertices[i].Position;
		vertices[k].Normal   = sphere.Vertices[i].Normal;
		vertices[k].Tex      = sphere.Vertices[i].TexC;
		vertices[k].TangentU = sphere.Vertices[i].TangentU;
	}

	for(size_t i = 0; i < cylinder.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos      = cylinder.Vertices[i].Position;
		vertices[k].Normal   = cylinder.Vertices[i].Normal;
		vertices[k].Tex      = cylinder.Vertices[i].TexC;
		vertices[k].TangentU = cylinder.Vertices[i].TangentU;
	}

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
	 vbd.ByteWidth = sizeof(Vertex::PosNormalTexTan) * totalVertexCount;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = &vertices[0];
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mShapesVB));

	//
	// Pack the indices of all the meshes into one index buffer.
	//

	std::vector<UINT> indices;
	indices.insert(indices.end(), box.Indices.begin(), box.Indices.end());
	indices.insert(indices.end(), grid.Indices.begin(), grid.Indices.end());
	indices.insert(indices.end(), sphere.Indices.begin(), sphere.Indices.end());
	indices.insert(indices.end(), cylinder.Indices.begin(), cylinder.Indices.end());

	D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(UINT) * totalIndexCount;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = &indices[0];
    HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mShapesIB));
}
 
//void NormalDisplacementMapApp::BuildSkullGeometryBuffers()
//{
//	std::ifstream fin("Models/skull.txt");
//	
//	if(!fin)
//	{
//		MessageBox(0, L"Models/skull.txt not found.", 0, 0);
//		return;
//	}
//
//	UINT vcount = 0;
//	UINT tcount = 0;
//	std::string ignore;
//
//	fin >> ignore >> vcount;
//	fin >> ignore >> tcount;
//	fin >> ignore >> ignore >> ignore >> ignore;
//	
//	std::vector<Vertex::Basic32> vertices(vcount);
//	for(UINT i = 0; i < vcount; ++i)
//	{
//		fin >> vertices[i].Pos.x >> vertices[i].Pos.y >> vertices[i].Pos.z;
//		fin >> vertices[i].Normal.x >> vertices[i].Normal.y >> vertices[i].Normal.z;
//	}
//
//	fin >> ignore;
//	fin >> ignore;
//	fin >> ignore;
//
//	mSkullIndexCount = 3*tcount;
//	std::vector<UINT> indices(mSkullIndexCount);
//	for(UINT i = 0; i < tcount; ++i)
//	{
//		fin >> indices[i*3+0] >> indices[i*3+1] >> indices[i*3+2];
//	}
//
//	fin.close();
//
//    D3D11_BUFFER_DESC vbd;
//    vbd.Usage = D3D11_USAGE_IMMUTABLE;
//	vbd.ByteWidth = sizeof(Vertex::Basic32) * vcount;
//    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//    vbd.CPUAccessFlags = 0;
//    vbd.MiscFlags = 0;
//    D3D11_SUBRESOURCE_DATA vinitData;
//    vinitData.pSysMem = &vertices[0];
//    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mSkullVB));
//
//	//
//	// Pack the indices of all the meshes into one index buffer.
//	//
//
//	D3D11_BUFFER_DESC ibd;
//    ibd.Usage = D3D11_USAGE_IMMUTABLE;
//	ibd.ByteWidth = sizeof(UINT) * mSkullIndexCount;
//    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
//    ibd.CPUAccessFlags = 0;
//    ibd.MiscFlags = 0;
//    D3D11_SUBRESOURCE_DATA iinitData;
//	iinitData.pSysMem = &indices[0];
//    HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mSkullIB));
//}

