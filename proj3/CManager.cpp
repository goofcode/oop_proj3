#include "CManager.h"

CManager* CManager::Manager = nullptr;

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

	if (!firstGoal)
		if (ball.getBallType() == TYPE_SOLID_BALL || ball.getBallType() == TYPE_STRIPE_BALL) 
			firstGoal = true;
	else
		updateScore(ball.getBallType());
}

int CManager::finishTurn()
{
	if (blackball_goal_in_turn) {

		if(score[turn] == 7){
			if (solid_goal_in_turn + stripe_goal_in_turn != 0)
				return turn == PLAYER1 ? PLAYER2_WIN_BY_PLAYER1_BLACKBALL : PLAYER1_WIN_BY_PLAYER2_BLACKBALL;
			return turn == PLAYER1 ? PLAYER1_WIN_CLEAR : PLAYER2_WIN_CLEAR;
		}
		else
			return turn == PLAYER1 ? PLAYER2_WIN_BY_PLAYER1_BLACKBALL : PLAYER1_WIN_BY_PLAYER2_BLACKBALL;
	}	
	else if (whiteball_goal_in_turn) {
		health[turn] --;
		if (health[turn] == 0) return turn == PLAYER1 ? PLAYER2_WIN_BY_PLAYER1_WHITEBALL : PLAYER1_WIN_BY_PLAYER2_WHITEBALL;

		turnover();
		solid_goal_in_turn = stripe_goal_in_turn = whiteball_goal_in_turn = 0;
		return CONTINUE;
	}
	// normal balls goals in turn
	else{
		// if first goal
		if (firstGoal && !firstGoalTurn) {
			firstGoalTurn = true;
			if (solid_goal_in_turn == 0 || stripe_goal_in_turn == 0) {
				ball_type[turn] = solid_goal_in_turn != 0 ? TYPE_SOLID_BALL : TYPE_STRIPE_BALL;
				ball_type[theOther(turn)] = solid_goal_in_turn != 0 ? TYPE_STRIPE_BALL: TYPE_SOLID_BALL ;
			}
			else {
				// 만들어야됨
				ball_type[turn] = TYPE_SOLID_BALL;
				ball_type[theOther(turn)] = TYPE_STRIPE_BALL;
			}

			updateScore(TYPE_SOLID_BALL, solid_goal_in_turn);
			updateScore(TYPE_STRIPE_BALL, stripe_goal_in_turn);
			solid_goal_in_turn = stripe_goal_in_turn = whiteball_goal_in_turn = 0;
			return CONTINUE;
		}

		// case of continue
		else if ((solid_goal_in_turn == 0 && stripe_goal_in_turn != 0 && ball_type[turn] == TYPE_STRIPE_BALL)
			|| (solid_goal_in_turn != 0 && stripe_goal_in_turn == 0 && ball_type[turn] == TYPE_SOLID_BALL)
			|| (solid_goal_in_turn != 0 && stripe_goal_in_turn != 0)) {
			solid_goal_in_turn = stripe_goal_in_turn = whiteball_goal_in_turn = 0;
			return CONTINUE;
		}

		// case of turn over
		else if ((solid_goal_in_turn == 0 && stripe_goal_in_turn != 0 && ball_type[turn] == TYPE_SOLID_BALL)
			|| (solid_goal_in_turn != 0 && stripe_goal_in_turn == 0 && ball_type[turn] == TYPE_STRIPE_BALL)
			|| (solid_goal_in_turn == 0 && stripe_goal_in_turn == 0)) {
			turnover();
			solid_goal_in_turn = stripe_goal_in_turn = whiteball_goal_in_turn = 0;
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
