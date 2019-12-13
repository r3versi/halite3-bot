#pragma once

const unsigned long MAX_SHIPS = 500;
const unsigned long MAX_DROPOFFS = 25;
const unsigned long MAX_DEPTH = 10;
const unsigned long MAX_CLUSTER_SIZE = 30;

const float TIME_LIMIT = 500.f;

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
const int FOUND = 0;
const int DELIVER = 1;
const int TRAVEL = 2;
const int MINE = 3;

const int MODE_2P = 0, MODE_4P = 1;


// (map_size - 32)/8 -> 0, 1, 2, 3, 4

const float LOWER_BOUND_2P[5] = {131.3f, 120.9f, 119.7f, 117.1f, 127.8f};
const float UPPER_BOUND_2P[5] = {229.4f, 199.6f, 188.8f, 173.1f, 219.0f};

const float LOWER_BOUND_4P[5] = {149.5f, 127.5f, 128.9f, 119.8f, 144.5f};
const float UPPER_BOUND_4P[5] = {277.1f, 232.1f, 222.9f, 197.9f, 263.0f};

const int MAX_DROPOFFS_2P[5] = {2, 2, 3, 4, 6};
const int MAX_DROPOFFS_4P[5] = {1, 1, 2, 3, 5};

const int MIN_SHIPS_2P[5] = {25, 30, 50, 70, 100};
const int MAX_SHIPS_2P[5] = {40, 50, 60, 90, 150};

const int MIN_SHIPS_4P[5] = {20, 25, 35, 55, 55};
const int MAX_SHIPS_4P[5] = {35, 40, 50, 70, 80};
