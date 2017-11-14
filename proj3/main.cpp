
#include "main.h"

HINSTANCE hInstance;

// global DX interfaces
IDirect3DDevice9* Device = NULL;
ID3DXFont* Font;
ID3DXSprite* Sprite;

// 3D components
CTable			table;
CSphere			ball[NUM_BALL];
CCue			cue;
CLight			light;
CCamera			camera;


// initialization
bool Setup( )
{
	/* create components */ 
	cout << "creating table" << endl;
	if (table.create(Device) == false) return false;
	cout << "creating balls" << endl;
	for (int i = 0; i < NUM_BALL; i++) 
		if (ball[i].create(Device, i, ball_pos[i]) == false) return false;
	cout << "creating cue" << endl;
	if (false == cue.create(Device)) return false;
	cout << "creating light" << endl;
	if(false == light.create(Device)) return false;
	light.setLight(Device);
	cout << "creating camera" << endl;
	if(camera.create(Device)== false) return false;
	/* create components */


	// Set the projection matrix.
	D3DXMATRIX mProj;
	D3DXMatrixIdentity(&mProj);
	D3DXMatrixPerspectiveFovLH(&mProj, D3DX_PI / 4, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 1.0f, 100.0f);
	Device->SetTransform(D3DTS_PROJECTION, &mProj);

	// Set render states.
	Device->SetRenderState(D3DRS_LIGHTING, TRUE);
	Device->SetRenderState(D3DRS_SPECULARENABLE, TRUE);
	Device->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);

	// create font
	D3DXCreateFont(Device, 25, 0, FW_BOLD, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("����ü"), &Font);
	D3DXCreateSprite(Device, &Sprite);

	return true;
}

void Cleanup(void)
{
	table.destroy();
	for (int i = 0; i < NUM_BALL; i++)
		ball[i].destroy( );
	light.destroy( );
	cue.destroy();
}


// timeDelta represents the time between the current image frame and the last image frame.
// the distance of moving balls should be "velocity * timeDelta"
bool Display(float timeDelta)
{
	if (Device)
	{
		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00afafaf, 1.0f, 0);
		Device->BeginScene();

		// update component status
		for (int i = 0; i < NUM_BALL; i++)
			ball[i].ballUpdate(timeDelta);
		for (int j = 0; j < NUM_BALL; j++)			
			table.hitByInnerWall(ball[j]);
		for (int j = 0; j < NUM_BALL; j++) 
			table.goalIntoHole(CManager::GetInstance(), ball[j]);
		for (int i = 0; i < NUM_BALL; i++) 
			for (int j = i + 1; j < NUM_BALL; j++)
				ball[i].hitBy(ball[j]);
		

		// draw components
		table.draw(Device);
		for (int i = 0; i < NUM_BALL; i++) ball[i].draw(Device);
		cue.draw(Device);


		// draw game information text
		CManager* manager = CManager::GetInstance();
		if (Sprite) Sprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE);

		RECT score_rect[2] = { { 0,0,0,0 }, { 0, 30, 0, 0 } }, 
			ball_type_rect[2] ={{ 900, 0, 0,0 },{ 900, 30, 0, 0 }}, 
			turn_rect = { 600,0,0,0 };
		
		for (int i = 0; i < 2; i++)
		{
			ostringstream sstream;
			sstream << "Player " << i + 1 << ": " << manager->getScore(i + 1) << " ball(s)";
			Font->DrawTextA(Sprite, sstream.str().c_str(), -1, &score_rect[i], DT_NOCLIP, D3DCOLOR_XRGB(0, 0, 0));

			int ball_type = manager->getBallType(i + 1);
			if (ball_type != -1)
				Font->DrawTextA(Sprite, (ball_type == TYPE_SOLID_BALL ? "SOLID" : "STRIPE"), -1, &ball_type_rect[i], DT_NOCLIP, D3DCOLOR_XRGB(0, 0, 0));
		}
		Font->DrawTextA(Sprite, manager->getTurn() == 1?"PLAYER 1 Turn": "PLAYER 2 Turn", -1, &turn_rect, DT_NOCLIP, D3DCOLOR_XRGB(0, 0, 0));
		Sprite->End();

		Device->EndScene( );
		Device->Present(0, 0, 0, 0);
		Device->SetTexture(0, NULL);
	}
	return true;
}


LRESULT CALLBACK d3d::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static int point_x, point_y;
	static float saved_power, saved_theta;
	static bool turn_over = true;
	switch(msg) {
		/*
		case WM_CREATE:
			d = CreateWindow("BUTTON", NULL, WS_CHILD | WS_VISIBLE | BS_BITMAP,
				10, 10, 180, 180, hwnd, 200, hInstance, 0);
			hbit = LoadBitmap(hInstance, "Bit");
			SendMessage(d, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit);
			break;
		*/
		// right mouse button down
		// start aiming and set camera pos properly
		case WM_RBUTTONDOWN:
		{
			// if white ball is stopped
			if(turn_over){
				cue.setIsAiming();
				point_x = LOWORD(lParam);
				point_y = HIWORD(lParam);

				double coor_x = (point_x - CONTENT_WIDTH / 2) * WND_TO_3D_RATIO;
				double coor_z = (-point_y + CONTENT_HEIGHT / 2) * WND_TO_3D_RATIO;

				//ready cue
				cue.ready(coor_x, cue.getCenter( ).y, coor_z, ball[WHITE_BALL].getCenter( ));
				
				//move camera to center of cue
				D3DXVECTOR3 cue_center = cue.getCenter( ) + D3DXVECTOR3(0, CUE_VIEW_Y,0);
				camera.moveCamera(cue_center, ball[WHITE_BALL].getCenter( ));
			}
			break;
		}

		// move mouse while aiming
		// change cue angle and camera pos
		case WM_MOUSEMOVE:
		{
			if(cue.getIsAiming()){
				int dx = LOWORD(lParam) - point_x;
				point_x = LOWORD(lParam);
				
				// roate cue
				cue.rotate(dx * X_TO_THETA_RATIO);

				// move camera to center of cue
				D3DXVECTOR3 cue_center = cue.getCenter() + D3DXVECTOR3(0, CUE_VIEW_Y, 0);
				camera.moveCamera(cue_center, ball[WHITE_BALL].getCenter( ));
			}
			break;
		}

		// right mouse button up
		// quit aiming and reset camera pos
		case WM_RBUTTONUP:
		{
			cue.clearIsAiming();
			if (cue.getIsCharging()){
				cue.clearIsCharging();
				cue.setPower(0);
				KillTimer(hwnd, CHARGE_TIMER_ID);
			}
			camera.resetCamera( );
			break;
		}

		// key down
		case WM_KEYDOWN:
		{
			switch(wParam) {
				// quit game
				case VK_ESCAPE:
					::DestroyWindow(hwnd);
					break;

				// space bar
				// start 
				case VK_SPACE:
					if(cue.getIsAiming()&& !cue.getIsCharging()){
						cue.setIsCharging();
						SetTimer(hwnd, CHARGE_TIMER_ID, CHARGE_TIMER_PERIOD, NULL);
					}
					break;
				}
			break;
		}

		case WM_TIMER:
		{
			if (wParam == CHARGE_TIMER_ID) {
				cue.charge();

				D3DXVECTOR3 cue_center = cue.getCenter();
				cue_center.y += CUE_VIEW_Y;
				camera.moveCamera(cue_center, ball[WHITE_BALL].getCenter());
			}
			else if (wParam == SHOOT_TIMER_ID) {
				if (cue.hasIntersected(ball[WHITE_BALL])) {
					cue.clearIsAiming();
					ball[WHITE_BALL].setPower(saved_power*cos(saved_theta), 0,saved_power*sin(saved_theta));
					KillTimer(hwnd, SHOOT_TIMER_ID);
					// turn started
					turn_over = false; 
					SetTimer(hwnd, TURN_OVER_TIMER_ID, TURN_OVER_TIMER_PERIOD, NULL);
				}
				else cue.discharge();
			}
			else if (wParam == TURN_OVER_TIMER_ID) {
				int i;
				for (i = 0; i < NUM_BALL; i++)
					if (ball[i].getVelocity_X() != 0 || ball[i].getVelocity_Z() != 0) break;
				
				if (i == NUM_BALL) {
					int result = CManager::GetInstance()->finishTurn();
					KillTimer(hwnd, TURN_OVER_TIMER_ID);
					cout << "turn finished" << endl;

					if (result == )

					else if (result != CManager::CONTINUE) {
						string result_str;
						switch (result) {
						case CManager::PLAYER1_WIN_CLEAR: 
						case CManager::PLAYER1_WIN_BY_PLAYER2_BLACKBALL:
						case CManager::PLAYER1_WIN_BY_PLAYER2_WHITEBALL:
							result_str = "PLAYER 1 WIN"; break; 
						case CManager::PLAYER2_WIN_CLEAR:
						case CManager::PLAYER2_WIN_BY_PLAYER1_BLACKBALL:
						case CManager::PLAYER2_WIN_BY_PLAYER1_WHITEBALL:
							result_str = "PLAYER 2 WIN"; break;
						}
						::MessageBox(hwnd, result_str.c_str(), "Game Over", 0);
						exit(0);
					}

					turn_over = true;
				}
			}
			break;
		}

		case WM_KEYUP:
		{
			// shooting
			if(wParam == VK_SPACE){
				if(cue.getIsAiming() && cue.getIsCharging()){
					// kill charge timer
					KillTimer(hwnd, CHARGE_TIMER_ID);

					cue.clearIsCharging();

					// save power and theta
					saved_power = cue.getPower();
					saved_theta = cue.getTheta();

					// set timer for shoot animation
					SetTimer(hwnd, SHOOT_TIMER_ID, SHOOT_TIMER_PERIOD, NULL);
				}
			}
			break;
		}

		case WM_DESTROY:
			::PostQuitMessage(0);
			break;
	}

	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	hInstance = hinstance;
	srand(static_cast<unsigned int>(time(NULL)));

	AllocConsole( );
	freopen("CONOUT$", "wt", stdout);
	printf("start\n");
	
	if(!d3d::InitD3D(hinstance, WINDOW_WIDTH, WINDOW_HEIGHT, true, D3DDEVTYPE_HAL, &Device)){
		::MessageBox(0, "InitD3D() - FAILED", 0, 0);
		return 0;
	}

	if(!Setup( )) { ::MessageBox(0,"Setup() - FAILED", 0, 0); return 0; }

	d3d::EnterMsgLoop(Display);

	Cleanup( );

	Device->Release( );

	return 0;
}