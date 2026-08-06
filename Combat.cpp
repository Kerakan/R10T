#pragma once
#include <iostream>
#include <algorithm>
#include "TraitStatSystem.h"
#include "Traits.h"
#include "AttacksAbilitiesDeathHandling.h"
#include "Log.h"
#include "Combat.h"
#include <set>
#include "EnemyFinding.h"
#include <random>
void AddTraits(){
    TraitsInTeam1.clear();
    TraitsInTeam2.clear();
    for (ChampState& champion: Team1){
        for(Trait trait: champion.def.ChampTraits){
            TraitDef* t = TRAIT_POOL.at(trait);
            if (std::find(TraitsInTeam1.begin(),TraitsInTeam1.end(),t)==TraitsInTeam1.end()){
                TraitsInTeam1.push_back(t);
            }
            t->numchampsT1 = 0;
        }
    }
    for (ChampState& champion: Team2){
        for(Trait trait: champion.def.ChampTraits){
            TraitDef* t = TRAIT_POOL.at(trait);
            if (std::find(TraitsInTeam2.begin(),TraitsInTeam2.end(),t)==TraitsInTeam2.end()){
                TraitsInTeam2.push_back(t);
            }
            t->numchampsT2 = 0;
        }
    }
    std::set<std::pair<TraitDef*, std::string>> ChampNamesAddedTeam;
    for (ChampState& champion: Team1){
        for(Trait trait: champion.def.ChampTraits){
            TraitDef* t = TRAIT_POOL.at(trait);
            if (std::find(TraitsInTeam1.begin(),TraitsInTeam1.end(),t)==TraitsInTeam1.end()){
                TraitsInTeam1.push_back(t);
            }
            if (ChampNamesAddedTeam.insert({t, champion.def.name}).second){
                t->numchampsT1++;
            }
        }
    }
    ChampNamesAddedTeam.clear();
    for (ChampState& champion: Team2){
        for(Trait trait: champion.def.ChampTraits){
            TraitDef* t = TRAIT_POOL.at(trait);
            if (std::find(TraitsInTeam2.begin(),TraitsInTeam2.end(),t)==TraitsInTeam2.end()){
                TraitsInTeam2.push_back(t);
            }
            if (ChampNamesAddedTeam.insert({t, champion.def.name}).second){
                t->numchampsT2++;
            }
        }
    }
    //Define max hp for each champion
    for (ChampState& c: Team1) c.hp_max = c.hp_current;
    for (ChampState& c: Team2) c.hp_max = c.hp_current;
}
void ApplyTraits(){
     for (ChampState& champion: Team1){
        for(Trait trait: champion.def.ChampTraits){
            TraitDef* t = TRAIT_POOL.at(trait);
        }
    }
    //Now we apply the effects
    for (ChampState& champion: Team1){
        for(Trait trait: champion.def.ChampTraits){
            TraitDef* t = TRAIT_POOL.at(trait);
            if (t->name != "Celestials" and t->name != "ShadowFighters") ApplyTraitEffects(*t,champion,1);
            else if(t->name == "Celestials") ApplyCelestialsTraitEffects(champion,1);
        }
    }
    Log("Team 1 traits activated");
    for (ChampState& champion: Team2){
        for(Trait trait: champion.def.ChampTraits){
            TraitDef* t = TRAIT_POOL.at(trait);
        }
    }
    //Now we apply the effects
    for (ChampState& champion: Team2){
        for(Trait trait: champion.def.ChampTraits){
            TraitDef* t = TRAIT_POOL.at(trait);
            if (t->name != "Celestials" and t->name != "ShadowFighters") ApplyTraitEffects(*t,champion,2);
            else if(t->name == "Celestials") ApplyCelestialsTraitEffects(champion,2);
            }
        }
    Log("Team 2 traits activated");
    //Define max hp for each champion
    for (ChampState& c: Team1) c.hp_max = c.hp_current;
    for (ChampState& c: Team2) c.hp_max = c.hp_current;
}
bool is_empty(std::vector<ChampState> Team){
    for (ChampState c: Team){
        if(!c.is_dead) return false;
    }
    return true;
}
void end_combat(){
}
void run_combat(float DeltaTime) {
    if (!combat_started) return;
    //Apply ShadowFighter per second
    seconds_in_combat+=DeltaTime;
    if(seconds_in_combat-Shadow_Fighters_last >= Shadow_Fighters_interval){
        for (ChampState& champion: Team1){
            if (champion.is_dead) continue;
            if (TRAIT_POOL.at(Trait::Shadow_Fighters)->numchampsT1 >= TRAIT_POOL.at(Trait::Shadow_Fighters)->thresholds[0]){
                if (std::find(champion.def.ChampTraits.begin(), champion.def.ChampTraits.end(), Trait::Shadow_Fighters) != champion.def.ChampTraits.end()){
                    ApplyShadowFighterTraitEffects(champion,1);
                }
            }
        }
        for (ChampState& champion: Team2){
            if (champion.is_dead) continue;
            if (TRAIT_POOL.at(Trait::Shadow_Fighters)->numchampsT2 >= TRAIT_POOL.at(Trait::Shadow_Fighters)->thresholds[0]){
                if (std::find(champion.def.ChampTraits.begin(), champion.def.ChampTraits.end(), Trait::Shadow_Fighters) != champion.def.ChampTraits.end()){
                    ApplyShadowFighterTraitEffects(champion,2);
                }
            }
        }
        Shadow_Fighters_last = seconds_in_combat;
    }
    std::vector<ChampState*> Combatants;
    for (ChampState& champion: Team1){
        if (!champion.is_dead) Combatants.push_back(&champion);
    }
    for (ChampState& champion: Team2){
        if (!champion.is_dead) Combatants.push_back(&champion);
    }
    static std::random_device rd;
    static std::mt19937 g(rd());
    std::shuffle(Combatants.begin(),Combatants.end(),g);
    for (ChampState* champptr: Combatants){
        ChampState& champion = *champptr;
        bool is_team1 = (std::find_if(Team1.begin(), Team1.end(), [&](ChampState& c) { return &c == champptr; }) != Team1.end());
        auto& AllyTeam = is_team1 ? Team1 : Team2;
        auto& EnemyTeam = is_team1 ? Team2 : Team1;
        if (champion.is_dead) continue;
        if (champion.enemytarget == nullptr or champion.enemytarget->is_dead){
            FindClosestEnemy(champion, AllyTeam, EnemyTeam);
        }
        if (seconds_in_combat-champion.lastautoattacktime >= 1.0f/champion.attackspeed_current){
            champion.lastautoattacktime = seconds_in_combat;
            autoattack(champion, AllyTeam, EnemyTeam);
        }   
        if (champion.mana_current >= champion.def.mana_max and champion.def.mana_max > 0){
                Ability(champion, AllyTeam, EnemyTeam);
                champion.mana_current = 0;
        }
    }
    if (is_empty(Team2)) {
        std::cout << "Team 1 wins!\n";
        end_combat();
    }
    if (is_empty(Team1)) {
        std::cout << "Team 2 wins!\n";
        end_combat();
    }
}