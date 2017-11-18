#pragma once

#include "common.h"

class CLight {
public:
	CLight(void);
	~CLight(void);
public:
	bool create(IDirect3DDevice9* pDevice);
	void destroy(void);
	bool setLight(IDirect3DDevice9* pDevice);

private:
	D3DLIGHT9           light;
	ID3DXMesh*          pMesh;
};