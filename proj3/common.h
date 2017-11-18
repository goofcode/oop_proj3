#pragma once

#include <d3dx9.h>
#include <random>
#include <string>
#include <limits>
#include <iostream>
#include <iomanip>
#include <sstream>

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768
#define CONTENT_WIDTH 1007
#define CONTENT_HEIGHT 729

// global defines
#define PI 3.14159265f
#define EPSILON 0.01f

#define NUM_BALL 16
#define WHITE_BALL 0
#define BLACK_BALL 8

using std::cout;
using std::endl;

using std::string;
using std::ostringstream;
using std::to_string;

const D3DXCOLOR	DX_WHITE(D3DCOLOR_XRGB(255, 255, 255));

inline float distance(float x1, float y1, float x2, float y2) { return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2)); }
inline float angle(float x1, float y1, float x2, float y2) { return atan2(y2 - y1, x2 - x1);}

