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

// ---------------------------------------------------------------------------
// Resolution scaling
// ---------------------------------------------------------------------------
// The layout below was originally designed at BASE_WIDTH x BASE_HEIGHT
// (the hex grid was centered at 750,537.5 -> half of 1500 x 1075).
// Every pixel value that isn't already derived from the live Width/Height
// parameters is now multiplied by GetUIScale(), and the hex-grid origin is
// tied to the actual screen center, so the whole layout scales and
// re-centers cleanly at any resolution (including portrait, e.g. 1080x1920).
constexpr float BASE_WIDTH = 1500.0f;
constexpr float BASE_HEIGHT = 1075.0f;

inline float GetUIScale(int Width, int Height){
    return std::min(Width / BASE_WIDTH, Height / BASE_HEIGHT);
}

// Extra downward shift (in design-space pixels) so the hex grid clears the
// Team 1 / Team 2 buttons sitting above it.
constexpr float GRID_Y_OFFSET = 45.0f;

// Centered-text helper, matching the pattern already used in DrawEnd:
// horizontally centers `text` on `centerX`, top of the text at `topY`.
inline void DrawCenteredText(const std::string& text, float centerX, float topY, int fontSize, Color color){
    DrawText(text.c_str(), centerX - MeasureText(text.c_str(), fontSize) / 2, topY, fontSize, color);
}

void DrawDamage(int Width, int Height){
    float scale = GetUIScale(Width, Height);
    float originX = 750 * scale;
    float originY = (537.5f + GRID_Y_OFFSET) * scale;
    float current_time = seconds_in_combat;
    int size = TimedTexts.size();
    for (int i = 0; i<size ; i++){
        if (TimedTexts[i].defined_until < current_time){
            continue;
        }
        std::string text = TimedTexts[i].text + " DMG";
        float cx = originX + 75 * scale * (TimedTexts[i].q * sqrt(3) + TimedTexts[i].r * sqrt(3)/2) + 60 * scale;
        float cy = originY + TimedTexts[i].r * 112.5f * scale - 20 * scale;
        DrawText(text.c_str(), cx, cy, std::max(1, int(12 * scale)), RED);
    }
}
void DrawInterfaceBackGround(int width, int height){
    float scale = GetUIScale(width, height);
    ClearBackground(BLACK);
    DrawText("Press A/D to Accelerate/Decelerate", 10 * scale, height - 30 * scale, std::max(1, int(20 * scale)), WHITE);
    DrawFPS(width - 100 * scale, 20 * scale);
};
void DrawSecondsInCombat(int width, int height, int seconds){
    float scale = GetUIScale(width, height);
    float w = 300 * scale;
    float h = 150 * scale;
    float x = (width / 2.0f) - w / 2.0f;
    float y = height - h;
    DrawRectangleLines(x, y, w, h, WHITE);
    DrawRectangle(x + 1, y + 1, w - 2, h - 2, YELLOW);
    DrawCenteredText(std::to_string(seconds), width / 2.0f, y + h / 2.0f - 25 * scale, std::max(1, int(50 * scale)), WHITE);
}
void DrawStartButton(int width, int height){
    if (!combat_started){
        float scale = GetUIScale(width, height);
        float w = 300 * scale;
        float h = 150 * scale;
        float x = (width / 2.0f) - w / 2.0f;
        float y = height - h;
        DrawRectangleLines(x, y, w, h, WHITE);
        DrawRectangle(x + 1, y + 1, w - 2, h - 2, SKYBLUE);
        Vector2 MousePos = GetMousePosition();
        Rectangle btnBounds = {x, y, w, h};
        if (CheckCollisionPointRec(MousePos,btnBounds)){
            DrawRectangle(x + 1, y + 1, w - 2, h - 2, BLUE);
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                combat_started = true;
                GamePhase = GameState::Combat;
                ApplyTraits();
            }
        }
        DrawCenteredText("START", width / 2.0f, y + h / 2.0f - 25 * scale, std::max(1, int(50 * scale)), WHITE);
    }
}
void DrawHexagon(Vector2 center, float radius, Color color){
    DrawPolyLines(center, 6, radius, 90, color);
}
void DrawGrid(int width, int height){
    float scale = GetUIScale(width, height);
    float originX = 750 * scale;
    float originY = (537.5f + GRID_Y_OFFSET) * scale;
    float hexRadius = 75 * scale;
    //Draw the board converting axial coordinates to pixels:
    for (GridPos H : GridTeam2){
        int q = H.q;
        int r = H.r;
        Vector2 c={originX + hexRadius * (q * sqrt(3) + r * sqrt(3)/2), originY + r * 112.5f * scale};
        DrawHexagon(c, hexRadius, YELLOW);
    }
    for (GridPos H : GridTeam1){
        int q = H.q;
        int r = H.r;
        Vector2 c={originX + hexRadius * (q * sqrt(3) + r * sqrt(3)/2), originY + r * 112.5f * scale};
        DrawHexagon(c, hexRadius, RED);
    }
}
void DrawHealthBar(int Cx, int Cy, int rad, ChampState& champion, float scale){
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
    //Now We Show Health in Numbers, centered under the bar
    std::string health = std::to_string(int(champion.hp_current)) + "/" + std::to_string(int(champion.hp_max));
    DrawCenteredText(health, Cx, Cy + l + 40 * scale, std::max(1, int(10 * scale)), WHITE);
}
void DrawChampions(std::vector<ChampState>& Team, int TeamNumber, int Width, int Height){
    float scale = GetUIScale(Width, Height);
    float originX = 750 * scale;
    float originY = (537.5f + GRID_Y_OFFSET) * scale;
    float hexRadius = 75 * scale;
    float champRadius = 50 * scale;
    Color drawcolor;
    Color ChampColor;
    if (TeamNumber == 1) drawcolor = RED;
    else if (TeamNumber == 2) drawcolor = YELLOW;
    for (ChampState& Champ: Team){
        if (Champ.star == 0) ChampColor = WHITE;
        else if (Champ.star == 1) ChampColor = LIGHTGRAY;
        else if (Champ.star == 2) ChampColor = GOLD;
        if (Champ.is_dead) continue;
        std::string name = Champ.def.name;
        int q = Champ.pos.q;
        int r = Champ.pos.r;
        Vector2 c= {originX + hexRadius * (q * sqrt(3) + r * sqrt(3)/2), originY + r * 112.5f * scale};
        DrawCircleLinesV(c, champRadius, drawcolor);
        DrawCenteredText(name, c.x, c.y - 12.5f * scale, std::max(1, int(20 * scale)), ChampColor);
        DrawHealthBar(c.x , c.y, champRadius, Champ, scale);
    }
}
void DrawTraitsSkeleton(int Width, int Height){
    float scale = GetUIScale(Width, Height);
    DrawText("Team 2 Traits", 100 * scale, 100 * scale, std::max(1, int(25 * scale)), YELLOW);
    DrawLine(50 * scale, 500 * scale, 150 * scale, 500 * scale, WHITE);
    DrawText("Team 1 Traits", 100 * scale, 550 * scale, std::max(1, int(25 * scale)), RED);
}
void DrawTraits(int position, std::vector<ChampState> &Team, std::vector<TraitDef*> TraitsInTeam, int Width, int Height){
    float scale = GetUIScale(Width, Height);
    DrawTraitsSkeleton(Width, Height);
    int i = 1;
    for (TraitDef* trait: TraitsInTeam){
        if (trait->name != "Celestials"){
            if (position == 1){
                std::string text = trait->name+ " " + std::to_string(trait->numchampsT1);
                if (trait->numchampsT1 >= trait->thresholds[1]){
                    DrawText(text.c_str(), 100 * scale, (550 + 75*i) * scale, std::max(1, int(20 * scale)), GOLD);
                    i++;
                }
                else if (trait->numchampsT1 >= trait->thresholds[0]){
                    DrawText(text.c_str(), 100 * scale, (550 + 75*i) * scale, std::max(1, int(20 * scale)), LIGHTGRAY);
                    i++;
                }
            }
            else if (position == 0){
                std::string text = trait->name+ " " + std::to_string(trait->numchampsT2);
                if (trait->numchampsT2 >= trait->thresholds[1]){
                    DrawText(text.c_str(), 100 * scale, (100 + 75*i) * scale, std::max(1, int(20 * scale)), GOLD);
                    i++;
                }
                else if (trait->numchampsT2 >= trait->thresholds[0]){
                    DrawText(text.c_str(), 100 * scale, (100 + 75*i) * scale, std::max(1, int(20 * scale)), LIGHTGRAY);
                    i++;
                }
            }
        }
        else{
            if (position == 1){
                std::string text = trait->name+ " " + std::to_string(trait->numchampsT1);
                if (trait->numchampsT1 >= trait->thresholds[0]){
                    DrawText(text.c_str(), 100 * scale, (550 + 75*i) * scale, std::max(1, int(20 * scale)), SKYBLUE);
                    i++;
                }
            }
            else if (position == 0){
                std::string text = trait->name+ " " + std::to_string(trait->numchampsT2);
                if (trait->numchampsT2 >= trait->thresholds[0]){
                    DrawText(text.c_str(), 100 * scale, (100 + 75*i) * scale, std::max(1, int(20 * scale)), SKYBLUE);
                    i++;
                }
            }
        }
    }
}
void DrawConsoleLog(int width, int height){
    float scale = GetUIScale(width, height);
    int fontSize = std::max(1, int(15 * scale));
    int lineHeight = std::max(1, int(20 * scale));
    float boxW = 600 * scale;
    float boxH = 400 * scale;
    int max_lines = int(boxH * 0.975f) / lineHeight; // ~390/20 ratio preserved
    float height_start = height - boxH * 0.9875f;    // ~395/400 ratio preserved
    float width_start = (width - boxW) + 5 * scale;
    int textsstart = 0;
    int texts = LogTxts.size();
    if (texts >= max_lines){
        textsstart = texts-max_lines;
    }
    DrawRectangleLines(width - boxW, height - boxH, boxW, boxH, WHITE);
    for (int i = textsstart; i<texts; i++){
        DrawText(LogTxts[i].c_str(), width_start, height_start + (i-textsstart)*lineHeight, fontSize, WHITE);
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
    float scale = GetUIScale(Width, Height);
    float trayW = 800 * scale;
    std::vector<std::string> Champions = {"Asura", "Takeshi", "Orion", "Delphinus", "Vesper", "Sable", "Akira", "Dante", "Draco", "Andromeda", "Thanatos", "Cassian", "Totom", "Lyra", "Hades", "Solarix", "Goliath"};
    DrawRectangleLines(Width-trayW, 0, trayW, 600 * scale, WHITE);
    Vector2 MousePos = GetMousePosition();
    for (int i = 0; i<5; i++){
        for (int j = 0; j<3; j++){
            std::string Champion = Champions[i*3+j];
            float cellW = trayW / 3.0f;
            float cellH = 100 * scale;
            float cellX = Width - trayW + j * cellW;
            float cellY = i * cellH;
            DrawRectangleLines(cellX, cellY, cellW, cellH, WHITE);
            Rectangle btnBounds = {cellX, cellY, cellW, cellH};
            if (CheckCollisionPointRec(MousePos,btnBounds)){
                DrawRectangle(cellX + 1, cellY + 1, cellW - 2, cellH - 2, SKYBLUE);
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                    SelectedChampPtr = &CHAMP_STORAGE[Champion];
                    Log("Selected Champion " + Champion);
                }
            }
            if (SelectedChampPtr != nullptr && SelectedChampPtr->name == Champion){
                DrawRectangle(cellX + 1, cellY + 1, cellW - 2, cellH - 2, GRAY);
            }
            DrawCenteredText(Champion, cellX + cellW / 2.0f, cellY + 20 * scale, std::max(1, int(15 * scale)), WHITE);
            std::vector<Trait> ChampTraits = CHAMP_POOL.at(Champion)->ChampTraits;
            std::string Traits = "Traits: ";
            for (int traitindex = 0; traitindex < ChampTraits.size(); traitindex++){
                Traits += TRAIT_TRANSFORM_REVERSE.at(ChampTraits[traitindex]);
                if(!(traitindex == ChampTraits.size() -1)) Traits += " / ";
            }
            DrawCenteredText(Traits, cellX + cellW / 2.0f, cellY + 80 * scale, std::max(1, int(12 * scale)), WHITE);
            std::string CostRange = "Cost: " + std::to_string(CHAMP_POOL.at(Champion)->cost) + " / Range: " + std::to_string(CHAMP_POOL.at(Champion)->range);
            DrawCenteredText(CostRange, cellX + cellW / 2.0f, cellY + 50 * scale, std::max(1, int(12 * scale)), WHITE);
        }
    }
    for (int i = 0; i<2; i++){
        std::string Champion = Champions[15+i];
        float cellW = trayW / 2.0f;
        float cellH = 100 * scale;
        float cellX = Width - trayW + i * cellW;
        float cellY = 500 * scale;
        DrawRectangleLines(cellX, cellY, cellW, cellH, WHITE);
        Rectangle btnBounds = {cellX, cellY, cellW, cellH};
        if (CheckCollisionPointRec(MousePos,btnBounds)){
            DrawRectangle(cellX + 1, cellY + 1, cellW - 2, cellH - 2, SKYBLUE);
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                SelectedChampPtr = &CHAMP_STORAGE[Champion];
                Log("Selected Champion " + Champion);
            }
        }
        if (SelectedChampPtr != nullptr && SelectedChampPtr->name == Champion){
            DrawRectangle(cellX + 1, cellY + 1, cellW - 2, cellH - 2, GRAY);
        }
        DrawCenteredText(Champion, cellX + cellW / 2.0f, cellY + 20 * scale, std::max(1, int(16 * scale)), WHITE);
        std::vector<Trait> ChampTraits = CHAMP_POOL.at(Champion)->ChampTraits;
        std::string Traits = "Traits: ";
        for (int traitindex = 0; traitindex < ChampTraits.size(); traitindex++){
            Traits += TRAIT_TRANSFORM_REVERSE.at(ChampTraits[traitindex]);
            if(!(traitindex == ChampTraits.size() -1)) Traits += " / ";
        }
        DrawCenteredText(Traits, cellX + cellW / 2.0f, cellY + 80 * scale, std::max(1, int(13 * scale)), WHITE);
        std::string CostRange = "Cost: " + std::to_string(CHAMP_POOL.at(Champion)->cost) + " / Range: " + std::to_string(CHAMP_POOL.at(Champion)->range);
        DrawCenteredText(CostRange, cellX + cellW / 2.0f, cellY + 50 * scale, std::max(1, int(13 * scale)), WHITE);
    }
}
void DrawValidHexes(std::vector<GridPos>& TeamGrid, int Width, int Height){
    float scale = GetUIScale(Width, Height);
    float originX = 750 * scale;
    float originY = (537.5f + GRID_Y_OFFSET) * scale;
    float hexRadius = 75 * scale;
    for (GridPos& H : TeamGrid){
        int q = H.q;
        int r = H.r;
        Vector2 c={originX + hexRadius * (q * sqrt(3) + r * sqrt(3)/2), originY + r * 112.5f * scale};
        DrawPoly(c, 6, 73 * scale, 90, LIGHTGRAY);
    }
}
void ChooseHex(std::vector<GridPos>& TeamGrid, int Width, int Height){
    float scale = GetUIScale(Width, Height);
    float originX = 750 * scale;
    float originY = (537.5f + GRID_Y_OFFSET) * scale;
    float hexRadius = 75 * scale;
    Vector2 MousePos = GetMousePosition();
    float Mouseq = sqrt(3)/3.0f * ((MousePos.x - originX)/hexRadius) - (1.0f/3) * ((MousePos.y - originY)/hexRadius);
    float Mouser = (2.0f/3) * ((MousePos.y - originY)/hexRadius);
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
        Vector2 c={originX + hexRadius * (q * sqrt(3) + r * sqrt(3)/2), originY + r * 112.5f * scale};
        if(H.is_occupied){
            DrawPoly(c, 6, 73 * scale, 90, DARKGRAY);
        }
        if(q==qfinal && r==rfinal){
            DrawPoly(c, 6, 73 * scale, 90, SKYBLUE);
            if (!H.is_occupied){
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                    if(!(SelectedChampPtr == nullptr)) {
                        auto& currentTeam = (Editing == 1) ? Team1 : Team2;   
                        if(currentTeam.size() < 5){
                            ChampPos = H;
                            H.is_occupied = true;
                            ChampState Champ = CreateChampion(SelectedChampPtr->name, SelectedStar);
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
                    AddTraits();
                    Log("Removed Champion from Hex with coordinates q: " + std::to_string(H.q) + " r: " + std::to_string(H.r));
                }
            }
        }
    }
}
void DrawTeamSelectButtons(int Width, int Height){
    float scale = GetUIScale(Width, Height);
    Vector2 MousePos = GetMousePosition();
    float btnW = 200 * scale;
    float btnH = 125 * scale;
    float centerX = 750 * scale; // align with hex grid origin, not full window width
    for (int i = 0; i<2; i++){
        float x = centerX - btnW + i*btnW;
        DrawRectangleLines(x, 0, btnW, btnH, WHITE);
        Rectangle btnBounds = {x, 0, btnW, btnH};
        if (CheckCollisionPointRec(MousePos,btnBounds)){
            DrawRectangle(x + 1, 1, btnW - 2, btnH - 2, SKYBLUE);
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                Editing = i+1;
            }
        }
        if (Editing == (i + 1)) {
            DrawRectangle(x + 1, 1, btnW - 2, btnH - 2, GRAY);
        }
        std::string text = "Team " + std::to_string(i+1);
        DrawCenteredText(text, x + btnW / 2.0f, 50 * scale, std::max(1, int(25 * scale)), WHITE);
    }
}
void DrawStarSelect(int Width, int Height){
    float scale = GetUIScale(Width, Height);
    Vector2 MousePos = GetMousePosition();
    float btnW = 200 * scale;
    float btnH = 125 * scale;
    float consoleLogW = 600 * scale;
    float consoleLogLeftEdge = Width - consoleLogW;
    float startBtnW = 300 * scale;
    float startBtnRightEdge = (Width / 2.0f) + (startBtnW / 2.0f);
    float gapCenterX = (startBtnRightEdge + consoleLogLeftEdge) / 2.0f;
    float groupH = btnH * 3.0f;
    float x = gapCenterX - btnW / 2.0f;
    float groupTop = Height - groupH;
    for (int i = 0; i<3; i++){
        float y = groupTop + i*btnH;
        DrawRectangleLines(x, y, btnW, btnH, WHITE);
        Rectangle btnBounds = {x, y, btnW, btnH};
        if (CheckCollisionPointRec(MousePos,btnBounds)){
            DrawRectangle(x + 1, y + 1, btnW - 2, btnH - 2, SKYBLUE);
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                SelectedStar = i;
                Log("Selected Star Level " + std::to_string(SelectedStar+1));
            }
        }
        if (SelectedStar == (i)) {
            DrawRectangle(x + 1, y + 1, btnW - 2, btnH - 2, GRAY);
        }
        std::string text = std::to_string(i+1) + " Star";
        DrawCenteredText(text, x + btnW / 2.0f, y + 50 * scale, std::max(1, int(25 * scale)), WHITE);
    }
}

void DrawPlanning(int Width, int Height){
    DrawInterfaceBackGround(Width,Height);
    DrawConsoleLog(Width,Height);
    DrawGrid(Width,Height);
    DrawChampionTray(Width,Height);
    DrawTeamSelectButtons(Width, Height);
    DrawStartButton(Width, Height);
    DrawStarSelect(Width, Height);
    if (Editing == 1){
        DrawValidHexes(GridTeam1, Width, Height);
        ChooseHex(GridTeam1, Width, Height);
    }
    else if (Editing == 2){
        DrawValidHexes(GridTeam2, Width, Height);
        ChooseHex(GridTeam2, Width, Height);
    }
    DrawChampions(Team1, 1, Width, Height);
    DrawChampions(Team2, 2, Width, Height);
    DrawTraits(1,Team1,TraitsInTeam1, Width, Height);
    DrawTraits(0,Team2,TraitsInTeam2, Width, Height);
}
void DrawCombat(int Width, int Height){
    DrawInterfaceBackGround(Width,Height);
    DrawTraits(1,Team1,TraitsInTeam1, Width, Height);
    DrawTraits(0,Team2,TraitsInTeam2, Width, Height);
    DrawSecondsInCombat(Width, Height, int(seconds_in_combat));
    DrawConsoleLog(Width,Height);
    DrawDamage(Width, Height);
    DrawGrid(Width,Height);
    DrawChampions(Team1, 1, Width, Height);
    DrawChampions(Team2, 2, Width, Height);
}
void DrawEndButtons(int Width, int Height){
    float scale = GetUIScale(Width, Height);
    Vector2 MousePos = GetMousePosition();
    float btnW = 400 * scale;
    float btnH = 150 * scale;
    float centerX = Width / 2.0f;
    float centerY = Height / 2.0f + 100 * scale;
    //Draw Retry Button
    float x = centerX - btnW - 100 * scale;
    float y = centerY;
    Rectangle btnBounds = {x, y, btnW, btnH};
    DrawRectangleLines(x, y, btnW, btnH, WHITE);
    if (CheckCollisionPointRec(MousePos,btnBounds)){
        DrawRectangle(x + 1, y + 1, btnW - 2, btnH - 2, LIGHTGRAY);
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            Team1.clear();
            Team2.clear();
            TraitsInTeam1.clear();
            TraitsInTeam2.clear();
            for (GridPos& H : GridTeam1){
                H.is_occupied = false;
            }
            for (GridPos& H : GridTeam2){
                H.is_occupied = false;
            }
            Editing = 1;
            SelectedChampPtr = nullptr;
            GamePhase = GameState::Planning;
            LogTxts.clear();
            Log("Game Reset");
        }
    }
    DrawCenteredText("CHOOSE AGAIN", x + btnW / 2.0f, y + btnH / 2.0f - 25 * scale, std::max(1, int(50 * scale)), WHITE);
    //Draw Exit Button
    x = centerX + 100 * scale;
    btnBounds = {x, y, btnW, btnH};
    DrawRectangleLines(x, y, btnW, btnH, WHITE);
    if (CheckCollisionPointRec(MousePos,btnBounds)){
        DrawRectangle(x + 1, y + 1, btnW - 2, btnH - 2, LIGHTGRAY);
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            CloseWindow();
        }
    }
    DrawCenteredText("EXIT", x + btnW / 2.0f, y + btnH / 2.0f - 25 * scale, std::max(1, int(50 * scale)), WHITE);
}
void DrawEnd(int Width, int Height){
    DrawInterfaceBackGround(Width,Height);
    DrawEndButtons(Width, Height);
    float scale = GetUIScale(Width, Height);
    int fontSize = std::max(1, int(100 * scale));
    DrawCenteredText(EndMsg, Width/2.0f, Height/2.0f - 100 * scale, fontSize, WHITE);
}