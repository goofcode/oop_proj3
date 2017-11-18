
#include "CLight.h"

CLight::CLight(void){ pMesh = NULL; }
CLight::~CLight(void) {}

bool CLight::create(IDirect3DDevice9* pDevice)
{
	if(NULL == pDevice)	return false;

	// initialize light
	ZeroMemory(&light, sizeof(light));
	light.Type = D3DLIGHT_SPOT;
	light.Diffuse = DX_WHITE;
	light.Specular = DX_WHITE * 0.9f;
	light.Ambient = DX_WHITE * 0.9f;
	light.Position = D3DXVECTOR3(0.0f, 4.0f, 0.0f);
	light.Range = 200.0f;
	light.Attenuation0 = 0.0f;
	light.Attenuation1 = 0.3f;
	light.Attenuation2 = 0.0f;
	return true;
}
void CLight::destroy(void)
{
	if(pMesh != NULL) {
		pMesh->Release( );
		pMesh = NULL;
	}
}
bool CLight::setLight(IDirect3DDevice9* pDevice)
{
	if(NULL == pDevice) return false;

	// set light enable
	pDevice->SetLight(0, &light);
	pDevice->LightEnable(0, TRUE);
	return true;
}

