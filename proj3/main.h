#pragma once

#include "common.h"
#include "CTable.h"
#include "CSphere.h"
#include "CLight.h"
#include "CCue.h"
#include "CCamera.h"
#include "CManager.h"

#define WND_TO_3D_RATIO 0.013f
#define X_TO_THETA_RATIO 0.01f

#define CHARGE_TIMER_ID 1
#define CHARGE_TIMER_PERIOD 10

#define SHOOT_TIMER_ID 2
#define SHOOT_TIMER_PERIOD 10

#define TURN_OVER_TIMER_ID 3
#define TURN_OVER_TIMER_PERIOD 10

// depending on the computer playing game,
// time goes too fast or too slow
// if game speed is not good, please modify this value.
#define TIME_CORRECTION_RATIO 0.8f