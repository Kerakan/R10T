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
std::vector<GridPos> AStar(GridPos start, GridPos end, int range){
    std::vector<HexNode> openlist;
    std::vector<HexNode> closedlist;
    HexNode startnode(start);
    HexNode endnode(end);
    startnode.G = 0;
    std::vector<GridPos> path;
    openlist.push_back(startnode);
    while (!openlist.empty()){
        HexNode current = openlist.front();
        openlist.erase(openlist.begin());
        closedlist.push_back(current);
        if (distance(current.pos, end) <= range){
            Log("Path has been found");
            path = {};
            while (current.parent->pos != start){
                path.push_back(current.parent->pos);
                current = *current.parent;
            }
            std::reverse(path.begin(), path.end());
            return path;
        }
        std::vector<GridPos> possible_moves = neighbours(current.pos);
        HexNode nextmove(GridPos(0, 0));
        int minF = 1000;
        for (GridPos& move: possible_moves){
            if (move.is_occupied or std::find(closedlist.begin(), closedlist.end(), move) != closedlist.end()) continue;
            HexNode node(move);
            node.parent = &current;
            node.G = current.G + 1;
            node.H = distance(move,end);
            node.F = node.G + node.H;
            if (node.F == minF){
                if (node.H <  nextmove.H){
                    nextmove = node;
                }
            }
            else if (node.F < minF){
                minF = node.F;
                nextmove = node;
            }
        }
        openlist.push_back(nextmove);
    }
    return {};
}
void MoveCloser(ChampState &champ, ChampState &target, std::vector<ChampState> &AllyTeam, std::vector<ChampState> &EnemyTeam){
    std::vector<GridPos> path = AStar(champ.pos, target.pos, champ.range);
    if(!path.empty()){
        GridPos pos = path[0];
        champ.pos = pos;
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