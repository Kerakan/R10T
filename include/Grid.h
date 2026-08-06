#pragma once
#include <iostream>
#include <vector>
struct GridPos{
        int q;
        int r;
        int s = -q - r;
        bool is_occupied = false;
        GridPos() = default;
        GridPos(int q, int r): q(q), r(r){}
        bool operator==(const GridPos& other) const{
                return (q==other.q and r==other.r);
        };
};
int distance (GridPos a, GridPos b);
std::vector<GridPos> Trajectory(GridPos a, GridPos b);
bool is_valid(GridPos a, std::vector<GridPos> TeamGrid);
inline std::vector<GridPos> GridTeam2 = {GridPos(0,-3),GridPos(1,-3),GridPos(2,-3),GridPos(3,-3),GridPos(-1,-2),GridPos(0,-2),GridPos(1,-2),GridPos(2,-2), GridPos(3,-2),GridPos(-1,-1),GridPos(0,-1),GridPos(1,-1),GridPos(2,-1)};
inline std::vector<GridPos> GridTeam1 = {GridPos(-2,0),GridPos(-1,0),GridPos(0,0),GridPos(1,0),GridPos(-3,1),GridPos(-2,1),GridPos(-1,1),GridPos(0,1),GridPos(1,1),GridPos(-3,2),GridPos(-2,2),GridPos(-1,2),GridPos(0,2)};