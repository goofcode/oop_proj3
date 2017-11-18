#include "CTable.h"

CTable::CTable()
{
	pMesh = NULL;
	m_numMaterials = 0;
	m_pMeshMaterials = NULL;
	m_ppMeshTextures = NULL;
}

bool CTable::create(IDirect3DDevice9 * pDevice)
{
	if (NULL == pDevice) return false;
	if (loadModel(pDevice) == false) return false;
	return true;
}

bool CTable::loadModel(IDirect3DDevice9 * pDevice)
{
	LPD3DXBUFFER pMaterialBuffer;
	if (FAILED(D3DXLoadMeshFromX("rsc\\board.x", D3DXMESH_MANAGED, pDevice, NULL, &pMaterialBuffer, NULL, &m_numMaterials, &pMesh))) 
		return false;

	D3DXMATERIAL* pMaterials = (D3DXMATERIAL*)pMaterialBuffer->GetBufferPointer();

	m_pMeshMaterials = new D3DMATERIAL9[m_numMaterials];
	m_ppMeshTextures = new LPDIRECT3DTEXTURE9[m_numMaterials];
	
	for (DWORD i = 0; i < m_numMaterials; i++)
	{
		string texture_file;
		
		m_pMeshMaterials[i] = pMaterials[i].MatD3D;
		m_pMeshMaterials[i].Ambient = m_pMeshMaterials[i].Diffuse;
		//m_pMeshMaterials[i].Emissive = d3d::WHITE * 0.2f;

		switch (i) {
			// outer wall
			case 1: 
				texture_file = "rsc\\wood.png";
				break;
			// ground 
			case 3:
				m_pMeshMaterials[i].Ambient = m_pMeshMaterials[i].Diffuse = d3d::WHITE;
				texture_file = "rsc\\green.png";
				break;
			// outside of hole
			case 4:
				ZeroMemory(&m_pMeshMaterials[i], sizeof(D3DMATERIAL9));
				m_pMeshMaterials[i].Ambient = d3d::WHITE* 0.3f;
				m_pMeshMaterials[i].Diffuse = d3d::WHITE * 0.8f;
				m_pMeshMaterials[i].Specular = d3d::WHITE * 0.3f;
				m_pMeshMaterials[i].Emissive = d3d::WHITE * 0.4f;
				m_pMeshMaterials[i].Power = 30.0f;
				texture_file = "rsc\\black.png";
				break;
			// inner of hole
			case 5:
				texture_file = "rsc\\hole.png";
				break;

			default:
				break;
		}

		if (texture_file != "") {
			if (FAILED(D3DXCreateTextureFromFile(pDevice, texture_file.c_str(), &m_ppMeshTextures[i])))
				return false;
		}
		else m_ppMeshTextures[i] = NULL;
	}

	return true;
}

bool CTable::hasIntersectedWithInnerWall(CSphere & ball)
{

	float rx, lx, uz, dz;		//right left x, up down z;
	rx = ball.getCenter().x + ball.getRadius(); lx = ball.getCenter().x - ball.getRadius();
	uz = ball.getCenter().z + ball.getRadius(); dz = ball.getCenter().z - ball.getRadius();


	if (rx >= RIGHT_BOUND || lx <= LEFT_BOUND || uz >= UP_BOUND || dz<= DOWN_BOUND) 
		return true;
	return false;
}
void CTable::hitByInnerWall(CSphere & ball)
{
	float rx, lx, uz, dz;		//right left x, up down z;
	rx = ball.getCenter().x + ball.getRadius(); lx = ball.getCenter().x - ball.getRadius();
	uz = ball.getCenter().z + ball.getRadius(); dz = ball.getCenter().z - ball.getRadius();

	// hit inner wall normaly
	if (hasIntersectedWithInnerWall(ball) && isColliding[ball.getID()] == false){

		isColliding[ball.getID()] = true;

		float vx = ball.getVelocity_X();
		float vz = ball.getVelocity_Z();

		if (rx >= RIGHT_BOUND || lx <= LEFT_BOUND)  return ball.setPower(-vx, 0, vz);
		if(uz >= UP_BOUND || dz <= DOWN_BOUND) return ball.setPower(vx, 0, -vz); 
	}

	// if collision detected and still intersected, skip this time
	else if (hasIntersectedWithInnerWall(ball) && isColliding[ball.getID()] == true ) return; 

	// if ball and wall are completely separated, clear collision
	else 
		isColliding[ball.getID()] = false; 
}

bool CTable::hasIntersectedWithHole(CSphere & ball)
{
	float cx, cz, vx, vz, direction;
	cx = ball.getCenter().x;
	cz = ball.getCenter().z;
	vx = ball.getVelocity_X();
	vz = ball.getVelocity_Z();
	direction = angle(0, 0, vx, vz);
	
	float hole_cx, hole_cz = UP_BOUND;
	for (int i = 0; i < 2; i++)
	{
		hole_cx = LEFT_BOUND;
		for (int j = 0; j < 3; j++) {
			if (distance(cx, cz, hole_cx, hole_cz) < HOLE_RADIUS  + GOAL_EPSILON)
				return true;

			hole_cx += TABLE_WIDTH / 2;
		}
		hole_cz -= TABLE_DEPTH;
	}

	return false;
}

void CTable::draw(IDirect3DDevice9 * pDevice)
{
	if (NULL == pDevice) return;

	// rotation, translation and draw
	D3DXMATRIX scale, rotate, trans;
	D3DXMatrixScaling(&scale, 1.5, 1, 1.5);
	D3DXMatrixTranslation(&trans, 0, -3.4f,0.0f);
	D3DXMatrixRotationY(&rotate, PI / 2);

	mWorld = scale*rotate*trans;
	pDevice->SetTransform(D3DTS_WORLD, &mWorld);
	for (DWORD i = 0; i < m_numMaterials; i++)
	{
		pDevice->SetMaterial(&m_pMeshMaterials[i]);
		pDevice->SetTexture(0, m_ppMeshTextures[i]);
		pMesh->DrawSubset(i);
	}
}

void CTable::destroy()
{
	// Delete the materials 
	if(m_pMeshMaterials != NULL) delete[] m_pMeshMaterials;

	// Delete the textures 
	for (DWORD i = 0; i < m_numMaterials; i++)
		if(m_ppMeshTextures[i] != NULL) m_ppMeshTextures[i]->Release();

	if(m_ppMeshTextures!= NULL) delete[] m_ppMeshTextures;
	if(pMesh!=NULL) pMesh->Release();
}



