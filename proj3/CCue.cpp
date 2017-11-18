#include "CCue.h"


CCue::CCue(void){ pVB = NULL; center.y = QUEUE_Y; }
CCue::~CCue(void) {}

bool CCue::create(IDirect3DDevice9* pDevice)
{
	if (NULL == pDevice)
		return false;

	if (loadModel(pDevice) == false) return false;
	loadMaterial();
	if (getTexture(pDevice) == false) return false;

	return true;
}

void CCue::destroy(void)
{
	if (m_pTexture != NULL)
		m_pTexture->Release();
}

void CCue::draw(IDirect3DDevice9* pDevice)
{
	if (NULL == pDevice) return;

	if (isAiming) {
		D3DXMatrixRotationYawPitchRoll(&mWorld,0, PI/2, theta+3*PI/2);
		mWorld._41 = center.x; mWorld._42 = center.y; mWorld._43 = center.z;
	}
	else {
		D3DXMatrixRotationZ(&mWorld, -PI/2);
		mWorld._41 = 0; mWorld._42 = center.y; mWorld._43 = 4.2f;
	}
	pDevice->SetTransform(D3DTS_WORLD, &mWorld);
	pDevice->SetStreamSource(0, pVB, 0, sizeof(CVERTEX));
	pDevice->SetTexture(0, m_pTexture);
	pDevice->SetFVF(FVF_CUE_VERTEX);
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, m_numVerts -2);
}

D3DXVECTOR3 CCue::getCenter( ) const{
	return center;
}

void CCue::ready(float x, float y, float z, D3DXVECTOR3 target_center){
	
	target = target_center;
	theta = angle(target.x, target.z, x, z);
	power = 0;

	setCenter();
}

void CCue::rotate(float dTheta){
	theta += dTheta;
	setCenter();
}

void CCue::charge()
{
	this->power += UNIT_POWER_CHARGE;
	if (this->power > MAX_POWER) this->power = MAX_POWER;
	setCenter();
}

void CCue::discharge()
{
	this->power -= UNIT_POWER_DISCHARGE;
	setCenter();
}

bool CCue::hasIntersected(const CSphere & ball) const
{
	return distance(ball.getCenter().x, ball.getCenter().z, this->center.x, this->center.z) <= CUE_LENGTH/2 + ball.getRadius();
}

bool CCue::loadModel(IDirect3DDevice9 * pDevice)
{
	// create cue shape vetex
	const int num_vert_one_side = 50;

	m_numVerts = num_vert_one_side * 2;
	if (FAILED(pDevice->CreateVertexBuffer(m_numVerts * sizeof(CVERTEX), 
		0, FVF_CUE_VERTEX, D3DPOOL_DEFAULT, &pVB, NULL))) return false;
	if (FAILED(pVB->Lock(0, 0, (void**)&m_pVerts, 0))) return false;

	for (int i = 0; i < num_vert_one_side; i++)
	{
		float theta = (2 * PI*i) / (50 - 1);

		// top, bottom part
		m_pVerts[2 * i + 0].pos = D3DXVECTOR3( TOP_RADIUS*sin(theta), CUE_LENGTH / 2, TOP_RADIUS* cos(theta));
		m_pVerts[2 * i + 1].pos = D3DXVECTOR3( BOTTOM_RADIUS*sin(theta), -CUE_LENGTH / 2, BOTTOM_RADIUS*cos(theta));

		m_pVerts[2 * i + 0].tu = 1.0f;
		m_pVerts[2 * i + 0].tv = (float)i / (num_vert_one_side - 1);
		m_pVerts[2 * i + 1].tu = 0.0f;
		m_pVerts[2 * i + 1].tv = (float)i / (num_vert_one_side - 1);
	}

	pVB->Unlock();
	return true;
}
void CCue::loadMaterial()
{
	ZeroMemory(&mMtrl, sizeof(mMtrl));
	//this->mMtrl.Ambient = DX_WHITE * 0.8f;
	//this->mMtrl.Diffuse = DX_WHITE * 0.9f;
	//this->mMtrl.Specular = DX_WHITE * 0.8f;
	this->mMtrl.Emissive = DX_WHITE*0.7f;
	//this->mMtrl.Power = 5.0f;
}
bool CCue::getTexture(IDirect3DDevice9 * pDevice)
{
	if (FAILED(D3DXCreateTextureFromFile(pDevice, "rsc\\cue.png", &m_pTexture))) return false;
	return true;
}

float CCue::getTheta( ) const
{
	return this->theta;
}

float CCue::getPower() const
{
	return this->power;
}

bool CCue::getIsAiming() const
{
	return this->isAiming;
}

bool CCue::getIsCharging() const
{
	return this->isCharging;
}

void CCue::setPower(float power)
{
	this->power = power;
}

void CCue::setIsAiming()
{
	this->isAiming = true;
}

void CCue::setIsCharging()
{
	this->isCharging = true;
}

void CCue::clearIsAiming()
{
	this->isAiming = false;
}

void CCue::clearIsCharging()
{
	this->isCharging = false;
}

void CCue::setCenter()
{
	float dist_from_target = DEFAULT_DIST_FROM_TARGET + this->power * POWER_TO_DIST_RATIO;
	this->center.x = this->target.x - dist_from_target * cos(this->theta);
	this->center.y = this->target.y;
	this->center.z = this->target.z - dist_from_target * sin(this->theta);
}
