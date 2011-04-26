#include "CraigacpBot.h"
using namespace std;
using namespace BWAPI;

bool analyzed;
bool analysis_just_finished;
Enemy enemyClass;
MatchType game_type;

void CraigacpBot::onStart()
{
  Broodwar->printf("PlasmaBot");
	//Broodwar->setLocalSpeed(FB_LOCAL_SPEED);
  
  // Find the centre of the map.
  mapCentre = Position(Broodwar->mapWidth() * TILE_SIZE / 2,
                       Broodwar->mapHeight() * TILE_SIZE / 2);

  // We don't have a target until we've scouted one out.
  target = NULL;

  //read map information into BWTA so terrain analysis can be done in another thread
  BWTA::readMap();
  analyzed = false;

  Broodwar->printf("Analyzing map... this may take a minute");
  CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AnalyzeThread, NULL, 0, NULL);
  
  enemyClass = Enemy();

  set<Unit*> allDudesInTheWorld = Broodwar->self()->getUnits();
  switch (allDudesInTheWorld.size()) {
    case 9:
      Broodwar->sendText("Woah dudes, it's the map what has zealots init.");
      game_type = Zealots;
      group_count = 3;
      break;
    case 12:
      Broodwar->sendText("Arrr! Here there be Dragooooooons!");
      game_type = Dragoons;
      group_count = 2;
      break;
    case 24:
      Broodwar->sendText("Mutalisks lol.");
      game_type = Mutalisks;
      group_count = 15;
      break;
  }
  int count = 0;
  int base = 0;
  if (game_type == Mutalisks) {
    dudeGroups = new Group[group_count];
    set<Unit*> mutas;
    for (set<Unit*>::iterator dude = allDudesInTheWorld.begin(); dude != allDudesInTheWorld.end(); dude++) {
      if ((*dude)->getType().groundWeapon().maxRange() == 0) {
        dudeGroups[count].addDude(*dude, DudeInfo());
        count++;
      } else {
        mutas.insert(*dude);
      }
    }
    allDudesInTheWorld = mutas;
    base = count;
    count = 0;
  } else {
    dudeGroups = new Group[group_count];
  }
  for (set<Unit*>::iterator dude = allDudesInTheWorld.begin(); dude != allDudesInTheWorld.end(); dude++) {
    dudeGroups[base + count].addDude(*dude, DudeInfo());
    count = ++count % (group_count - base);
  }
  switch (game_type) {
    case Dragoons:
      for (int i=0; i < group_count; i++) {
        dudeGroups[i].move(mapCentre + Position((i * 200) - 100, (i * 200) - 100));
      }
      break;
    case Zealots:
      dudeGroups[0].move(mapCentre);
      dudeGroups[1].move(mapCentre + Position(0, 150));
      dudeGroups[2].move(mapCentre + Position(0, 300));
      break;
    case Mutalisks:
      for (int i=0; i < base; i++) {
        dudeGroups[i].move(mapCentre + Position(0, i * 30));
      }
      for (int i=base; i < group_count; i++) {
        dudeGroups[i].move(mapCentre + Position(50, (i - 10) * 50));
      }
      break;
  }
}

void CraigacpBot::onFrame()
{
  enemyClass.onFrame();
  if (rand() % 200 == 0) {
    Broodwar->sendText("Lolololololololololol.");
  } else if (rand() % 200 == 0) {
    Broodwar->sendText("If you like losing to THE PLASMABOT so much, why don't you write a book about it?");
  } else if (rand() % 200 == 0) {
    Broodwar->sendText("So is your face!");
  } else if (rand() % 200 == 0) {
    Broodwar->sendText("What an incredible smell you've discovered.");
  }

  for (int i=0; i < group_count; i++) {

    if (dudeGroups[i].target == NULL) {
      if (rand() % 100 > 30) {
        dudeGroups[i].attack(enemyClass.getClosestValidEnemy(dudeGroups[i].centroid,i));
      } else {
        dudeGroups[i].attack(enemyClass.getWeakestEnemy());
      }
    }
    else if(!(dudeGroups[i].target->exists()))
    {
      Broodwar->sendText("Once again, one of your pathetic units is crushed beneath the mighty hoof of the PlasmaBot");
      if (rand() > 0.3) {
        dudeGroups[i].attack(enemyClass.getClosestValidEnemy(dudeGroups[i].centroid,i));
      } else {
        dudeGroups[i].attack(enemyClass.getWeakestEnemy());
      }
    }

    dudeGroups[i].onFrame();

  }

  if (analysis_just_finished)
  {
      Broodwar->printf("Finished analyzing map.");
      analysis_just_finished=false;
  }
}

/**
 * Target the enemy that causes our units to travel the least to
 * attack. If no enemies are visible, do nothing.
 */
Unit *CraigacpBot::tryAquireTarget()
{
 
}

void CraigacpBot::attackTarget()
{
	
}

void CraigacpBot::moveToCentre()
{
 
}

void CraigacpBot::drawMarkings()
{
    if (target != NULL && target->exists())
    {
        // Mark target on map.
        Position p = target->getPosition();
        Broodwar->drawBoxMap(p.x() - 2, p.y() - 2, p.x() + 2, p.y() + 2, Colors::Red, true);
    }
}


void CraigacpBot::onSendText(string text)
{
    if (text=="/attack")
    {
    }
    else if(text=="/coward")
    {
    }
    
        Broodwar->printf("You typed '%s'!",text.c_str());
        Broodwar->sendText("%s",text.c_str());
}


void CraigacpBot::onReceiveText(Player* player, string text)
{
    Broodwar->printf("%s said '%s'", player->getName().c_str(), text.c_str());
}

void CraigacpBot::onUnitDiscover(Unit* discoveredUnit)
{
  if (discoveredUnit->getPlayer() == Broodwar->enemy())
  {
    enemyClass.addEnemy(discoveredUnit);
  }
}

// Callback stubs
void CraigacpBot::onEnd(bool isWinner) {}
void CraigacpBot::onNukeDetect(Position target) {}
void CraigacpBot::onPlayerLeft(Player* player) {}
void CraigacpBot::onSaveGame(string gameName) {}
void CraigacpBot::onUnitCreate(Unit* unit) {}
void CraigacpBot::onUnitDestroy(Unit* unit) {}
void CraigacpBot::onUnitEvade(Unit* unit) {}
void CraigacpBot::onUnitHide(Unit* unit) {}
void CraigacpBot::onUnitMorph(Unit* unit) {}
void CraigacpBot::onUnitRenegade(Unit* unit) {}
void CraigacpBot::onUnitShow(Unit* unit) {}


DWORD WINAPI AnalyzeThread()
{
    BWTA::analyze();
    analyzed   = true;
    analysis_just_finished = true;
    return 0;
}
