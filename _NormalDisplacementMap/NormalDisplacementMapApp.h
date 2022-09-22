#ifndef NORMAL_DISPLACEMENT_MAP_APP
#define NORMAL_DISPLACEMENT_MAP_APP

#include "d3dApp.h"
#include "Camera.h"
#include "Sky.h"

#include "d3dx11Effect.h"
#include "GeometryGenerator.h"
#include "MathHelper.h"
#include "LightHelper.h"
#include "Effects.h"
#include "Vertex.h"
#include "RenderStates.h"

#include "DXTK/DDSTextureLoader.h"  // FarUPD_WinSDK

enum RenderOptions
{
	RenderOptionsBasic = 0,
	RenderOptionsNormalMap = 1,
	RenderOptionsDisplacementMap = 2
};

class NormalDisplacementMapApp : public D3DApp
{
public:
	NormalDisplacementMapApp(HINSTANCE hInstance);
	~NormalDisplacementMapApp();

	virtual bool Init();
	virtual void DrawScene();

	void OnResize();
	void UpdateScene(float dt);

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

private:
	void BuildShapeGeometryBuffers();

	// -------- DynamicReflection --------
	//void BuildCubeFaceCamera(float x, float y, float z);
	//void BuildDynamicCubeMapViews();
	// -----------------------------------------

protected:
	virtual void DrawScene(Camera& camera);

	Sky* mSky;

	ID3D11Buffer* mShapesVB;
	ID3D11Buffer* mShapesIB;

	//ID3D11Buffer* mSkullVB;
	//ID3D11Buffer* mSkullIB;

	ID3D11Buffer* mSkySphereVB;
	ID3D11Buffer* mSkySphereIB;

	ID3D11ShaderResourceView* mStoneTexSRV;
	ID3D11ShaderResourceView* mBrickTexSRV;

	ID3D11ShaderResourceView* mStoneNormalTexSRV;
	ID3D11ShaderResourceView* mBrickNormalTexSRV;

	DirectionalLight mDirLights[3];
	Material mGridMat;
	Material mBoxMat;
	Material mCylinderMat;
	Material mSphereMat;
	//Material mSkullMat;

	// Define transformations from local spaces to world space.
	XMFLOAT4X4 mSphereWorld[10];
	XMFLOAT4X4 mCylWorld[10];
	XMFLOAT4X4 mBoxWorld;
	XMFLOAT4X4 mGridWorld;
	//XMFLOAT4X4 mSkullWorld;

	int mBoxVertexOffset;
	int mGridVertexOffset;
	int mSphereVertexOffset;
	int mCylinderVertexOffset;

	UINT mBoxIndexOffset;
	UINT mGridIndexOffset;
	UINT mSphereIndexOffset;
	UINT mCylinderIndexOffset;

	UINT mBoxIndexCount;
	UINT mGridIndexCount;
	UINT mSphereIndexCount;
	UINT mCylinderIndexCount;

	//UINT mSkullIndexCount;

	RenderOptions mRenderOptions;

	Camera mCam;

	// -------- DynamicReflection --------
	//Camera mDynamicCubeMapCamera[6];

	//D3D11_VIEWPORT mDynamicCubeMapViewport;

	//ID3D11RenderTargetView* mDynamicCubeMapRTV[6];
	//ID3D11ShaderResourceView* mDynamicCubeMapSRV;
	//ID3D11DepthStencilView* mDynamicCubeMapDSV;

	//static const int DynamicCubeMapSize = 256;
	// -----------------------------------------

	POINT mLastMousePos;
};

#endif