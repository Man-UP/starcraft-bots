#include "TeaBot.h"
#include "adv.cpp"
#include "engage.cpp"
#include "init.cpp"
#include "retreat.cpp"

void TeaBot::onFrame() {
   /* If we haven't got a target or our target has been destroyed,
    * try to aquire a new target. If a new target cannot be found,
    * start scouting again.
    */
   if (state == INIT)
      init();
   else if (state == ADV)
      adv();
   else if (state == ENGAGE)
      engage();
   drawMarkings();
}

int TeaBot::totalHp(Unit *unit) {
   return unit->getHitPoints() + unit->getShields();
}

double TeaBot::distance(Unit *unit, Unit *enemy) {
   double travelDistance = unit->getDistance(enemy);
   if (unit->getType().isFlyer())
      travelDistance -= unit->getType().airWeapon().maxRange();
   else
      travelDistance -= unit->getType().groundWeapon().maxRange();
   return (travelDistance > 0 ? travelDistance : 0);
}

void TeaBot::drawMarkings() {
   for (map<Unit*, Unit*>::iterator i = targets.begin(); i != targets.end(); ++i)
   if (i->second && i->second->exists() && i->first && i->first->exists()) {
         Position p1 = i->first->getPosition();
         Position p2 = i->second->getPosition();
         Broodwar->drawLineMap(p1.x(), p1.y(), p2.x(), p2.y(), Colors::White);
      }
}

// Callback stubs
void TeaBot::onEnd(bool isWinner) {}
void TeaBot::onNukeDetect(Position target) {}
void TeaBot::onPlayerLeft(Player* player) {}
void TeaBot::onReceiveText(Player* player, string text) {}
void TeaBot::onSaveGame(string gameName) {}
void TeaBot::onSendText(string text) {}
void TeaBot::onUnitCreate(Unit* unit) {}
void TeaBot::onUnitDestroy(Unit* unit) {}
void TeaBot::onUnitDiscover(Unit* discoveredUnit) {}
void TeaBot::onUnitEvade(Unit* unit) {}
void TeaBot::onUnitHide(Unit* unit) {}
void TeaBot::onUnitMorph(Unit* unit) {}
void TeaBot::onUnitRenegade(Unit* unit) {}
void TeaBot::onUnitShow(Unit* unit) {}