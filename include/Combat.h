#pragma once
#include "Champ.h"
#include "Traits.h"
#include <vector>
inline std::vector<TraitDef*> TraitsInTeam1;
inline std::vector<TraitDef*> TraitsInTeam2;
inline float Shadow_Fighters_last = 0.0f;
inline float Shadow_Fighters_interval=1.0f;
inline float seconds_in_combat = 0.0f;
inline bool combat_started = false;
bool is_empty(std::vector<ChampState> Team);
void AddTraits();
void ApplyTraits();
void run_combat(float DeltaTime);