
#include "CLight.h"

CLight::CLight(void)
{
	static DWORD i = 0;
	m_index = i++;
	pMesh = NULL;
	m_bound._center = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_bound._radius = 0.0f;
}
CLight::~CLight(void) {}

bool CLight::create(IDirect3DDevice9* pDevice)
{
	if(NULL == pDevice)
		return false;

	::ZeroMemory(&m_lit, sizeof(m_lit));
	m_lit.Type = D3DLIGHT_POINT;
	m_lit.Diffuse = d3d::WHITE;
	m_lit.Specular = d3d::WHITE * 0.9f;
	m_lit.Ambient = d3d::WHITE * 0.9f;
	m_lit.Position = D3DXVECTOR3(0.0f, 2.0f, 0.0f);
	m_lit.Range = 100.0f;
	m_lit.Attenuation0 = 0.0f;
	m_lit.Attenuation1 = 0.9f;
	m_lit.Attenuation2 = 0.0f;

	m_bound._center = m_lit.Position;
	m_bound._radius = 0.01f;
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
	if(NULL == pDevice)
		return false;

	D3DXMATRIX identity;
	D3DXMatrixIdentity(&identity);

	D3DXVECTOR3 pos(m_bound._center);
	D3DXVec3TransformCoord(&pos, &pos, &identity);
	m_lit.Position = pos;

	pDevice->SetLight(m_index, &m_lit);
	pDevice->LightEnable(m_index, TRUE);
	return true;
}

D3DXVECTOR3 CLight::getPosition(void) const { return D3DXVECTOR3(m_lit.Position); }

