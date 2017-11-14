#pragma once

#include "d3dUtility.h"

#define BALL_RADIUS 0.18f   // ball radius
#define BALL_MASS 0.21f
#define RESIST 0.98f

#define TYPE_WHITE_BALL 0
#define TYPE_SOLID_BALL 1
#define TYPE_STRIPE_BALL 2
#define TYPE_BLACK_BALL 3

#define POWER_TO_ANGLE 0.1f

#define DISAPPEAR_Y		-0.4f

const float ball_pos[16][3] = { { -1.20f, BALL_RADIUS, 0.00f },{ 1.20f, BALL_RADIUS, 0.00f },{ 1.58f, BALL_RADIUS, 0.22f },
								{ 1.58f, BALL_RADIUS, -0.22f },{ 1.97f, BALL_RADIUS, 0.44f },{ 1.97f, BALL_RADIUS, 0.00f },
								{ 1.97f, BALL_RADIUS, -0.44f },{ 2.35f, BALL_RADIUS, 0.66f },{ 2.35f, BALL_RADIUS, 0.22f },
								{ 2.35f, BALL_RADIUS, -0.22f },{ 2.35f, BALL_RADIUS, -0.66f },{ 2.74f, BALL_RADIUS, 0.88f },
								{ 2.74f, BALL_RADIUS, 0.44f },{ 2.74f, BALL_RADIUS, 0.00f },{ 2.74f, BALL_RADIUS, -0.44f },
								{ 2.74f, BALL_RADIUS, -0.88f } };

class CSphere {

private:
	int					id;
	float               radius;
	D3DXVECTOR3			center;
	D3DXVECTOR3			velocity;
	D3DXVECTOR3			rotation;

	static bool			isColliding[NUM_BALL][NUM_BALL];
	static int			goaled_ball;


public:
	CSphere(void);

	bool create(IDirect3DDevice9* pDevice, int id, const float pos[3]);
	void destroy(void);
	void draw(IDirect3DDevice9* pDevice);

	bool hasIntersected(CSphere& ball);
	bool hitBy(CSphere& ball);
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
	DWORD					m_numMaterials;
	D3DMATERIAL9			*m_pMeshMaterials;
	LPDIRECT3DTEXTURE9		*m_ppMeshTextures;

	D3DXMATRIX				mWorld;
	D3DMATERIAL9            mMtrl;
	ID3DXMesh*              pMesh;

	D3DMATERIAL9			materials;

	void initState(int id, const float pos[3]);
	bool loadModel(IDirect3DDevice9* pDevice);

};
