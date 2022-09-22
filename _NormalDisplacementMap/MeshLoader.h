#ifndef MESH_LOADER
#define MESH_LOADER

#include <d3d11.h>
#include "LightHelper.h"
//#include <minwindef.h>

class SkullAndShapes_MeshLoader
{
public:
	ID3D11ShaderResourceView* mSkullStoneTexSRV; // SRV for Skull texture

	ID3D11Buffer* mSkullVB;
	ID3D11Buffer* mSkullIB;

	Material mSkullMat;
	XMFLOAT4X4 mSkullWorld;
	UINT mSkullIndexCount;

private:
	void CreateSkullTextureSRV(ID3D11Device * md3dDevice);
	void BuildSkullGeometryBuffers(ID3D11Device * md3dDevice);

public:
	SkullAndShapes_MeshLoader();
	~SkullAndShapes_MeshLoader();

	void Init(ID3D11Device * md3dDevice);
};
#endif

