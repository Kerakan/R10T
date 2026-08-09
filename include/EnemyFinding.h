#pragma once
#include <iostream>
#include <vector>
#include "Champ.h"
#include "Grid.h"
struct HexNode{
    HexNode* parent = nullptr;
    GridPos pos;
    int G;
    int H;
    int F;
    HexNode(GridPos pos): pos(pos){};
};
GridPos* findGridPos(GridPos pos);
std::vector<GridPos*> neighbours(GridPos pos);
std::vector<GridPos> AStar(GridPos start, GridPos end, int range);
void MoveCloser(ChampState &champ, ChampState &target, std::vector<ChampState> &AllyTeam, std::vector<ChampState> &EnemyTeam);
void FindClosestEnemy(ChampState &Champ, std::vector<ChampState> &AllyTeam, std::vector<ChampState> &EnemyTeam);