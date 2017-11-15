#pragma once
#include "d3dUtility.h"
#include "CManager.h"
#include "CSphere.h"

#define TABLE_WIDTH		11.5f
#define TABLE_DEPTH		5.75f

#define LEFT_BOUND		-TABLE_WIDTH/2
#define RIGHT_BOUND		TABLE_WIDTH/2
#define UP_BOUND		TABLE_DEPTH/2
#define DOWN_BOUND		-TABLE_DEPTH/2

#define HOLE_RADIUS		0.22f
#define GOAL_EPSILON	0.03f


class CTable{

private:
	bool isColliding[NUM_BALL];

public:
	CTable();

	bool create(IDirect3DDevice9* pDevice);
	void draw(IDirect3DDevice9 * pDevice);
	void destroy();

	bool hasIntersectedWithInnerWall(CSphere& ball);
	bool hasIntersectedWithHole(CSphere& ball);
	void hitByInnerWall(CSphere& ball);


private:
	DWORD m_numMaterials;
	D3DMATERIAL9 *m_pMeshMaterials;
	LPDIRECT3DTEXTURE9 *m_ppMeshTextures;

	D3DXMATRIX				mWorld;
	D3DMATERIAL9            mMtrl;
	ID3DXMesh*              pMesh;

	bool loadModel(IDirect3DDevice9* pDevice);

};