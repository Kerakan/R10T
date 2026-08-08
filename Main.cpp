#pragma once
#include <iostream>
#include "Champ.h"
#include "Traits.h"
#include "Combat.h"
#include "Draw.h"
int main(){
	LoadTraits();
	LoadChampions();
	const float Fixed_dt = 1.0f/120;
	float accumulator = 0;
	InitWindow(GetMonitorWidth(0), GetMonitorHeight(0), "Autobattler");
    ToggleBorderlessWindowed();
	MaximizeWindow();
	int Width = GetScreenWidth();
    int Height = GetScreenHeight();
    SetTargetFPS(120);
    EnableCursor();
	while (!WindowShouldClose()){
		BeginDrawing();
		if (GamePhase == GameState::Planning){
			DrawPlanning(Width,Height);
		}
		else if(GamePhase == GameState::Combat){
			HideCursor();
			float dt = GetFrameTime();
			accumulator += dt;
			if (accumulator >= Fixed_dt){
				run_combat(Fixed_dt);
				accumulator -= Fixed_dt;
			}	
			DrawCombat(Width, Height);
			if(IsKeyDown(KEY_A)) Accelerate();
			if(IsKeyDown(KEY_D)) Decelerate();   
		}
		else if(GamePhase == GameState::End){
			ShowCursor();
			DrawEnd(Width,Height);
		}
		EndDrawing();
	}
	CloseWindow();
	return 0;
}