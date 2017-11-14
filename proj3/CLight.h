#pragma once

#include "d3dUtility.h"

class CLight {
public:
	CLight(void);
	~CLight(void);
public:
	bool create(IDirect3DDevice9* pDevice);
	void destroy(void);
	bool setLight(IDirect3DDevice9* pDevice);

	D3DXVECTOR3 getPosition(void) const;

private:
	DWORD               m_index;
	D3DLIGHT9           m_lit;
	ID3DXMesh*          pMesh;
	d3d::BoundingSphere m_bound;
};