//
//  common.h
//  Tetris
//
//  Created by Svenja on 2/19/2014.
//  Copyright (c) 2014 Shan Cao. All rights reserved.
//

#ifndef Tetris_common_h
#define Tetris_common_h
#include "Angel.h"

const int WINDOW_HEIGHT = 512;
const int WINDOW_WIDTH = 256;

const float SCALE_X = 1; // 0.9
const float SCALE_Y = 1; // 0.95
const float LEFT_BOUND = -1 * SCALE_X;
const float RIGHT_BOUND = 1 * SCALE_X;
const float BOTTOM_BOUND = -1 * SCALE_Y;
const float TOP_BOUND = 1 * SCALE_Y;
const float EDGE_LENGTH_X = 0.2 * SCALE_X;
const float EDGE_LENGTH_Y = 0.1 * SCALE_Y;

const float TIME_INTERVAL = 500.0;

const int POINTS = 900;
const int POINT_OFFSET = 0;
const int COLOR_OFFSET =  900 * sizeof(vec2);
const int GRID_POINTS = 64;
const int GRID_COLORS = 64;

const int ESCAPE = 033;
const char QUIT_UPPER = 'Q';
const char QUIT_LOWER = 'q';
const char LEFT = 'l';
const char RIGHT = 'r';
const char DOWN = 'd';
const char UP = 'u';

const int MAX_X = 4;
const int MAX_Y = 9;
const int MID_X = 0;
const int MID_Y = 0;
const int MIN_X = -5;
const int MIN_Y = -10;
const int X_CALIBRATE = 5;
const int Y_CALIBRATE = 10;

const bool SUCCESS = true;
const bool FAILED = false;

const int O_TILE = 0;
const int I_TILE = 1;
const int S_TILE = 2;
const int Z_TILE = 3;
const int L_TILE = 4;
const int J_TILE = 5;
const int T_TILE = 6;
const char type[7] = {'o', 'i', 's', 'z', 'l', 'j', 't'};

const int BLANK = -1;
const vec4 backgroundColor = vec4(0.15, 0.16, 0.13, 1.0);
const vec4 gridColor = vec4(0.27, 0.28, 0.25, 1.0);
const vec4 boundColor = gridColor;// vec4(1.0, 1.0, 1.0, 1.0);
const vec4 tileColor[7] = {
    // Monokai
    vec4(0.97, 0.97, 0.95, 1.0), // white
    vec4(0.99, 0.12, 0.44, 1.0), // red
    vec4(1.00, 0.60, 0.00, 1.0), // orange
    vec4(0.90, 0.86, 0.43, 1.0), // yellow
    vec4(0.64, 0.89, 0.01, 1.0), // green
    vec4(0.38, 0.85, 0.95, 1.0), // blue
    vec4(0.69, 0.49, 1.00, 1.0)  // purple
};


#endif
