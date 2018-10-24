#pragma once

#include <basetypes.h>

/*
 *  Actions:
 *  MOVE: m id dir = {o,n,e,s,w}
 *  DROPOFF: c id 
 * 
 *  SPAWN: g
 */

const int moves[5] = {0, 1, 2, 3, 4};
const char moves_str[5] = {'o', 'n', 'e', 's', 'w'};
const Point moves_dir[5] = {Point(0, 0), Point(0, -1), Point(1, 0), Point(0, 1), Point(-1, 0)};
