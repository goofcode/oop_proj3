#include "CSphere.h"

bool CSphere::isColliding[NUM_BALL][NUM_BALL] = {false,};
int CSphere::goaled_ball = 0;

CSphere::CSphere(void) { 
	radius = M_RADIUS; 
	pMesh = NULL;
}
CSphere::~CSphere(void) {}

bool CSphere::create(IDirect3DDevice9 * pDevice, int id, const float pos[3])
{
	if(NULL == pDevice)
		return false;

	initState(id, pos);
	loadMaterial();
	if (getMesh(pDevice) == false) return false;
	if (getTexture(pDevice) == false) return false;
	mapTexture();
	
	return true;
}

void CSphere::destroy(void)
{
	if(pMesh != NULL) {
		pMesh->Release( );
		pMesh = NULL;
	}
}

void CSphere::draw(IDirect3DDevice9* pDevice)
{
	if(NULL == pDevice)
		return;

	D3DXMatrixIdentity(&mWorld);

	// translation
	D3DXMatrixTranslation(&mWorld, this->center.x, this->center.y, this->center.z);
	// rotation
	mapTexture();
	
	pDevice->SetTransform(D3DTS_WORLD, &mWorld);
	pDevice->SetTexture(0, m_pTexture);
	pDevice->SetMaterial(&mMtrl);
	pDevice->SetFVF(FVF_SPHERE_VERTEX);
	this->pMesh->DrawSubset(0);
}

bool CSphere::hasIntersected(CSphere& ball)
{
	return distance(this->center.x, this->center.z, ball.getCenter( ).x, ball.getCenter( ).z) <= (ball.getRadius( ) + this->getRadius( ));
}

void CSphere::hitBy(CSphere& ball)
{
	if(hasIntersected(ball) && !getColliding(this->id, ball.getID())) {
		setColliding(this->id, ball.getID( ));

		float m1 = M_MASS, m2 = M_MASS;
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
	}
	// if collision detected and still intersected, skip this time
	else if (hasIntersected(ball) && getColliding(this->id, ball.getID())){ return; }
	// if two balls are completely separated, clear collision
	else if (!hasIntersected(ball)) { clearColliding(this->id, ball.getID( )); }
}

void CSphere::disappear()
{
	this->setCenter(-4.0f+0.3f*goaled_ball, 0.0f, -4.0f);
	this->setPower(0,0, 0);
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

void CSphere::loadMaterial()
{
	ZeroMemory(&mMtrl, sizeof(mMtrl));
	this->mMtrl.Ambient = d3d::WHITE;
	this->mMtrl.Diffuse = d3d::WHITE;
	this->mMtrl.Specular = d3d::WHITE;
	this->mMtrl.Emissive = d3d::WHITE;
	this->mMtrl.Power = 5.0f;
}

bool CSphere::getMesh(IDirect3DDevice9 * pDevice)
{
	LPD3DXMESH tmpMesh;
	if (FAILED(D3DXCreateSphere(pDevice, getRadius(), 50, 50, &tmpMesh, NULL)))  return false;
	if (FAILED(tmpMesh->CloneMeshFVF(D3DXMESH_SYSTEMMEM, FVF_SPHERE_VERTEX, pDevice, &pMesh))) return false;
	tmpMesh->Release();
	return true;
}

bool CSphere::getTexture(IDirect3DDevice9 * pDevice)
{
	ostringstream sstream;
	sstream << "rsc\\"<<std::setw(2) << setfill('0') << this->id << ".jpg";
	if (FAILED(D3DXCreateTextureFromFile(pDevice, sstream.str().c_str(), &m_pTexture)))
		return false;
	return true;
}

void CSphere::mapTexture()
{
	if (SUCCEEDED(pMesh->LockVertexBuffer(0, (LPVOID *)&m_pVerts))) {
		m_numVerts = pMesh->GetNumVertices();

		D3DXMATRIX m_rotate;
		
		D3DXMatrixIdentity(&m_rotate);
		D3DXMatrixRotationYawPitchRoll(&m_rotate, -this->rotation.y, -this->rotation.z, -this->rotation.x);

		D3DXVECTOR3 rotated; 
			
		for (int i = 0; i < m_numVerts; i++) {
			D3DXVec3TransformCoord(&rotated, &(m_pVerts[i].norm), &m_rotate);
			m_pVerts[i].tu = asin(rotated.x) / (2 * PI) + 0.25f;
			m_pVerts[i].tv = asin(rotated.y) / (PI)+0.5f;

		}
		pMesh->UnlockVertexBuffer();
	}
}