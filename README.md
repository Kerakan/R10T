# [Project R10T] — C++ Autobattler Combat Simulator

A data-driven Autobattler combat simulator built on C++ using raylib for the visual GUI, it includes a custom hex grid system, A* pathfinding, 17 custom champions and abilities to go with each of them as well as custom traits.

<img width="1919" height="1079" alt="image" src="https://github.com/user-attachments/assets/6d07c129-7f60-4605-b1db-01967aa81fca" />


[Demo Video](TO BE MADE)

---

## What Is This?

This is a data-driven autobattler combat simulator built from scratch in C++. Inspired by TFT's combat systems, it takes a different approach — rather than implementing the full autobattler loop, it provides a sandbox where you configure two boards freely and simulate combat directly. TFT has no sandbox mode; building one turned out to be both a more interesting engineering problem and a more useful tool for exploring how unit composition and trait synergies actually interact at a systems level. Every system — the hex grid, the modifier layer, the combat engine, the pathfinding — all custom-built with no game engine dependencies.

The program is built across 10+ systems decoupled from one another. A data-driven approach was taken such that a JSON pipeline loads champions and traits definitions at runtime, this way balancing champions and implementing new ones requires no code changes. The combat engine runs on a fixed timestep such that there are no hardware advantages and it handles targeting, attack speed, mana management, death, retargeting. Unit stats are modified using a modifier layer that handles all buffs and percentage multipliers coming from traits or abilities simultaneously. As for the hex grid, it uses axial oriented coordinates with an implemented A* pathfinding algorithm that uses early exit for ranged units, the Raylib renderer gets executed outside of the main combat loop so combat can be resolved without having to render.

I purposely decided to build everything from scratch instead of relying on a game engine, this meant every system from abilities to pathfinding was a genuine architectural challenge that I needed to tackle instead of just using an engine default. The sandbox format required having to design a system that could deal with any arbitrary configuration of teams. This project also directly reflects my Mathematics and Statistics background directly — damage formulas, balance, trait thresholds, projectile trajectory calculations for piercing abilities — are all mathematically justified instead of done by feeling, I will go into it in the Design/Brainstorming section.

---

## Architecture Overview

,,,,,

### Key Design Principles

- **Data-driven design** — Champions, traits, and items are defined entirely in JSON, decoupling content from code so balancing and new content require zero code changes.
- **Modifier layer separation** — All stat changes from traits, items, and abilities flow through a single centralised modifier layer, keeping interactions predictable and new modifiers self-contained.
- **Engine-agnostic combat** — The combat engine has zero dependency on Raylib, meaning simulations run independently of rendering and the visual layer could be swapped entirely without touching combat logic.

---

## Systems Breakdown

### Unit and Stat System

Every unit is defined with three sets of base stats corresponding to their star level — HP, attack damage, ability power, armor, magic resistance, attack speed, mana, and range (mana and range don't change with star level). During combat, a separate stat layer sits on top of these base values and accumulates all active modifiers from traits, items, and abilities using both addition and percentage multiplication. The key architectural decision was keeping base stats immutable during combat, this way base stats are only accessed at the start of combat and only effective stats are modified, which means removing a buff or debuff is always clean and predictable regardless of how many modifier sources are active simultaneously.

### Trait System

Each team's active traits are determined by counting how many units on the board share a given trait, with each unique champion contributing exactly once regardless of duplicates. When a unit count reaches a trait threshold, that trait activates and applies stat modifiers to all units on the team that carry it, some traits add an execution threshold, others add simple stats like HP or attack damage while others add synergies between champions. Each of the values is applied to the champions on the team being evaluated that have this trait.

### Item System

The item system architecture is implemented and extensible — items are defined via the same JSON pipeline as champions and feed into the modifier layer — though the current build ships without active items to keep balance testing focused on trait and ability interactions.

### Combat Engine

Each combat tick a run_combat function is called, it shuffles all living units from both teams into a single randomised processing order, eliminating any turn order advantage between teams. Each unit then resolves its tick sequentially, if out of range of its target it moves one step along an A* generated path at a fixed movement speed, then if their autoattack is off cooldown they autoattack gaining 5 mana and dealing damage, after that if they have enough mana to cast their ability they do so immediately, if the targeted enemy dies the unit retargets on the next tick.

### Hex Grid and Pathfinding

The board gets represented as a hex grid using axial coordinates with pointy-top orientation in order to make calculations more intuitive, A* uses cube distance as its heuristic, admissible on a uniform cost hex grid where every step costs 1. A* guarantees that the optimal path to the enemy is chosen and exiting early whenever the enemy target is in range of the attacker unit, this way both ranged and melee units are handled without using unnecessary movements.

### Ability System

Every champion has a unique ability that fires when their mana threshold is reached, dealing either physical or magic damage depending on the champion's design. Abilities cover a deliberate range of archetypes, single target nukes, piercing projectiles, autoattack empowerment, healing, shielding, and invulnerability, ensuring every composition has meaningful ability diversity. New abilities are added by implementing them in a single file and assigning them to a champion, with no modifications to the combat loop required

### Renderer

The renderer is built as a pure read-only layer on top of the combat engine, it reads game state each frame but never modifies it, meaning combat logic and visual representation are fully decoupled. Raylib handles all visual output across three distinct screens: the team configuration screen where boards are assembled, the combat screen with the hex grid, health bars, and floating damage numbers, and the post-round result screen. The hex grid visually differentiates starting zones for each team, and a live combat log displays damage events and ability casts as they occur. All draw calls are scaled via a resolution-independent system that derives a uniform scale factor from the current window dimensions.

---

## Development Progression

### Early Iteration — Start of July
![First Visual Combat Iteration](https://github.com/user-attachments/assets/84ff4844-2829-4fc6-94d4-a679685c9ed5)
> At this stage only champions, their abilities, and traits were implemented with a greedy approach to pathfinding. The GUI was minimal — the grid, champions, and health bars were drawn, but the log and start button were non-functional.

### Mid Development — Mid-Late July
![Mid Development](https://github.com/user-attachments/assets/647d1927-76af-4aee-bcd0-f4fe66be0332)
> The GUI began taking shape at this stage, though team creation still relied on console inputs rather than the GUI. The combat phase visuals were nearly identical to the final iteration.

### Final State — Start of August

**Planning Phase — Early**

![First Planning Phase GUI](https://github.com/user-attachments/assets/59e133ec-4015-4ac6-a889-a2d0e8d4342c)

**Planning Phase — Final**

![Final Planning Phase GUI](https://github.com/user-attachments/assets/ba4a9349-e75e-4bcd-8497-99aea90ba441)

**Combat Phase**

![Final Combat Iteration](https://github.com/user-attachments/assets/94cc12c5-c2d0-4d27-af11-938ffd26b8d4)

**End Screen**

![End Screen](https://github.com/user-attachments/assets/d7c73310-c467-45c8-8246-606762c9efae)

> The final version as it appears when compiled and run. A* pathfinding was fully integrated and team selection was moved entirely into the GUI.
---

## Design and Brainstorming

<img width="1080" height="1920" alt="image" src="https://github.com/user-attachments/assets/4e5a1218-dd6a-46b2-81d7-2f299a34d00c" />
<img width="1920" height="1080" alt="WhatsApp Image 2026-08-10 at 9 42 26 AM" src="https://github.com/user-attachments/assets/043866da-2841-4b86-b5fe-23fdddb1400c" />
<img width="1148" height="2040" alt="WhatsApp Image 2026-08-10 at 9 42 25 AM" src="https://github.com/user-attachments/assets/6dc35393-3f3a-4727-8685-9bc08aeb7aaa" />
<img width="1148" height="2040" alt="WhatsApp Image 2026-08-10 at 9 42 25 AM (1)" src="https://github.com/user-attachments/assets/a4958e80-5f0c-49ff-9ad3-287fdb9e799d" />


### Champion Design Philosophy

Each champion was designed around a clearly defined role — bruiser, guardian, mage, sniper, or assassin — with stat distributions that reinforce that role mechanically. Bruisers receive moderate HP, armor, magic resistance, and attack damage for sustained fighting, while snipers trade survivability for high attack damage, attack speed, and range. Stat values were derived by referencing TFT set databases as a baseline and then adjusting independently to fit my sandbox format, where no economy or carousel system exists.

Faction membership was useful both naming and design — each champion's name reflects their faction identity thematically. The roster was deliberately set at 17 champions to support three 5-unit factions with maximum team sizes of 5, leaving two faction-less champions — Goliath and Solarix — who compensate for their lack of faction synergy through higher base stats, Goliath's ability is the only one that allows for invulnerability while Solarix's ability nukes all champions on the enemy team.

### Trait System Design

Traits can be separated into two major archetypes, that is factions(Dark Knights, Shadow Fighters and Celestials) that provide meaningful upgrades and changes to how the rounds play out and classes(Lovers, Bruisers, Snipers, Mages, ...) which usually just upgrade one or two stats of the units affected. A unit can be affected by bonuses from both factions and classes at the same time. Most traits activate at two thresholds — 2 and 3, or 3 and 5 — chosen to reward partial investment while making full 5-unit compositions meaningfully stronger rather than just incrementally better

While most traits have two thresholds at 2/3 or 3/5, Celestials work differently since they modify their stats depending on how many team members with the trait exist. The Lovers trait is the most mechanically distinct, a 2-unit trait between Draco and Lyra that creates a reactive synergy: when Draco takes damage Lyra gains a shield, and when Lyra deals damage Draco heals, making their positioning relative to each other a meaningful tactical decision.

### Balancing Decisions

Physical damage in the simulator follows the standard formula:

**effective_damage = raw_damage × (100 / (100 + armor))**

Magic damage uses a modified version with a lower baseline (may change in some abilities):

**effective_damage = raw_damage × (100 / (50 + magic_resistance))**

This asymmetry is deliberate — at 0 magic resistance a unit takes the full raw AP value doubled, this is important since AP attacks only get activated when the ability is cast, AP compositions are inherently more lethal against unprotected targets. Units only begin meaningfully resisting magic damage above 50 MR, making AP damage punishing against squishies while bruisers and guardians with high MR remain durable against both damage types.

Armor values across roles reflect clear survivability tiers. A 1★ Asura (Sniper) has 15 armor, taking 87% of incoming physical damage — intentionally fragile to reinforce her high-damage low-survivability role. A 1★ Totom (Bruiser) has 40 armor, reducing incoming physical damage to 71%, while a 1★ Goliath (Titan/Guardian) has 50 armor, taking only 67%. At 3★ these gaps widen significantly — Totom reaches 140 armor (42% physical damage taken) and Goliath reaches 175 armor (36% physical damage taken), placing them firmly in the frontline tank range while Snipers at 60 armor remain at 63% damage taken throughout.

Class trait bonuses were calibrated to feel impactful without being mandatory. Bruiser at 2 units grants +5% HP and +10% AD, scaling to +10% HP and +20% AD at 3 units. Sniper grants +15% AD at 2 units and +25% AD at 3 units, amplifying an already high-damage role. Guardian inverts this, granting +15% and +25% HP to reinforce survivability on units with intentionally low AD. Assassin grants 20% lifesteal at 2 units regardless of threshold, synergising with high attack speed to create a sustain profile distinct from Guardian tankiness. Dark Knights apply an execution threshold — units below 5% HP are instantly killed at 3 Dark Knights, rising to 10% at 5 — incentivising burst compositions that punish healing.

Celestials scale continuously rather than at fixed breakpoints: each Celestial unit on the board contributes a +4% multiplier to all stats (AD, AP, HP) of every Celestial champion, applied at combat start. A single Celestial at 1★ receives a 1.04× multiplier, while a full 5-unit Celestial composition receives a 1.20× multiplier across all three stats simultaneously — the only trait in the roster that rewards every additional unit placed rather than hitting discrete thresholds.

Shadow Fighters apply their bonus differently from every other trait — rather than a one-time multiplier at combat start, they receive a compounding +1% to all stats (AD, AP, HP) every second at 3 units, rising to +1.5% per second at 5 units. At the 1% rate a Shadow Fighter unit that survives 10 seconds has received a 1.01^10 ≈ 1.105 multiplier across all three stats, reaching 1.01^20 ≈ 1.22 at second 20. The 1.5% rate compounds to 1.015^20 ≈ 1.35 by second 20 — comparable to a maxed Celestial bonus but earned through combat survival rather than unit count. This makes Shadow Fighters uniquely punishing against slow compositions and rewards building a frontline that can keep Shadow Fighter units alive long enough to snowball their stats.

---

## Technical Challenges

### A* Pathfinding on a Hex Grid

Initial pathfinding used a greedy approach — each unit moved to whichever adjacent hex minimised the distance to its target. This worked in most cases but broke in edge cases where units would stop moving entirely when two equidistant hexes created an unresolvable tie. After researching pathfinding algorithms I implemented A* using axial coordinates with cube distance as the heuristic, chosen because standard Manhattan distance produces incorrect results on a hex grid due to its diagonal movement constraints. The first implementation had a critical bug — only the lowest F cost neighbour was being added to the open list rather than all valid neighbours, with the list then sorted by F cost using H as a tiebreaker. Once corrected, A* resolved all edge cases the greedy approach failed on and handled early exit for ranged units cleanly without special casing.

### Data-Driven Architecture

Implementing a data-driven architecture was a first for this project — all previous work had hardcoded game data directly in source files. After researching C++ JSON libraries, nlohmann/json was chosen for its header-only integration and clean syntax, eliminating any additional build dependencies. The architecture required restructuring how champions and traits were defined, moving all values out of source files and into external JSON files loaded at runtime.

---

## What I Would Do Differently

The most significant change I would make is I wouldn't wait until I am halfway through development to make the change to a data-driven design. Migrating hardcoded champion and trait values into JSON mid-project required restructuring systems that were already built around fixed values, a refactor that would have been unnecessary had I gone with the right architecture from the start. The impact on iteration speed was immediately obvious: being able to adjust a champion's stats without recompiling transformed the balancing process entirely. For the rendering layer, while Raylib was the right choice for this project given its lightweight integration and fast setup, a future project would use a lower-level rendering API such as OpenGL — both to demonstrate graphics programming depth and to work closer to the layer that studios actually build on

---

## How to Build and Run

### Requirements

- C++20 or higher
- CMake 3.x
- Raylib [version]
- nlohmann/json [version]

### Build Instructions for Linux

```bash
git clone https://github.com/Kerakan/Autobattler-combat-simulator-inspired-by-TFT/tree/main
cd R10T
mkdir build && cd build
cmake ..
cmake --build .
./autobattler
```

---

## Project Structure

```
R10T/
├── data/
│   ├── ChampionPool.json       # Champion definitions, stats and traits
│   └── Traits.json             # Trait thresholds and stat modifiers
├── include/
│   ├── raylib/                 # Raylib library
│   ├── json.hpp                # nlohmann/json header-only library
│   ├── Champ.h                 # ChampDef and ChampState structs as well as Team1,Team2 and GamePhase definitions
│   ├── Combat.h                # Combat loop declarations
│   ├── Draw.h                  # Renderer declarations
│   ├── EnemyFinding.h          # Target acquisition declarations
│   ├── Grid.h                  # Hex grid declarations and GridPos definition
│   ├── Log.h                   # Combat logger declarations
│   ├── Traits.h                # Trait definitions and unordered maps
│   ├── TraitStatSystem.h       # Modifier layer declarations
│   └── AttacksAbilitiesDeathHandling.h  # Attack and ability declarations
├── AttacksAbilitiesDeathHandling.cpp    # Attack resolution, ability dispatch, death handling
├── Champ.cpp                   # Champion loading from JSON
├── Combat.cpp                  # Core combat loop and tick processing
├── Draw.cpp                    # Raylib renderer and all visual output
├── EnemyFinding.cpp            # Target acquisition and A* pathfinding
├── Grid.cpp                    # Hex grid representation, distance and trajectory calculations
├── Log.cpp                     # Combat event logging
├── Main.cpp                    # Entry point and initialisation
├── Traits.cpp                  # Trait loading from JSON
├── TraitStatSystem.cpp         # Modifier layer and trait effect application
└── CMakeLists.txt              # Build configuration
```

---

## Skills Demonstrated

- C++ systems architecture and data-driven design
- Hex grid implementation with axial coordinates
- A* pathfinding adapted for ranged unit combat
- Modular modifier and buff system
- JSON-driven game data pipeline
- Resolution-independent rendering with Raylib
- Mathematical balancing and stat system design

---

*Built as a portfolio project targeting gameplay and systems engineering roles. Inspired by the autobattler genre.*
