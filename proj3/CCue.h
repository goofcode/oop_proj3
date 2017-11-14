#pragma once

#include "d3dUtility.h"
#include "CSphere.h"

#define CUE_LENGTH 8.0f
#define TOP_RADIUS 0.06f
#define BOTTOM_RADIUS 0.10f

#define QUEUE_Y 0.21f

#define MAX_POWER 6.0f
#define UNIT_POWER_CHARGE 0.1f
#define UNIT_POWER_DISCHARGE 2.0f

#define DEFAULT_DIST_FROM_TARGET (CUE_LENGTH/2 + 0.4f)
#define POWER_TO_DIST_RATIO 0.13
#define CUE_VIEW_Y 1.5f

#define FVF_CUE_VERTEX (D3DFVF_XYZ | D3DFVF_TEX1)

typedef struct CUE_VERTEX
{
	D3DXVECTOR3 pos;     // vertex position
	float tu;            // texture coordinates
	float tv;
} CVERTEX, *LPCVERTEX;


class CCue {

private:
	D3DXVECTOR3		target;
	D3DXVECTOR3		center;
	float			theta;
	float			power;

	bool			isAiming;
	bool			isCharging;

	void setCenter();

public:
	CCue();
	~CCue();

public:
	bool create(IDirect3DDevice9* pDevice);
	void destroy(void);
	void draw(IDirect3DDevice9* pDevice);
	
	D3DXVECTOR3 getCenter( ) const;
	double getTheta() const;
	double getPower() const;
	bool getIsAiming() const;
	bool getIsCharging() const;

	void setPower(float power);

	void setIsAiming();
	void setIsCharging();
	void clearIsAiming();
	void clearIsCharging();

	void ready(double x, double y, double z, D3DXVECTOR3 target_center);
	void rotate(double dTheta);
	void charge();
	void discharge();
	bool hasIntersected(const CSphere& ball) const;

private:
	void initState();
	bool loadModel(IDirect3DDevice9 * pDevice);
	bool getTexture(IDirect3DDevice9 * pDevice);

	D3DXMATRIX				mWorld;
	LPCVERTEX				m_pVerts;
	LPDIRECT3DVERTEXBUFFER9 pVB;
	LPDIRECT3DTEXTURE9      m_pTexture;
	int			            m_numVerts;
};