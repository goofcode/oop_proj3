#pragma once
#include "d3dUtility.h"

#define POS_X 0.0f
#define POS_Y 12.0f
#define POS_Z -0.0001f

#define TARGET_X 0.0f
#define TARGET_Y 0.0f
#define TARGET_Z 0.0f

#define UP_X 0.0f
#define UP_Y 3.0f
#define UP_Z 0.0f

class CCamera{
private :
	IDirect3DDevice9 * pDevice;
	D3DXVECTOR3 pos;
	D3DXVECTOR3 target;
	D3DXVECTOR3 up;

	void setCamera();

public:
	CCamera();
	~CCamera();
	bool create(IDirect3DDevice9 * pDevice);
	void destroy();
	void moveCamera(D3DXVECTOR3 pos, D3DXVECTOR3 target);
	void resetCamera();
};