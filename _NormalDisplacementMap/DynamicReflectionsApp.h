#ifndef DYNAMIC_REFLECTIONS_APP
#define DYNAMIC_REFLECTIONS_APP

#include "NormalDisplacementMapApp.h"
#include "MeshLoader.h"

class DynamicReflectionsApp : public NormalDisplacementMapApp
{
private:
	// Mesh
	SkullAndShapes_MeshLoader SkullMesh;

	// Dynamic reflections  
	Camera mDynamicCubeMapCamera[6];

	D3D11_VIEWPORT mDynamicCubeMapViewport;

	ID3D11RenderTargetView* mDynamicCubeMapRTV[6];
	ID3D11ShaderResourceView* mDynamicCubeMapSRV;
	ID3D11DepthStencilView* mDynamicCubeMapDSV;

	static const int DynamicCubeMapSize = 256;

protected:
	virtual void DrawScene(Camera& camera, bool drawCenterSkull);

public:
	DynamicReflectionsApp(HINSTANCE hInstance);
	~DynamicReflectionsApp();

	// Init 
	virtual bool Init();
	virtual void DrawScene();

	// Regime functions 
	void BuildCubeFaceCamera(float x, float y, float z);
	void BuildDynamicCubeMapViews();
};

#endif