#pragma once
#include <iostream>
#include <unordered_map>
#include <string>
#include "json.hpp"
#include "raylib.h"
#include <fstream>
#include "Traits.h"
void LoadTraits() {
    std::string path = "data/Traits.json";
    std::ifstream f(path);
    nlohmann::json data = nlohmann::json::parse(f);
    for (auto& t : data) {
        TraitDef* trait = new TraitDef();
        trait->name        = t["name"];
        trait->thresholds  = t["thresholds"].get<std::vector<int>>();
        trait->hp_modifier = t["hp_modifier"].get<std::vector<float>>();
        trait->ad_modifier = t["ad_modifier"].get<std::vector<float>>();
        trait->ap_modifier = t["ap_modifier"].get<std::vector<float>>();
        trait->lifesteal   = t["lifesteal"];
        trait->execute     = t["execute"].get<std::vector<float>>();
        TRAIT_POOL[TRAIT_TRANSFORM.at(trait->name)] = trait;
    }
}