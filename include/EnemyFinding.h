#pragma once
#include <iostream>
#include <vector>
#include "Champ.h"
#include "Grid.h"
struct HexNode{
    GridPos parent;
    GridPos pos;
    int G;
    int H;
    int F;
    HexNode(GridPos pos): pos(pos){};
};
std::vector<GridPos> neighbours(GridPos pos);
void MoveCloser(ChampState &champ, ChampState &target, std::vector<ChampState> &AllyTeam, std::vector<ChampState> &EnemyTeam);
void FindClosestEnemy(ChampState &Champ, std::vector<ChampState> &AllyTeam, std::vector<ChampState> &EnemyTeam);