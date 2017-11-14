#include "CManager.h"

CManager* CManager::Manager = nullptr;
void CManager::first_hit_in_turn(const CSphere & ball)
{
	if (firsthit_in_turn == 0 && (blackball_goal_in_turn == true || ball.getBallType() == ball_type[theOther(turn)])) {
		if (ball_type[turn] == -1)
			firsthit_in_turn = 1;
		else
			firsthit_in_turn = 2;
	}
	else
		firsthit_in_turn = 1;
}

void CManager::showMessage(IDirect3DDevice9* Device, int seconds, string message)
{
	

}

void CManager::showGameInfo(IDirect3DDevice9 * Device)
{
	
	if (Sprite) Sprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE);

	RECT score_rect[2] = { { 0,0,0,0 },{ 0, 30, 0, 0 } },
		ball_type_rect[2] = { { 900, 0, 0,0 },{ 900, 30, 0, 0 } },
		turn_rect = { 600,0,0,0 };

	for (int i = 0; i < 2; i++)
	{
		ostringstream sstream;
		sstream << "Player " << i + 1 << ": " << getScore(i + 1) << " ball(s)";
		Font->DrawTextA(Sprite, sstream.str().c_str(), -1, &score_rect[i], DT_NOCLIP, D3DCOLOR_XRGB(0, 0, 0));

		int ball_type = getBallType(i + 1);
		if (ball_type != -1)
			Font->DrawTextA(Sprite, (ball_type == TYPE_SOLID_BALL ? "SOLID" : "STRIPE"), -1, &ball_type_rect[i], DT_NOCLIP, D3DCOLOR_XRGB(0, 0, 0));
	}
	Font->DrawTextA(Sprite, getTurn() == 1 ? "PLAYER 1 Turn" : "PLAYER 2 Turn", -1, &turn_rect, DT_NOCLIP, D3DCOLOR_XRGB(0, 0, 0));
	Sprite->End();

	Device->EndScene();
	Device->Present(0, 0, 0, 0);
	Device->SetTexture(0, NULL);
}

void CManager::createFont(IDirect3DDevice9 * Device)
{
	D3DXCreateFont(Device, 25, 0, FW_BOLD, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("굴림체"), &Font);
	D3DXCreateSprite(Device, &Sprite);
}



int CManager::getBallType(int playernum)
{
	if (firstGoal) return ball_type[playernum-1];
	else return - 1;
}

int CManager::getTurn()
{
	if (turn == PLAYER1) return 1;
	else if (turn == PLAYER2) return 2;
	else return -1;
}

int CManager::getScore(int playernum)
{
	return score[playernum - 1];
}

//처음 들어갔을 때 볼 타입 결정 두개이상 동시시 스코어계산때 체크박스 띄울 예정
void CManager::goal(const CSphere & ball)
{
	if (ball.getBallType() == TYPE_BLACK_BALL) blackball_goal_in_turn = true;
	if (ball.getBallType() == TYPE_WHITE_BALL) whiteball_goal_in_turn = true;
	if (ball.getBallType() == TYPE_SOLID_BALL) solid_goal_in_turn++;
	if (ball.getBallType() == TYPE_STRIPE_BALL) stripe_goal_in_turn++;

	if (!firstGoal) {
		if (ball.getBallType() == TYPE_SOLID_BALL || ball.getBallType() == TYPE_STRIPE_BALL)
			firstGoal = true;
	}
	else {
		updateScore(ball.getBallType());
	}
}

int CManager::finishTurn()
{
	if ((firsthit_in_turn == 0)|| (firsthit_in_turn == 2)) {
		turnover();
		solid_goal_in_turn = stripe_goal_in_turn = whiteball_goal_in_turn = firsthit_in_turn = 0;
		return FREEBALL;
	}
	if (blackball_goal_in_turn) {

		if (score[turn] == 7) {
			if (solid_goal_in_turn + stripe_goal_in_turn != 0)
				return turn == PLAYER1 ? PLAYER2_WIN_BY_PLAYER1_BLACKBALL : PLAYER1_WIN_BY_PLAYER2_BLACKBALL;
			return turn == PLAYER1 ? PLAYER1_WIN_CLEAR : PLAYER2_WIN_CLEAR;
		}
		else
			return turn == PLAYER1 ? PLAYER2_WIN_BY_PLAYER1_BLACKBALL : PLAYER1_WIN_BY_PLAYER2_BLACKBALL;
	}
	else if (whiteball_goal_in_turn) {
		health[turn] --;
		cout << health[turn];
		if (health[turn] == 0) return turn == PLAYER1 ? PLAYER2_WIN_BY_PLAYER1_WHITEBALL : PLAYER1_WIN_BY_PLAYER2_WHITEBALL;

		turnover();
		solid_goal_in_turn = stripe_goal_in_turn = whiteball_goal_in_turn = firsthit_in_turn = 0;
		return FREEBALL;
	}
	// normal balls goals in turn

	else {
		// if first goal
		if (firstGoal && !firstGoalTurn) {
			firstGoalTurn = true;
			if (solid_goal_in_turn == 0 || stripe_goal_in_turn == 0) {
				ball_type[turn] = solid_goal_in_turn != 0 ? TYPE_SOLID_BALL : TYPE_STRIPE_BALL;
				ball_type[theOther(turn)] = solid_goal_in_turn != 0 ? TYPE_STRIPE_BALL : TYPE_SOLID_BALL;
			}
			else {
				// 만들어야됨
				ball_type[turn] = TYPE_SOLID_BALL;
				ball_type[theOther(turn)] = TYPE_STRIPE_BALL;
			}

			updateScore(TYPE_SOLID_BALL, solid_goal_in_turn);
			updateScore(TYPE_STRIPE_BALL, stripe_goal_in_turn);
			solid_goal_in_turn = stripe_goal_in_turn = whiteball_goal_in_turn = firsthit_in_turn = 0;
			return CONTINUE;
		}

		// case of continue
		else if ((firsthit_in_turn == 1) && (solid_goal_in_turn == 0 && stripe_goal_in_turn != 0 && ball_type[turn] == TYPE_STRIPE_BALL)
			|| (solid_goal_in_turn != 0 && stripe_goal_in_turn == 0 && ball_type[turn] == TYPE_SOLID_BALL)
			|| (solid_goal_in_turn != 0 && stripe_goal_in_turn != 0)) {
			solid_goal_in_turn = stripe_goal_in_turn = whiteball_goal_in_turn = firsthit_in_turn = 0;
			return CONTINUE;
		}

		// case of turn over
		else if ((firsthit_in_turn == 1) && (solid_goal_in_turn == 0 && stripe_goal_in_turn != 0 && ball_type[turn] == TYPE_SOLID_BALL)
			|| (solid_goal_in_turn != 0 && stripe_goal_in_turn == 0 && ball_type[turn] == TYPE_STRIPE_BALL)
			|| (solid_goal_in_turn == 0 && stripe_goal_in_turn == 0)) {
			turnover();
			solid_goal_in_turn = stripe_goal_in_turn = whiteball_goal_in_turn = firsthit_in_turn = 0;
			return CONTINUE;
		}
		else {
			cout << "Please check calculate_score() logic " << endl;
			return CONTINUE;
		}
	}
}

CManager::CManager(){}

int CManager::theOther(int turn)
{
	return turn==0?1:0;
}
void CManager::turnover()
{
	turn = turn == PLAYER1 ? PLAYER2 : PLAYER1;
}
void CManager::updateScore(int balltype, int num)
{
	if (balltype == ball_type[PLAYER1]) score[PLAYER1] += num;
	else if (balltype == ball_type[PLAYER2]) score[PLAYER2] += num;
}
