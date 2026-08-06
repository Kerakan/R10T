#pragma once
#include "Champ.h"
#include <functional>
void managedamage(ChampState& champ1,ChampState& champ2, float dmg, std::vector<ChampState> &AllyTeam, std::vector<ChampState> &EnemyTeam);
void autoattack(ChampState& champ, std::vector<ChampState> &AllyTeam, std::vector<ChampState> &EnemyTeam);
using AbilityFunction = std::function<void(ChampState&, std::vector<ChampState>&, std::vector<ChampState>&)>;
void akira_ability(ChampState& champ, std::vector<ChampState>& AllyTeam, std::vector<ChampState>& EnemyTeam);
void totom_ability(ChampState& champ, std::vector<ChampState>& AllyTeam, std::vector<ChampState>& EnemyTeam);
void asura_ability(ChampState& champ, std::vector<ChampState>& AllyTeam, std::vector<ChampState>& EnemyTeam);
void dante_ability(ChampState& champ, std::vector<ChampState>& AllyTeam, std::vector<ChampState>& EnemyTeam);
void takeshi_ability(ChampState& champ, std::vector<ChampState>& AllyTeam, std::vector<ChampState>& EnemyTeam);
void draco_ability(ChampState& champ, std::vector<ChampState>& AllyTeam, std::vector<ChampState>& EnemyTeam);
void lyra_ability(ChampState& champ, std::vector<ChampState>& AllyTeam, std::vector<ChampState>& EnemyTeam);
void orion_ability(ChampState& champ, std::vector<ChampState>& AllyTeam, std::vector<ChampState>& EnemyTeam);
void andromeda_ability(ChampState& champ, std::vector<ChampState>& AllyTeam, std::vector<ChampState>& EnemyTeam);
void delphinus_ability(ChampState& champ, std::vector<ChampState>& AllyTeam, std::vector<ChampState>& EnemyTeam);
void hades_ability(ChampState& champ, std::vector<ChampState>& AllyTeam, std::vector<ChampState>& EnemyTeam);
void thanatos_ability(ChampState& champ, std::vector<ChampState>& AllyTeam, std::vector<ChampState>& EnemyTeam);
void vesper_ability(ChampState& champ, std::vector<ChampState>& AllyTeam, std::vector<ChampState>& EnemyTeam);
void cassian_ability(ChampState& champ, std::vector<ChampState>& AllyTeam, std::vector<ChampState>& EnemyTeam);
void sable_ability(ChampState& champ, std::vector<ChampState>& AllyTeam, std::vector<ChampState>& EnemyTeam);
void goliath_ability(ChampState& champ, std::vector<ChampState>& AllyTeam, std::vector<ChampState>& EnemyTeam);
void solarix_ability(ChampState& champ, std::vector<ChampState>& AllyTeam, std::vector<ChampState>& EnemyTeam);
const std::unordered_map<std::string, AbilityFunction> ABILITY_MAP = {
    {"Akira",     akira_ability},
    {"Totom",     totom_ability},
    {"Asura",     asura_ability},
    {"Dante",     dante_ability},
    {"Takeshi",   takeshi_ability},
    {"Draco",     draco_ability},
    {"Lyra",      lyra_ability},
    {"Orion",     orion_ability},
    {"Andromeda", andromeda_ability},
    {"Delphinus", delphinus_ability},
    {"Hades",     hades_ability},
    {"Thanatos",  thanatos_ability},
    {"Vesper",    vesper_ability},
    {"Cassian",   cassian_ability},
    {"Sable",     sable_ability},
    {"Goliath",   goliath_ability},
    {"Solarix",   solarix_ability},
};
void Ability(ChampState &champ, std::vector<ChampState>& AllyTeam, std::vector<ChampState>& EnemyTeam);