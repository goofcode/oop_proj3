#include "CSphere.h"

bool CSphere::isColliding[NUM_BALL][NUM_BALL] = {false,};
int CSphere::goaled_ball = 0;

CSphere::CSphere(void) { 
	radius = BALL_RADIUS; 
	pMesh = NULL;
	m_numMaterials = 0;
	m_pMeshMaterials = NULL;
	m_ppMeshTextures = NULL;
}

bool CSphere::create(IDirect3DDevice9 * pDevice, int id, const float pos[3])
{
	if(NULL == pDevice)
		return false;
	
	initState(id, pos);
	if (loadModel(pDevice) == false) return false;
	return true;
}

void CSphere::destroy(void)
{
	// Delete the materials 
	if (m_pMeshMaterials != NULL) delete[] m_pMeshMaterials;

	// Delete the textures 
	for (DWORD i = 0; i < m_numMaterials; i++)
		if (m_ppMeshTextures[i] != NULL) m_ppMeshTextures[i]->Release();

	if (m_ppMeshTextures != NULL) delete[] m_ppMeshTextures;
	if (pMesh != NULL) pMesh->Release();
}

void CSphere::draw(IDirect3DDevice9* pDevice)
{
	if (NULL == pDevice) return;

	// rotation, translation and draw
	D3DXMATRIX scale, rotate, trans;
	D3DXMatrixScaling(&scale, getRadius(), getRadius(), getRadius());
	D3DXMatrixTranslation(&trans, center.x, center.y, center.z);
	D3DXMatrixRotationYawPitchRoll(&rotate, 0, rotation.z, rotation.x);

	mWorld = scale*rotate*trans;
	pDevice->SetTransform(D3DTS_WORLD, &mWorld);
	pDevice->SetMaterial(&m_pMeshMaterials[0]);
	pDevice->SetTexture(0, m_ppMeshTextures[0]);
	pMesh->DrawSubset(0);
}

bool CSphere::hasIntersected(CSphere& ball)
{
	return distance(this->center.x, this->center.z, ball.getCenter( ).x, ball.getCenter( ).z) <= (ball.getRadius( ) + this->getRadius( ));
}

bool CSphere::hitBy(CSphere& ball)
{
	if(hasIntersected(ball) && !getColliding(this->id, ball.getID())) {
		setColliding(this->id, ball.getID( ));

		float m1 = BALL_MASS, m2 = BALL_MASS;
		float v1_x = this->getVelocity_X( ), v1_y = this->getVelocity_Z( );
		float v2_x = ball.getVelocity_X( ), v2_y = ball.getVelocity_Z( );

		float theta = angle(this->center.x, this->center.z, ball.center.x, ball.center.z);
		float v1_x_p = (m1 - RESIST*m2) / (m1 + m2)*(v1_x*cos(theta) + v1_y*sin(theta)) + (m2 + RESIST*m2) / (m1 + m2)*(v2_x*cos(theta) + v2_y*sin(theta));
		float v2_x_p = (m1 + RESIST*m1) / (m1 + m2)*(v1_x*cos(theta) + v1_y*sin(theta)) + (m2 - RESIST*m1) / (m1 + m2)*(v2_x*cos(theta) + v2_y*sin(theta));
		float v2_y_p = v2_y*cos(theta) - v2_x*sin(theta);
		float v1_y_p = v1_y*cos(theta) - v1_x*sin(theta);

		float power1_x = v1_x_p*cos(theta) - v1_y_p*sin(theta);
		float power1_y = v1_x_p*sin(theta) + v1_y_p*cos(theta);
		float power2_x = v2_x_p*cos(theta) - v2_y_p*sin(theta);
		float power2_y = v2_x_p*sin(theta) + v2_y_p*cos(theta);

		//printf("%lf %lf %lf %lf\n", power1_x, power1_y, power2_x, power2_y);

		this->setPower(power1_x, 0, power1_y);
		ball.setPower(power2_x, 0, power2_y);
		return true;
	}
	// if collision detected and still intersected, skip this time
	else if (hasIntersected(ball) && getColliding(this->id, ball.getID())){ return true; }
	// if two balls are completely separated, clear collision
	else if (!hasIntersected(ball)) { clearColliding(this->id, ball.getID()); return false; }
}

void CSphere::disappear()
{
	this->setCenter(-4.0f+ (BALL_RADIUS + 0.02f)*goaled_ball, 0.0f, -4.0f);
	this->setPower(0,0, 0);
	goaled_ball++;
}

void CSphere::ballUpdate(float timeDiff)
{
	const float TIME_SCALE = 3.3f;
	D3DXVECTOR3 center = this->getCenter( );
	double vx = this->velocity.x;
	double vy = this->velocity.y;
	double vz = this->velocity.z;

	if(abs(vx) > 0.01 || abs(vz) > 0.01)
	{
		this->setCenter(center.x + TIME_SCALE*timeDiff*vx,
						center.y + TIME_SCALE*timeDiff*vy,
						center.z + TIME_SCALE*timeDiff*vz);

		rotation.z += vx * POWER_TO_ANGLE;
		rotation.x += vz * POWER_TO_ANGLE;
	}
	else { this->setPower(0, 0, 0); }

	float rate = 1 - (1 - DECREASE_RATE)*timeDiff * 400;
	if(rate < 0) rate = 0;
	
	this->setPower(getVelocity_X() * rate, 0, getVelocity_Z() * rate);
}


int CSphere::getID( ) const { return this->id; }
float CSphere::getRadius(void)  const { return this->radius; }
D3DXVECTOR3 CSphere::getCenter(void) const { return center; }
float CSphere::getVelocity_X( ) const { return this->velocity.x; }
float CSphere::getVelocity_Z( ) const { return this->velocity.z; }
bool CSphere::getColliding(int id1, int id2) const
{
	return isColliding[id1][id2];
}
int CSphere::getBallType() const
{
	if (1 <= this->id && this->id <= 7) return TYPE_SOLID_BALL;
	else if (9 <= this->id && this->id <= 15) return TYPE_STRIPE_BALL;
	else if (this->id == WHITE_BALL) return TYPE_WHITE_BALL;
	else if (this->id == BLACK_BALL) return TYPE_BLACK_BALL;
	else return -1;
}


void CSphere::setCenter(float x, float y, float z)
{
	this->center.x = x;
	this->center.y = y;
	this->center.z = z;
}
void CSphere::setPower(float vx, float vy, float vz)
{
	this->velocity.x = vx;
	this->velocity.y = vy;
	this->velocity.z = vz;
}
void CSphere::setColliding(int id1, int id2) { 
	isColliding[id1][id2] = isColliding[id2][id1] = true;
}
void CSphere::clearColliding(int id1, int id2){
	isColliding[id1][id2] = isColliding[id2][id1] = false;
}

void CSphere::initState(int id, const float pos[3])
{
	this->id = id;
	this->center.x = pos[0];
	this->center.y = pos[1];
	this->center.z = pos[2];
	this->velocity.x = 0;
	this->velocity.y = 0;
	this->velocity.z = 0;
	
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> pi_rand(-PI, PI);
	this->rotation.x = pi_rand(gen);
	this->rotation.y = pi_rand(gen);
	this->rotation.z = pi_rand(gen);
}

bool CSphere::loadModel(IDirect3DDevice9 * pDevice)
{
	LPD3DXBUFFER pMaterialBuffer;
	if (FAILED(D3DXLoadMeshFromX("rsc\\ball.x", D3DXMESH_MANAGED, pDevice, NULL, &pMaterialBuffer, NULL, &m_numMaterials, &pMesh)))
		return false;

	D3DXMATERIAL* pMaterials = (D3DXMATERIAL*)pMaterialBuffer->GetBufferPointer();

	m_pMeshMaterials = new D3DMATERIAL9[m_numMaterials];
	m_ppMeshTextures = new LPDIRECT3DTEXTURE9[m_numMaterials];
	
	ZeroMemory(&mMtrl, sizeof(mMtrl));
	this->mMtrl.Ambient = d3d::WHITE;
	this->mMtrl.Diffuse = d3d::WHITE;
	this->mMtrl.Specular = d3d::WHITE;
	this->mMtrl.Emissive = d3d::WHITE;
	this->mMtrl.Power = 2.0f;
	//m_pMeshMaterials[0] = pMaterials[0].MatD3D;
	//m_pMeshMaterials[0].Ambient = m_pMeshMaterials[0].Diffuse;
	m_pMeshMaterials[0] = mMtrl;
	
	ostringstream sstream;
	sstream << "rsc\\" <<  this->id << ".jpg";
	cout << "reading " << sstream.str() << endl;

	if (FAILED(D3DXCreateTextureFromFile(pDevice, sstream.str().c_str(), &m_ppMeshTextures[0])))
		return false;
	return true;
}

