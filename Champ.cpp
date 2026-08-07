#pragma once
#include <iostream>
#include <array>
#include <vector>
#include <string>
#include "json.hpp"
#include <fstream>
#include "Grid.h"
#include "Traits.h"
#include "Champ.h"
void LoadChampions() {
    std::string path = "data/ChampionPool.json";
    std::ifstream f(path);
    nlohmann::json cdata = nlohmann::json::parse(f);
    for (auto& c : cdata) {
        ChampDef def;
        def.name     = c["name"];
        def.cost     = c["cost"];
        def.range    = c["range"];
        def.mana_max = c["mana_max"];
        def.Targeting = TARGETING_POOL.at(c["Targeting"]);
        def.hp       = {c["hp"][0], c["hp"][1], c["hp"][2]};
        def.armor    = {c["armor"][0], c["armor"][1], c["armor"][2]};
        def.magicres = {c["magicres"][0], c["magicres"][1], c["magicres"][2]};
        def.ad       = {c["ad"][0], c["ad"][1], c["ad"][2]};
        def.ap       = {c["ap"][0], c["ap"][1], c["ap"][2]};
        def.attackspeed = {c["attackspeed"][0], c["attackspeed"][1], c["attackspeed"][2]};
        for (auto& trait_name : c["traits"]) {
            def.ChampTraits.push_back(TRAIT_TRANSFORM.at(trait_name));
        }
        CHAMP_STORAGE[def.name] = def;
    }
    for (auto& [name, def] : CHAMP_STORAGE) {
        CHAMP_POOL[name] = &def;
    }
}
ChampState CreateChampion(std::string name, int star){
    return ChampState(*CHAMP_POOL.at(name),star);
}