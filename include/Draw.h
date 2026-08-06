#pragma once
#include "raylib.h"
#include "Champ.h"
#include "Traits.h"
inline int Editing = 1;
inline ChampDef* SelectedChampPtr = nullptr;
inline GridPos ChampPos;
void DrawDamage();
void DrawInterfaceBackGround(int width, int height);
void DrawSecondsInCombat(int width, int height, int seconds);
void DrawStartButton(int width, int height);
void DrawHexagon(Vector2 center,Color color);
void DrawGrid(int width, int height);
void DrawHealthBar(int Cx, int Cy, int rad,ChampState& champion);
void DrawChampions(std::vector<ChampState>& Team, int TeamNumber);
void DrawTraitsSkeleton();
void DrawTraits(int position, std::vector<ChampState> &Team, std::vector<TraitDef*> TraitsInTeam);
void DrawConsoleLog(int width, int height);
void Accelerate();
void Decelerate();
void DrawChampionTray(int Width, int Height);
void DrawPlanning(int Width, int Height);
void DrawCombat(int Width, int Height);