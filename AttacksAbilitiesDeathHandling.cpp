#pragma once
#include "Champ.h"
#include "EnemyFinding.h"
#include <functional>
#include "Grid.h"
#include <cmath>
#include "Log.h"
#include "Combat.h"
#include "AttacksAbilitiesDeathHandling.h"
void managedamage(ChampState& champ1,ChampState& champ2, float dmg, std::vector<ChampState> &AllyTeam, std::vector<ChampState> &EnemyTeam){
    float current_time = seconds_in_combat;
    if(current_time - champ2.is_invulnerable_until < 0){
        dmg = 0;
        Log(champ2.def.name + " is invulnerable, can't deal damage");
    }
    if (champ2.current_shield >0){
        if (champ2.current_shield >= dmg){
            champ2.current_shield -= dmg;
            dmg = 0;
        }
        else{
            dmg -= champ2.current_shield;
            champ2.current_shield = 0;
        }
    }
    champ2.hp_current -= dmg;
    TimedTexts.push_back(TimedText(std::to_string(int(dmg)),champ2.pos.q, champ2.pos.r, current_time));
    if (champ2.def.name == "Draco"){
        for(ChampState &champ: EnemyTeam){
            if (champ.def.name == "Lyra") champ.current_shield += 0.1f * dmg;
        }
    }
    if (champ1.def.name == "Lyra"){
        for(ChampState &champ: AllyTeam){
            if (champ.def.name == "Draco") champ.hp_current = std::min(champ.hp_max,champ.hp_current + 0.1f * dmg);
        }
    }
    if (champ1.lifesteal > 0){
        float lifesteal_amount = (champ1.lifesteal/100)*dmg;
        champ1.hp_current += lifesteal_amount;
        if (champ1.hp_current >= champ1.hp_max) champ1.hp_current = champ1.hp_max;
        Log("Champion " + champ1.def.name + " lifesteals " + std::to_string(lifesteal_amount));
    }
    if (champ1.execute > 0){
        if(champ2.hp_current<= champ2.hp_max*(champ1.execute/100)){
            Log("Champion " + champ1.def.name + " executes " + champ2.def.name + " with " + std::to_string(champ2.hp_current) + " health");
            champ2.hp_current = 0;
        }
    }
    if (champ2.hp_current<=0){
        Log("Champion " + champ2.def.name + "died");
        champ2.is_dead = true;
    }
}
void autoattack(ChampState& champ, std::vector<ChampState> &AllyTeam, std::vector<ChampState> &EnemyTeam){
    if (champ.enemytarget == nullptr){
        Log("No enemy target found for " + champ.def.name);
        return;
    }
    float damage = champ.ad_current*(100/(100+champ.enemytarget->armor_current));
    Log("Champion " + champ.def.name + " attacks with AD: " + std::to_string(champ.ad_current) + " to " + champ.enemytarget->def.name + " dealing " + std::to_string(damage));
    managedamage(champ, *champ.enemytarget, damage, AllyTeam, EnemyTeam);
    champ.mana_current += 5;
}
using AbilityFunction = std::function<void(ChampState&, std::vector<ChampState>&, std::vector<ChampState>&)>;
void akira_ability(ChampState& champ, std::vector<ChampState>& AllyTeam, std::vector<ChampState>& EnemyTeam){
    std::vector<ChampState> targets;
    for (ChampState& enemy: EnemyTeam){
        if (enemy.is_dead) continue;
        if (targets.size() < 3){
            targets.push_back(enemy);
        }
        else{
            int max_distance = 0;
            int max_index = 0;
            for (int i = 0; i<targets.size(); i++){
                int dis = distance(champ.pos, targets[i].pos);
                if (dis > max_distance){
                    max_distance = dis;
                    max_index = i;
                }
            }
            if (distance(champ.pos, enemy.pos) < max_distance){
                targets[max_index] = enemy;
            }
        }
    }
    for (ChampState& target: targets){
        float damage = champ.ap_current*(100/(50+target.magicres_current));
        managedamage(champ, target, damage, AllyTeam, EnemyTeam);
        Log("Champion " + champ.def.name + " uses ability on " + target.def.name + " dealing " + std::to_string(damage) + " damage");
    }
}
void totom_ability(ChampState& champ, std::vector<ChampState>& AllyTeam, std::vector<ChampState>& EnemyTeam){
    float x = champ.current_shield;
    champ.current_shield += 0.1f*champ.hp_current;
    Log("Champion " + champ.def.name + " uses ability gaining a shield of " + std::to_string(champ.current_shield - x));
}
void asura_ability(ChampState& champ, std::vector<ChampState>& AllyTeam, std::vector<ChampState>& EnemyTeam){
    //Programmed inside EnemyFinding.h
}
void dante_ability(ChampState& champ, std::vector<ChampState>& AllyTeam, std::vector<ChampState>& EnemyTeam){
    float damage = champ.ap_current*(100/(50+champ.enemytarget->magicres_current));
    managedamage(champ, *champ.enemytarget, damage, AllyTeam, EnemyTeam);
    Log("Champion " + champ.def.name + " uses ability on " + champ.enemytarget->def.name + " dealing " + std::to_string(damage) + " damage");
}
void takeshi_ability(ChampState& champ, std::vector<ChampState>& AllyTeam, std::vector<ChampState>& EnemyTeam){
    if (champ.star==0) champ.ad_current += 10;
    else if (champ.star==1) champ.ad_current += 15;
    else if (champ.star==2) champ.ad_current += 25;
    float extra_ad = champ.ad_current-champ.def.ad[champ.star];
    Log("Champion " + champ.def.name + " uses ability getting " + std::to_string(extra_ad) + " more ad");
    autoattack(champ, AllyTeam, EnemyTeam);
    champ.ad_current -= extra_ad;
}
void draco_ability(ChampState& champ, std::vector<ChampState>& AllyTeam, std::vector<ChampState>& EnemyTeam){
    for (ChampState& ally: AllyTeam){
        if (ally.is_dead) continue;
        if (distance(champ.pos, ally.pos) <=1){
            ally.current_shield += 0.3f*champ.def.hp[champ.star];
            ally.hp_current += 0.1f*champ.hp_current;
            Log("Champion " + champ.def.name + " uses ability on " + ally.def.name + " gaining a shield of " + std::to_string(ally.current_shield) + " and healing " + std::to_string(0.1f*champ.hp_current));
        }
    }
}
void lyra_ability(ChampState& champ, std::vector<ChampState>& AllyTeam, std::vector<ChampState>& EnemyTeam){
    Log("Champion " + champ.def.name + " uses ability");
    std::vector<GridPos> Hexes = Trajectory(champ.pos,champ.enemytarget->pos);
    int targetshit = 0;
    for (GridPos& Hex: Hexes){
        if (champ.pos == Hex) continue;
        for (ChampState& enemy: EnemyTeam){
            if (enemy.is_dead) continue;
            if(Hex == enemy.pos){
                float damage = champ.ad_current*(100/(100+enemy.armor_current))*(1+(1.0f/(2+targetshit)));
                Log("Champion " + champ.def.name + " deals " + std::to_string(damage) + " damage to " + enemy.def.name);
                managedamage(champ, enemy, damage, AllyTeam, EnemyTeam);
                targetshit++;
                break;
            }
        }
    }
    Log(std::to_string(targetshit) + " enemies hit");
}
void orion_ability(ChampState& champ, std::vector<ChampState>& AllyTeam, std::vector<ChampState>& EnemyTeam){
    if (champ.star==0) champ.ad_current += 15;
    else if (champ.star==1) champ.ad_current += 25;
    else if (champ.star==2) champ.ad_current += 40;
    float extra_ad = champ.ad_current-champ.def.ad[champ.star];
    Log("Champion " + champ.def.name + " uses ability getting " + std::to_string(extra_ad) + " more ad");
    autoattack(champ, AllyTeam, EnemyTeam);
    champ.ad_current -= extra_ad;
}
void andromeda_ability(ChampState& champ, std::vector<ChampState>& AllyTeam, std::vector<ChampState>& EnemyTeam){
    float x = champ.ad_current;
    champ.ad_current *= 1.25f;
    Log("Champion " + champ.def.name + " uses ability getting " + std::to_string(0.25f*champ.ad_current) + " more ad");
    autoattack(champ, AllyTeam, EnemyTeam);
    champ.ad_current = x;
}
void delphinus_ability(ChampState& champ, std::vector<ChampState>& AllyTeam, std::vector<ChampState>& EnemyTeam){
    for (ChampState& ally: AllyTeam){
        if (ally.is_dead) continue;
        float missing_health = ally.def.hp[ally.star]-ally.hp_current;
        ally.hp_current += 0.3f*missing_health;
        Log("Champion " + champ.def.name + " uses ability on " + ally.def.name + " healing " + std::to_string(0.3f*missing_health));
    }
}
void hades_ability(ChampState& champ, std::vector<ChampState>& AllyTeam, std::vector<ChampState>& EnemyTeam){
    Log("Champion " + champ.def.name + " uses ability");
    std::vector<GridPos> Hexes = Trajectory(champ.pos,champ.enemytarget->pos);
    int targetshit = 0;
    for (GridPos& Hex: Hexes){
        if (champ.pos == Hex) continue;
        for (ChampState& enemy: EnemyTeam){
            if (enemy.is_dead) continue;
            if(Hex == enemy.pos){
                float damage = champ.ap_current*(100/(50+enemy.magicres_current))*(1+(1.0f/(2+targetshit)));
                Log("Champion " + champ.def.name + " deals " + std::to_string(damage) + " damage to " + enemy.def.name);
                managedamage(champ, enemy, damage, AllyTeam, EnemyTeam);
                targetshit++;
                break;
            }
        }
    }
    Log(std::to_string(targetshit) + " enemies hit");
}
void thanatos_ability(ChampState& champ, std::vector<ChampState>& AllyTeam, std::vector<ChampState>& EnemyTeam){
    if (champ.star==0) champ.ad_current += 15;
    else if (champ.star==1) champ.ad_current += 25;
    else if (champ.star==2) champ.ad_current += 40;
    float extra_ad = champ.ad_current-champ.def.ad[champ.star];
    Log("Champion " + champ.def.name + " uses ability getting " + std::to_string(extra_ad) + " more ad");
    autoattack(champ, AllyTeam, EnemyTeam);
    champ.ad_current -= extra_ad;
}
void vesper_ability(ChampState& champ, std::vector<ChampState>& AllyTeam, std::vector<ChampState>& EnemyTeam){
    champ.current_shield += 0.15f*champ.def.hp[champ.star];
    Log("Champion " + champ.def.name + " uses ability gaining a shield of " + std::to_string(champ.current_shield));
}
void cassian_ability(ChampState& champ, std::vector<ChampState>& AllyTeam, std::vector<ChampState>& EnemyTeam){
    float heal = 0.2f*(champ.def.hp[champ.star]-champ.hp_current);
    float shield = 0.1f*champ.def.hp[champ.star];
    champ.hp_current += heal;
    champ.current_shield += shield;
    Log("Champion " + champ.def.name + " uses ability healing " + std::to_string(heal) + " and gaining a shield of " + std::to_string(shield));
}
void sable_ability(ChampState& champ, std::vector<ChampState>& AllyTeam, std::vector<ChampState>& EnemyTeam){
    champ.lifesteal += 20;
    Log("Champion " + champ.def.name + " uses ability gaining 20% lifesteal for the next autoattack");
    autoattack(champ, AllyTeam, EnemyTeam);
    champ.lifesteal -= 20;
}
void goliath_ability(ChampState& champ, std::vector<ChampState>& AllyTeam, std::vector<ChampState>& EnemyTeam){
    float time = seconds_in_combat;
    champ.is_invulnerable_until = time + 2.0f;
    Log("Champion " + champ.def.name + " has activated ability becoming invulnerable for 2 seconds");
}
void solarix_ability(ChampState& champ, std::vector<ChampState>& AllyTeam, std::vector<ChampState>& EnemyTeam){
    for (ChampState& enemy: EnemyTeam){
        if (enemy.is_dead) continue;
        float damage = champ.ap_current*(100/(50+enemy.magicres_current));
        managedamage(champ, enemy, damage, AllyTeam, EnemyTeam);
        Log("Champion " + champ.def.name + " uses ability on " + enemy.def.name + " dealing " + std::to_string(damage) + " damage");
    }
}
void Ability(ChampState &champ, std::vector<ChampState>& AllyTeam, std::vector<ChampState>& EnemyTeam){
    Log("Champion " + champ.def.name + " is using their ability");
    AbilityFunction ability = ABILITY_MAP.at(champ.def.name);
    ability(champ, AllyTeam, EnemyTeam);
}