#pragma once

const unsigned long MAX_SHIPS = 500;
const unsigned long MAX_DROPOFFS = 10;
const unsigned long MAX_DEPTH = 10;
const unsigned long MAX_CLUSTER_SIZE = 30;

// linear num of sectors
const int SECTOR_ROW = 4;
const int NUM_SECTORS = SECTOR_ROW*SECTOR_ROW;
const int SECTOR_SIDE[5] = {32/SECTOR_ROW, 40/SECTOR_ROW, 48/SECTOR_ROW, 56/SECTOR_ROW, 64/SECTOR_ROW};

const int MOVE_COST_RATIO = 10;
const int FULL_SHIP = 900;
const unsigned long SPAWN_COST = 1000;
const int EXTRACTION_RATIO = 4;
const int MAX_CARGO = 1000;
const int INSPIRATION_BONUS = 2;
const int INSPIRATION_RADIUS = 4;
const unsigned long INSPIRATION_SHIP_COUNT = 2;

// ship tasks (~ order priority)
const int NONE = -1;
const int DELIVER = 0;
const int GOTO = 1;
const int MINE = 2;

const int MODE_2P = 0, MODE_4P = 1;


// (map_size - 32)/8 -> 0, 1, 2, 3, 4

const int MAX_DROPOFFS_2P[5] = {1, 1, 2, 4, 6};
const int MAX_DROPOFFS_4P[5] = {1, 1, 2, 3, 4};

const int MIN_SHIPS_2P[5] = {25, 40, 45, 55, 55};
const int MAX_SHIPS_2P[5] = {30, 45, 60, 80, 80};

const int MIN_SHIPS_4P[5] = {20, 25, 30, 40, 40};
const int MAX_SHIPS_4P[5] = {25, 30, 40, 50, 50};
