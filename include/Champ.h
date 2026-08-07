#pragma once
#include <iostream>
#include <array>
#include <vector>
#include <string>
#include "Grid.h"
#include "Traits.h"
enum class TargetingMethod {LowestHP, Closest};
struct ChampDef{
    std::string name;
    int cost;
    int range;
    int mana_max;
    TargetingMethod Targeting;
    std::array <float,3> hp;
    std::array <float,3> armor;
    std::array <float,3> magicres;
    std::array <float,3> ad;
    std::array <float,3> ap;
    std::array <float,3> attackspeed;
    std::vector <Trait> ChampTraits;
};
struct ChampState{
    ChampDef def;
    GridPos pos = GridPos(0,0);
    ChampState* enemytarget=nullptr;
    float lastautoattacktime = 0.0f;
    int star;
    int range;
    float hp_current;
    int mana_current;
    float armor_current;
    float magicres_current;
    float ad_current;
    float ap_current;
    float attackspeed_current;
    float current_shield = 0;
    float lifesteal = 0;
    float execute = 0;
    float hp_max = 0;
    float is_invulnerable_until = 0.0f;
    bool is_dead = false;
    ChampState(const ChampDef& d, int s): def(d), star(s){
        range = def.range;
        hp_current = def.hp[star];
        mana_current = 0;
        armor_current = def.armor[star];
        magicres_current = def.magicres[star];
        ad_current = def.ad[star];
        ap_current = def.ap[star];
        attackspeed_current = def.attackspeed[star];
    }
};
inline std::unordered_map<std::string, TargetingMethod> TARGETING_POOL{
    {"LowestHP",     TargetingMethod::LowestHP},
    {"Closest",      TargetingMethod::Closest},
};
inline std::unordered_map<std::string, ChampDef> CHAMP_STORAGE;
inline std::unordered_map<std::string, const ChampDef*> CHAMP_POOL;
enum class GameState{Planning, Combat, End};
inline GameState GamePhase = GameState::Planning;
void LoadChampions();
inline std::string EndMsg;
ChampState CreateChampion(std::string name, int star);
inline std::vector<ChampState> Team1;
inline std::vector<ChampState> Team2;