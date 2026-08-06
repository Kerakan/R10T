#pragma once
#include <iostream>
#include <vector>
#include "Champ.h"
#include "Grid.h"
#include "Log.h"
#include <algorithm>
#include <string>
#include "EnemyFinding.h"
std::vector<GridPos> neighbours(GridPos pos){
    std::vector<GridPos> result;
    result.push_back(GridPos(pos.q + 1, pos.r));
    result.push_back(GridPos(pos.q - 1, pos.r));
    result.push_back(GridPos(pos.q, pos.r + 1));
    result.push_back(GridPos(pos.q, pos.r - 1));
    result.push_back(GridPos(pos.q + 1, pos.r - 1));
    result.push_back(GridPos(pos.q - 1, pos.r + 1));
    return result;
};
void MoveCloser(ChampState &champ, ChampState &target, std::vector<ChampState> &AllyTeam, std::vector<ChampState> &EnemyTeam){
    GridPos startpos = champ.pos;
    GridPos endpos = target.pos;
    std::vector<HexNode> possible_moves;
    GridPos currentmovingpos;
    int minF = 10000;
    int minH;
    //Find the Moves that can be executed
    for (GridPos move: neighbours(champ.pos)){
        if (std::find(GridTeam1.begin(),GridTeam1.end(),move)!=GridTeam1.end() or std::find(GridTeam2.begin(),GridTeam2.end(),move)!=GridTeam2.end() and !(move.is_occupied)){
            possible_moves.push_back(HexNode(move));
        }
    }
    //Apply the values for the A* pathfinding
    for (HexNode& move: possible_moves){
        move.G = 1;
        move.H = distance(move.pos, endpos);
        move.F = move.H + move.G;
    }
    //Find the best hex to move to
    for (HexNode& move: possible_moves){
        if (move.F < minF){
            int minH = move.H;
            currentmovingpos = move.pos;
        }
        else if(move.F == minF){
            if (move.H < minH){
                minH = move.H;
                minF = move.F;
                currentmovingpos = move.pos;
            }
        }
    }
    if (distance(currentmovingpos, endpos) < distance(startpos,endpos)){
        champ.pos.is_occupied = false;
        champ.pos = currentmovingpos;
    }
};
void FindClosestEnemy(ChampState &Champ, std::vector<ChampState> &AllyTeam, std::vector<ChampState> &EnemyTeam){
    GridPos champpos = Champ.pos;
    Champ.enemytarget = &EnemyTeam[0];
    int closest_distance = 1000;
    if (Champ.def.Targeting == TargetingMethod::LowestHP){
        float lowest_hp =EnemyTeam[0].hp_current;
        ChampState* target = Champ.enemytarget;
        for (ChampState &Enemy: EnemyTeam){
            if (Enemy.is_dead) continue;
            if (Enemy.hp_current < lowest_hp){
                lowest_hp = Enemy.hp_current;
                Champ.enemytarget = &Enemy;
            }
        }
    }
    else if(Champ.def.Targeting == TargetingMethod::Closest){    
        for (ChampState &Enemy: EnemyTeam){
            if (Enemy.is_dead) continue;
            GridPos enemypos = Enemy.pos;
            int dis = distance(champpos,enemypos);
            if (dis < closest_distance){
                closest_distance = dis;
                Champ.enemytarget = &Enemy;
            }
        }
    }
    if (closest_distance<=Champ.range){
        Log("Champion " + Champ.def.name + " found enemy " + Champ.enemytarget->def.name);
    }
    else{
        Log("Champion " + Champ.def.name + " is moving closer to enemy " + Champ.enemytarget->def.name);
        MoveCloser(Champ, *Champ.enemytarget, AllyTeam, EnemyTeam);
        Champ.enemytarget = nullptr;
    }
}