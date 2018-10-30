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