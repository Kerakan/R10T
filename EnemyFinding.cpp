#pragma once
#include <iostream>
#include <vector>
#include "Champ.h"
#include "Grid.h"
#include "Log.h"
#include <algorithm>
#include <deque>
#include <string>
#include "EnemyFinding.h"
GridPos* findGridPos(GridPos pos){
    for (GridPos& tile : GridTeam1){
        if (tile.q == pos.q && tile.r == pos.r) return &tile;
    }
    for (GridPos& tile : GridTeam2){
        if (tile.q == pos.q && tile.r == pos.r) return &tile;
    }
    return nullptr;
}
std::vector<GridPos*> neighbours(GridPos pos){
    std::vector<GridPos*> result;
    GridPos candidates[6] = {
        GridPos(pos.q + 1, pos.r),
        GridPos(pos.q - 1, pos.r),
        GridPos(pos.q, pos.r + 1),
        GridPos(pos.q, pos.r - 1),
        GridPos(pos.q + 1, pos.r - 1),
        GridPos(pos.q - 1, pos.r + 1)
    };
    for (GridPos& c : candidates){
        GridPos* real = findGridPos(c);
        if (real != nullptr) result.push_back(real);
    }
    return result;
}
std::vector<GridPos> AStar(GridPos start, GridPos end, int range){
    std::vector<HexNode> openlist;
    std::deque<HexNode> closedlist;
    HexNode startnode(start);
    HexNode endnode(end);
    startnode.G = 0;
    std::vector<GridPos> path;
    openlist.push_back(startnode);
    while (!openlist.empty()){
        auto best = std::min_element(openlist.begin(), openlist.end(),
            [](const HexNode& a, const HexNode& b){
                if (a.F != b.F) return a.F < b.F;
                return a.H < b.H;
            });
        HexNode current = *best;
        openlist.erase(best);
        closedlist.push_back(current);
        HexNode* currentPtr = &closedlist.back();
        if (distance(current.pos, end) <= range){
            path = {};
            path.push_back(currentPtr->pos);
            HexNode* node = currentPtr;
            while (node->parent != nullptr && !(node->parent->pos == start)){
                node = node->parent;
                path.push_back(node->pos);
            }
            std::reverse(path.begin(), path.end());
            return path;
        }
        std::vector<GridPos*> possible_moves = neighbours(current.pos);
        HexNode nextmove(GridPos(0, 0));
        int minF = 1000;
        for (GridPos* move: possible_moves){
            if (move->is_occupied or std::find_if(closedlist.begin(), closedlist.end(), [move](const HexNode& n){ return n.pos == *move; }) != closedlist.end()) continue;
            HexNode node(*move);
            node.parent = currentPtr;
            node.G = current.G + 1;
            node.H = distance(*move,end);
            node.F = node.G + node.H;
            openlist.push_back(node);
        }
    }
    return {};
}
void MoveCloser(ChampState &champ, ChampState &target, std::vector<ChampState> &AllyTeam, std::vector<ChampState> &EnemyTeam){
    std::vector<GridPos> path = AStar(champ.pos, target.pos, champ.range);
    if(!path.empty() and (seconds_in_combat - champ.time_since_lastmove)>= 0.5f){
        Log("Champion " + champ.def.name + " is moving closer to enemy " + champ.enemytarget->def.name);
        GridPos* oldTile = findGridPos(champ.pos);
        if (oldTile != nullptr) oldTile->is_occupied = false;
        GridPos pos = path[0];
        champ.pos = pos;
        GridPos* newTile = findGridPos(champ.pos);
        if (newTile != nullptr) newTile->is_occupied = true;
        champ.time_since_lastmove = seconds_in_combat;
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
    if (closest_distance<=Champ.range) return;
    else{
        MoveCloser(Champ, *Champ.enemytarget, AllyTeam, EnemyTeam);
    }
}
