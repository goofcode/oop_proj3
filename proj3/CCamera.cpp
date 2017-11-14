#include "CCamera.h"


CCamera::CCamera(){}

CCamera::~CCamera(){}

bool CCamera::create(IDirect3DDevice9 * pDevice){

	if (pDevice == NULL) return false;

	this->pDevice = pDevice;
	resetCamera();
	return true;
}

void CCamera::destroy() {
}

void CCamera::moveCamera(D3DXVECTOR3 pos, D3DXVECTOR3 target){
	this->pos = pos; 
	this->target = target;
	setCamera();
}

void CCamera::resetCamera(){
	this->pos.x = POS_X; 
	this->pos.y = POS_Y; 
	this->pos.z = POS_Z;
	
	this->target.x = TARGET_X;
	this->target.y = TARGET_Y;
	this->target.z = TARGET_Z;

	this->up.x = UP_X;
	this->up.y = UP_Y;
	this->up.z = UP_Z;

	setCamera();
}

void CCamera::setCamera(){
	D3DXMATRIX view;
	D3DXMatrixLookAtLH(&view, &this->pos, &this->target, &this->up);
	this->pDevice->SetTransform(D3DTS_VIEW, &view);
}