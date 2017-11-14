#pragma once
#include "d3dUtility.h"
#include "CSphere.h"

#define PLAYER1 0
#define PLAYER2 1


class CManager
{
private:
	int score[2] = { 0,0 };
	int health[2] = { 3,3 };
	int ball_type[2] = { -1,-1 };
	int turn = PLAYER1;
	bool firstGoal = false;
	bool firstGoalTurn = false;
	int solid_goal_in_turn = 0;
	int stripe_goal_in_turn = 0;
	bool blackball_goal_in_turn = false;
	bool whiteball_goal_in_turn = false;
	int firsthit_in_turn = 0;
	CManager();
	~CManager() {};
	CManager(const CManager& other);

public:
	static CManager* GetInstance() {
		if (Manager == NULL) Manager = new CManager();
		return Manager;
	}

	const static enum {
		PLAYER1_WIN_CLEAR,
		PLAYER1_WIN_BY_PLAYER2_BLACKBALL,
		PLAYER1_WIN_BY_PLAYER2_WHITEBALL,
		CONTINUE,
		PLAYER2_WIN_CLEAR,
		PLAYER2_WIN_BY_PLAYER1_BLACKBALL,
		PLAYER2_WIN_BY_PLAYER1_WHITEBALL
	} game_status;
	int getBallType(int playernum);
	int getTurn(); 
	int getScore(int playernum);
	void goal(const CSphere& ball);
	int finishTurn();
	void first_hit_in_turn(const CSphere& ball);
private:

	static CManager *Manager;

	int theOther(int turn);
	void turnover();
	void updateScore(int balltype, int num =1);
};


