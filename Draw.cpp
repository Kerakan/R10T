#pragma once
#include "raylib.h"
#include "Grid.h"
#include "Champ.h"
#include "Traits.h"
#include <cmath>
#include "Log.h"
#include <utility>
#include "Combat.h"
#include "Draw.h"
#include <algorithm>
void DrawDamage(){
    float current_time = seconds_in_combat;
    int size = TimedTexts.size();
    for (int i = 0; i<size ; i++){
        if (TimedTexts[i].defined_until < current_time){
            continue;
        }
        std::string text = TimedTexts[i].text + " DMG";
        DrawText(text.c_str(), 750 + 75 * (TimedTexts[i].q * sqrt(3) + TimedTexts[i].r * sqrt(3)/2) + 60, 537.5 + TimedTexts[i].r * 112.5 - 20, 12, RED);
    }
}
void DrawInterfaceBackGround(int width, int height){
    ClearBackground(BLACK);
    DrawText("Press A/D to Accelerate/Decelerate", 10, height-30, 20, WHITE);
    DrawFPS(width-100,20);
};
void DrawSecondsInCombat(int width, int height, int seconds){
    DrawRectangleLines((width/2)-150, height-150, 300, 150, WHITE);
    DrawRectangle((width/2)-149, height-149, 298, 148, YELLOW);
    DrawText(std::to_string(seconds).c_str(),(width/2)-20,(height-100),50,WHITE);
}
void DrawStartButton(int width, int height){
    if (!combat_started){
        DrawRectangleLines((width/2)-150, height-150, 300, 150, WHITE);
        DrawRectangle((width/2)-149, height-149, 298, 148, BLUE);
        DrawText("START",(width/2)-85,(height-100),50,WHITE);
        Vector2 MousePos = GetMousePosition();
        Rectangle btnBounds = {(width/2)-150, height-150, 300, 150};
        if (CheckCollisionPointRec(MousePos,btnBounds)){
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                combat_started = true;
                GamePhase = GameState::Combat;
                ApplyTraits();
            }
        }
    }
}
void DrawHexagon(Vector2 center,Color color){
    DrawPolyLines(center,6,75,90,color);
}
void DrawGrid(int width, int height){
    //Draw the board converting axial coordinates to pixels:
    for (GridPos H : GridTeam2){
        int q = H.q;
        int r = H.r;
        Vector2 c={750 + 75 * (q * sqrt(3) + r * sqrt(3)/2), 537.5 + r * 112.5};
        DrawHexagon(c,YELLOW);
    }
    for (GridPos H : GridTeam1){
        int q = H.q;
        int r = H.r;
        Vector2 c={750 + 75 * (q * sqrt(3) + r * sqrt(3)/2), 537.5 + r * 112.5};
        DrawHexagon(c,RED);
    }
}
void DrawHealthBar(int Cx, int Cy, int rad,ChampState& champion){
    //Draw the outline of the hp bar
    float w = 2 * rad;
    float l = rad *(1.0f/4);
    DrawRectangleLines(Cx-rad,Cy+rad, w, l, WHITE);
    float hp_mult = std::max(champion.hp_current/champion.hp_max,0.0f);
    //DrawHealth
    DrawRectangle(Cx-rad+1,Cy+rad+1,  (w - 2)*hp_mult, l - 2, GREEN);
    //DrawShield
    float shield_mult = std::max(champion.current_shield/champion.hp_max,0.0f);
    DrawRectangle(Cx-rad+1+(w - 2)*hp_mult,Cy+rad+1,  (w - 2)*hp_mult*shield_mult, l - 2, WHITE);
    //Now We Show Health in Numbers
    std::string health = std::to_string(int(champion.hp_current)) + "/" + std::to_string(int(champion.hp_max));
    DrawText(health.c_str(),Cx + w - 105, Cy + l + 40, 10, WHITE);
}
void DrawChampions(std::vector<ChampState>& Team, int TeamNumber){
    Color drawcolor;
    if (TeamNumber == 1) drawcolor = RED;
    else if (TeamNumber == 2) drawcolor = YELLOW;
    for (ChampState& Champ: Team){
        if (Champ.is_dead) continue;
        std::string name = Champ.def.name;
        int q = Champ.pos.q;
        int r = Champ.pos.r;
        Vector2 c= {750 + 75 * (q * sqrt(3) + r * sqrt(3)/2), 537.5 + r * 112.5};
        DrawCircleLinesV(c, 50, drawcolor);
        DrawText(name.c_str(), 750 + 75 * (q * sqrt(3) + r * sqrt(3)/2) - 25, 537.5 + r * 112.5 - 12.5, 20, WHITE);
        DrawHealthBar(c.x , c.y, 50, Champ);
    }
}
void DrawTraitsSkeleton(){
    DrawText("Team 2 Traits", 100, 100, 25, YELLOW);
    DrawLine(50,500,150,500, WHITE);
    DrawText("Team 1 Traits", 100, 550, 25, RED);
}
void DrawTraits(int position, std::vector<ChampState> &Team, std::vector<TraitDef*> TraitsInTeam){
    DrawTraitsSkeleton();
    int i = 1;
    for (TraitDef* trait: TraitsInTeam){
        if (trait->name != "Celestials"){
            if (position == 1){
                std::string text = trait->name+ " " + std::to_string(trait->numchampsT1);
                if (trait->numchampsT1 >= trait->thresholds[1]){
                    DrawText(text.c_str(), 100, 550 + 75*i, 20, GOLD);
                    i++;
                }
                else if (trait->numchampsT1 >= trait->thresholds[0]){
                    DrawText(text.c_str(), 100, 550 + 75*i, 20, LIGHTGRAY);
                    i++;
                }
            }
            else if (position == 0){
                std::string text = trait->name+ " " + std::to_string(trait->numchampsT2);
                if (trait->numchampsT2 >= trait->thresholds[1]){
                    DrawText(text.c_str(), 100, 100 + 75*i, 20, GOLD);
                    i++;
                }
                else if (trait->numchampsT2 >= trait->thresholds[0]){
                    DrawText(text.c_str(), 100, 100 + 75*i, 20, LIGHTGRAY);
                    i++;
                }
            }
        }
        else{
            if (position == 1){
                std::string text = trait->name+ " " + std::to_string(trait->numchampsT1);
                if (trait->numchampsT1 >= trait->thresholds[0]){
                    DrawText(text.c_str(), 100, 550 + 75*i, 20, SKYBLUE);
                    i++;
                }
            }
            else if (position == 0){
                std::string text = trait->name+ " " + std::to_string(trait->numchampsT2);
                if (trait->numchampsT2 >= trait->thresholds[1]){
                    DrawText(text.c_str(), 100, 100 + 75*i, 20, SKYBLUE);
                    i++;
                }
            }
        }
    }
}
void DrawConsoleLog(int width, int height){
    int size = 15;
    int max_lines = 390/20;
    int height_start = height - 395;
    int width_start = (width - 600) + 5;
    int textsstart = 0;
    int texts = LogTxts.size();
    if (texts >= max_lines){
        textsstart = texts-max_lines;
    }
    DrawRectangleLines((width-600), height - 400, 600, 400, WHITE);
    for (int i = textsstart; i<texts; i++){
        DrawText(LogTxts[i].c_str(), width_start, height_start + (i-textsstart)*20, size, WHITE);
    }
}
void Accelerate(){
    for (ChampState& Champ: Team1){
        Champ.attackspeed_current *= 1.05f;
        Champ.attackspeed_current = std::min(Champ.attackspeed_current, Champ.def.attackspeed[Champ.star]*2);
    }
    for (ChampState& Champ: Team2){
        Champ.attackspeed_current *= 1.05f;
        Champ.attackspeed_current = std::min(Champ.attackspeed_current, Champ.def.attackspeed[Champ.star]*2);
    }
}
void Decelerate(){
    for (ChampState& Champ: Team1){
        Champ.attackspeed_current *= 0.95f;
        Champ.attackspeed_current = std::max(Champ.attackspeed_current, Champ.def.attackspeed[Champ.star]/3);
    }
    for (ChampState& Champ: Team2){
        Champ.attackspeed_current *= 0.95f;
        Champ.attackspeed_current = std::max(Champ.attackspeed_current, Champ.def.attackspeed[Champ.star]/3);
    }
}
void DrawChampionTray(int Width, int Height){
    std::vector<std::string> Champions = {"Asura", "Takeshi", "Orion", "Delphinus", "Vesper", "Sable", "Akira", "Dante", "Draco", "Andromeda", "Thanatos", "Cassian", "Totom", "Lyra", "Hades", "Solarix", "Goliath"};
    DrawRectangleLines(Width-800, 0, 800, 600, WHITE);
    Vector2 MousePos = GetMousePosition();
    for (int i = 0; i<5; i++){
        for (int j = 0; j<3; j++){
            std::string Champion = Champions[i*3+j];
            DrawRectangleLines(Width-800 + j*(800/3.0f), 0 + 100*i, (800/3.0f), 100, WHITE);
            Rectangle btnBounds = {Width-800 + j*(800/3.0f), 0 + 100*i, (800/3.0f), 100};
            if (CheckCollisionPointRec(MousePos,btnBounds)){
                DrawRectangle(Width-799 + j*(800/3.0f),1 + 100*i, (800/3.0f) -2 , 98, SKYBLUE);
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                    SelectedChampPtr = &CHAMP_STORAGE[Champion];
                    Log("Selected Champion " + Champion);
                }
            }
            if (SelectedChampPtr != nullptr && SelectedChampPtr->name == Champion){
                DrawRectangle(Width-799 + j*(800/3.0f),1 + 100*i, (800/3.0f) -2 , 98, GRAY);
            }
            DrawText(Champion.c_str(), Width-800 + j*(800/3.0f) + 100, 0 + 100*i + 20, 15, WHITE);
            std::vector<Trait> ChampTraits = CHAMP_POOL.at(Champion)->ChampTraits;
            std::string Traits = "Traits: ";
            for (int traitindex = 0; traitindex < ChampTraits.size(); traitindex++){
                Traits += TRAIT_TRANSFORM_REVERSE.at(ChampTraits[traitindex]);
                if(!(traitindex == ChampTraits.size() -1)) Traits += " / ";
            }
            DrawText(Traits.c_str(), Width-800 + j*(800/3.0f) + 25, 0 + 100*i + 80, 12, WHITE);
            std::string CostRange = "Cost: " + std::to_string(CHAMP_POOL.at(Champion)->cost) + " / Range: " + std::to_string(CHAMP_POOL.at(Champion)->range);
            DrawText(CostRange.c_str(), Width-800 + j*(800/3.0f) + 75, 0 + 100*i + 50, 12, WHITE);
        }
    }
    for (int i = 0; i<2; i++){
        std::string Champion = Champions[15+i];
        DrawRectangleLines(Width-800 + i * (400), 500, 400, 100, WHITE);
        Rectangle btnBounds = {Width-800 + i * (400), 500, 400, 100};
        if (CheckCollisionPointRec(MousePos,btnBounds)){
            DrawRectangle(Width-799 + i * (400), 501, 398, 98, SKYBLUE);
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                SelectedChampPtr = &CHAMP_STORAGE[Champion];
                Log("Selected Champion " + Champion);
            }
        }
            if (SelectedChampPtr != nullptr && SelectedChampPtr->name == Champion){
                DrawRectangle(Width-799 + i * (400), 501, 398, 98, GRAY);
            }
        DrawText(Champion.c_str(), Width-800 + i * (400) + 150, 520, 16, WHITE);
        std::vector<Trait> ChampTraits = CHAMP_POOL.at(Champion)->ChampTraits;
            std::string Traits = "Traits: ";
            for (int traitindex = 0; traitindex < ChampTraits.size(); traitindex++){
                Traits += TRAIT_TRANSFORM_REVERSE.at(ChampTraits[traitindex]);
                if(!(traitindex == ChampTraits.size() -1)) Traits += " / ";
            }
        DrawText(Traits.c_str(), Width-800 + i * 400 + 100, 500 + 80, 13, WHITE);
        std::string CostRange = "Cost: " + std::to_string(CHAMP_POOL.at(Champion)->cost) + " / Range: " + std::to_string(CHAMP_POOL.at(Champion)->range);
        DrawText(CostRange.c_str(), Width-800 + i * 400 + 125, 500 + 50, 13, WHITE);
    }
}
void DrawValidHexes(std::vector<GridPos>& TeamGrid){
    for (GridPos& H : TeamGrid){
        int q = H.q;
        int r = H.r;
        Vector2 c={750 + 75 * (q * sqrt(3) + r * sqrt(3)/2), 537.5 + r * 112.5};
        DrawPoly(c, 6, 73, 90, LIGHTGRAY);
    }
}
void ChooseHex(std::vector<GridPos>& TeamGrid){
    Vector2 MousePos = GetMousePosition();
    float Mouseq = sqrt(3)/3.0f * ((MousePos.x - 750)/75) - (1.0f/3) * ((MousePos.y - 537.5)/75);
    float Mouser = (2.0f/3) * ((MousePos.y - 537.5)/75);
    float sfloat = -Mouseq - Mouser;
    int q = std::round(Mouseq);
    int r = std::round(Mouser);
    int s = std::round(sfloat);
    float diffq = std::abs(q-Mouseq);
    float diffr = std::abs(r-Mouser);
    float diffs = std::abs(s-sfloat);
    float maxdiff = std::max({diffq, diffr, diffs});
    int qfinal;
    int rfinal;
    if (maxdiff == diffq){
        qfinal = -r - s;
        rfinal = r;
    }
    else if (maxdiff == diffr){
        qfinal = q;
        rfinal = -q - s;
    }
    else if (maxdiff == diffs){
        qfinal = q;
        rfinal = r;
    }
    for (GridPos& H : TeamGrid){
        int q = H.q;
        int r = H.r;
        Vector2 c={750 + 75 * (q * sqrt(3) + r * sqrt(3)/2), 537.5 + r * 112.5};
        if(H.is_occupied){
            DrawPoly(c, 6, 73, 90, DARKGRAY);
        }
        if(q==qfinal && r==rfinal){
            DrawPoly(c, 6, 73, 90, SKYBLUE);
            if (!H.is_occupied){
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                    if(!(SelectedChampPtr == nullptr)) {
                        auto& currentTeam = (Editing == 1) ? Team1 : Team2;   
                        if(currentTeam.size() < 5){
                            ChampPos = H;
                            H.is_occupied = true;
                            ChampState Champ = CreateChampion(SelectedChampPtr->name, 0);
                            Champ.pos = ChampPos;
                            currentTeam.push_back(Champ);
                            Log("Placed Champion " + SelectedChampPtr->name + " at Hex with coordinates q: " + std::to_string(H.q) + " r: " + std::to_string(H.r));
                            AddTraits();
                            SelectedChampPtr = nullptr;
                        }
                        else {
                            Log("Can't input more than 5 Champions per team");
                        }
                        continue;
                    }
                }
            }
            else{
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                    auto& currentTeam = (Editing == 1) ? Team1 : Team2;
                    std::erase_if(currentTeam, [&](const ChampState& champ) { 
                        return champ.pos.q == H.q && champ.pos.r == H.r; 
                    });
                    H.is_occupied = false;
                    Log("Removed Champion from Hex with coordinates q: " + std::to_string(H.q) + " r: " + std::to_string(H.r));
                }
            }
        }
    }
}
void DrawTeamSelectButtons(int Width, int Height){
    Vector2 MousePos = GetMousePosition();
    for (int i = 0; i<2; i++){
        DrawRectangleLines((Width/2) - 400 + i*200, 0, 200, 125, WHITE);
        Rectangle btnBounds = {(Width/2) - 400 + i*200, 0, 200, 125};
        if (CheckCollisionPointRec(MousePos,btnBounds)){
            DrawRectangle((Width / 2) - 400 + i * 200 + 1, 1, 198, 123, SKYBLUE);
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                Editing = i+1;
            }
        }
        if (Editing == (i + 1)) {
            DrawRectangle((Width / 2) - 400 + i * 200 + 1, 1, 198, 123, GRAY);
        }
        std::string text = "Team " + std::to_string(i+1);
        DrawText(text.c_str(), (Width/2) - 400 + i*200 + 50, 50, 25, WHITE);
    }
}
void DrawPlanning(int Width, int Height){
    DrawInterfaceBackGround(Width,Height);
    DrawConsoleLog(Width,Height);
    DrawGrid(Width,Height);
    DrawChampionTray(Width,Height);
    DrawTeamSelectButtons(Width, Height);
    DrawStartButton(Width, Height);
    if (Editing == 1){
        DrawValidHexes(GridTeam1);
        ChooseHex(GridTeam1);
    }
    else if (Editing == 2){
        DrawValidHexes(GridTeam2);
        ChooseHex(GridTeam2);
    }
    DrawChampions(Team1, 1);
    DrawChampions(Team2, 2);
    DrawTraits(1,Team1,TraitsInTeam1);
    DrawTraits(0,Team2,TraitsInTeam2);
}
void DrawCombat(int Width, int Height){
    DrawInterfaceBackGround(Width,Height);
    DrawTraits(1,Team1,TraitsInTeam1);
    DrawTraits(0,Team2,TraitsInTeam2);
    DrawSecondsInCombat(Width, Height, int(seconds_in_combat));
    DrawConsoleLog(Width,Height);
    DrawDamage();
    DrawGrid(Width,Height);
    DrawChampions(Team1, 1);
    DrawChampions(Team2, 2);
}
void DrawEnd(int Width, int Height){
    DrawInterfaceBackGround(Width,Height);
}