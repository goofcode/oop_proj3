#pragma once

#include "d3dUtility.h"

#define M_RADIUS 0.18f   // ball radius
#define M_MASS 0.21f
#define RESIST 0.98f

#define TYPE_WHITE_BALL 0
#define TYPE_SOLID_BALL 1
#define TYPE_STRIPE_BALL 2
#define TYPE_BLACK_BALL 3

#define POWER_TO_ANGLE 0.1f

#define DISAPPEAR_Y		-0.4f

#define FVF_SPHERE_VERTEX    (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1)
typedef struct SPHERE_VERTEX
{
	D3DXVECTOR3 pos;     // vertex position
	D3DXVECTOR3 norm;    // vertex normal
	float tu;            // texture coordinates
	float tv;
} SVERTEX, *LPSVERTEX;

const float ball_pos[16][3] = { { -1.20f, M_RADIUS, 0.00f },{ 1.20f, M_RADIUS, 0.00f },{ 1.58f, M_RADIUS, 0.22f },
								{ 1.58f, M_RADIUS, -0.22f },{ 1.97f, M_RADIUS, 0.44f },{ 1.97f, M_RADIUS, 0.00f },
								{ 1.97f, M_RADIUS, -0.44f },{ 2.35f, M_RADIUS, 0.66f },{ 2.35f, M_RADIUS, 0.22f },
								{ 2.35f, M_RADIUS, -0.22f },{ 2.35f, M_RADIUS, -0.66f },{ 2.74f, M_RADIUS, 0.88f },
								{ 2.74f, M_RADIUS, 0.44f },{ 2.74f, M_RADIUS, 0.00f },{ 2.74f, M_RADIUS, -0.44f },
								{ 2.74f, M_RADIUS, -0.88f } };

class CSphere {

private:
	int					id;
	D3DXVECTOR3			center;
	float               radius;
	D3DXVECTOR3			velocity;
	D3DXVECTOR3			rotation;

	static bool			isColliding[NUM_BALL][NUM_BALL];
	static int			goaled_ball;

public:
	CSphere(void);
	~CSphere(void);

public:
	bool create(IDirect3DDevice9* pDevice, int id, const float pos[3]);
	void destroy(void);
	void draw(IDirect3DDevice9* pDevice);

	bool hasIntersected(CSphere& ball);
	void hitBy(CSphere& ball);
	void startFalling();
	void disappear();
	void ballUpdate(float timeDiff);


	int getID( ) const;
	float getRadius(void)  const;
	D3DXVECTOR3 getCenter(void) const;
	float getVelocity_X( ) const;
	float getVelocity_Z( ) const;
	bool getColliding(int id1, int id2) const;
	int getBallType() const;


	void setCenter(float x, float y, float z);
	void setPower(float vx, float vy, float vz);

	static void setColliding(int id1, int id2);
	static void clearColliding(int id1, int id2);

private:
	void initState(int id, const float pos[3]);
	void loadMaterial();
	bool getMesh(IDirect3DDevice9 * pDevice);
	bool getTexture(IDirect3DDevice9 * pDevice);
	void mapTexture();

	D3DXMATRIX				mWorld;
	D3DMATERIAL9            mMtrl;
	ID3DXMesh*              pMesh;
	LPSVERTEX				m_pVerts;
	LPDIRECT3DTEXTURE9      m_pTexture; 
	int			            m_numVerts; 
};
