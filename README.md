# Autobattler-combat-simulator-inspired-by-TFT
A TFT-inspired autobattler combat simulator built in C++ with Raylib.

This project features a hex grid system, combat optimized with spatial hashing, specific abilities for each champion, custom data architecture, per-star stats and a trait sinergy system. It is built as a portfolio project aiming towards AAA game studio roles.

Current Status, Final Step: Polishing, project is pretty much done and can see a fully functional combat using console inputs to select champions, might think about adding a team selection screen, combat and traits is fully done, have succeeded in solving primitive combat rounds, death handling complete, enemy targeting complete, hex grid initialization complete, trait application complete for the most part, champion pool definitions complete(17/17 Champion, balance will be done as part of the polish), trait enumeration complete, stats system complete, team creation complete.

I have managed to bring forth a data-driven approach to the champions and traits architecture, the next days will be focused on polishing the code and cleaning out things that may have been hardcoded.

While polishing I have decided to make the code more readable and industry-standard, for this reason I have decided to move all logic from the header files into different cpp files with the same name as them, I have also changed main.cpp so that the combat calculations and visuals are separated, What`s left is to keep polishing and to implement (if possible) a better pathfinding to enemies.


Over the past week I have added a Team Selection screen and separated the simulator into different stages (Planning, Combat, End) What is left to do is create a End Screen and implement A* Pathfinding(Which is proving to be more difficult than I thought).
