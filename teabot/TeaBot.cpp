#include "stuff.cpp"

void TeaBot::onStart() {
   //Broodwar->printf("TeaBot");
   //Broodwar->setLocalSpeed(FB_LOCAL_SPEED);

   INIT = 1; ADV = 2; ENGAGE = 3;
   state = INIT;
   DRAGOON = 1; MUTA = 2; ZEALOT = 3;

   // We don't have a target until we've scouted one out.
   set<Unit*> units = Broodwar->self()->getUnits();
   forUnitSet (unit, units)
      targets[*unit] = NULL;

   if ((*units.begin())->getType() == UnitTypes::Protoss_Zealot)
      mapType = ZEALOT;
   else if ((*units.begin())->getType() == UnitTypes::Protoss_Dragoon)
      mapType = DRAGOON;
   else {
      mapType = MUTA;
   }

   // Find the centre of the map.
   findCentre(units, x1, y1);
   home = Position((int)x1, (int)y1);
   //if (mapType == ZEALOT || mapType == DRAGOON)
      mapCentre = Position((Broodwar->mapWidth() * TILE_SIZE / 2),//  + (int)x1) / 5,
                           Broodwar->mapHeight() * TILE_SIZE / 2 );
   //else
      //mapCentre = Position((Broodwar->mapWidth() * TILE_SIZE * 4 / 5), Broodwar->mapHeight() * TILE_SIZE / 2);

   // Start scouting.
   init();
   //moveToCentre();
}

void TeaBot::attackTarget() {
   // All units attack the target.
   set<Unit *> units = Broodwar->self()->getUnits();
   forUnitSet(unit, units)
      (*unit)->attackUnit(targets[*unit]);
}

void TeaBot::moveToCentre() {
   // Move all units to the centre of the map.
   set<Unit *> units = Broodwar->self()->getUnits();
   forUnitSet(unit, units)
	   (*unit)->move(mapCentre);
}

void TeaBot::findCentre(set<Unit*> units, double &x, double &y) {
   x = y = 0;
   forUnitSet (unit, units) {
      x += (*unit)->getPosition().x();
      y += (*unit)->getPosition().y();
   }
   x /= units.size();
   y /= units.size();
}

Unit* TeaBot::aquireTarget(Unit *unit) {
if (mapType == ZEALOT || mapType == MUTA) {
   set<Unit*> enemies = Broodwar->enemy()->getUnits();
   Unit *closestEnemy;
   double leastTravelDistance = numeric_limits<double>::max();
   int minHp = numeric_limits<int>::max();
   int bestId = 0;
   forUnitSet (enemy, enemies) {
      double travelDistance = distance(unit, *enemy);
      int hp = totalHp(*enemy);
      int thisId = (*enemy)->getID();
      if (mapType == ZEALOT) {
      // Prioritise distance
         if (travelDistance < leastTravelDistance) {
            closestEnemy = *enemy;
            leastTravelDistance = travelDistance;
            minHp = hp;
            bestId = thisId;
         } else if (travelDistance == leastTravelDistance)
            if (hp < minHp) {
               closestEnemy = *enemy;
               //leastTravelDistance == travelDistance;
               minHp = hp;
               bestId = thisId;
            } else if (hp == minHp)
               if (thisId < bestId) {
               closestEnemy = *enemy;
               //leastTravelDistance == travelDistance;
               //minHp = hp
               bestId = thisId;
            }
      } else if (mapType == MUTA) {
         // Prioritise hp
         if (hp < minHp) {
            closestEnemy = *enemy;
            leastTravelDistance = travelDistance;
            minHp = hp;
         } else if (hp == minHp && distance(unit, *enemy) < leastTravelDistance) {
            closestEnemy = *enemy;
            leastTravelDistance = travelDistance;
         }
      }
   }
   return closestEnemy;
} else { //(mapType == DRAGOON)
   set<Unit *> emenies = Broodwar->enemy()->getUnits();
	set<Unit *> units = Broodwar->self()->getUnits();

	Unit *closestEnemy;
   double leastTotalTravelDistance = numeric_limits<double>::max();

	for (set<Unit *>::iterator enemy = emenies.begin(); enemy != emenies.end(); enemy++) {
		double totalTraveldistance = 0.0;
        for (set<Unit *>::iterator unit = units.begin(); unit != units.end(); unit++) {
            double travelDistance = (*unit)->getDistance(*enemy) - FB_DRAGOON_WEAPON_RANGE;
            if (travelDistance > 0)
                totalTraveldistance += travelDistance;
		}
		if (totalTraveldistance < leastTotalTravelDistance) {
			closestEnemy = *enemy;
			leastTotalTravelDistance = totalTraveldistance;
      } else if (totalTraveldistance == leastTotalTravelDistance) {
         if (closestEnemy->getID() < (*enemy)->getID()) {
            closestEnemy = *enemy;
         }
      }
	}
    return closestEnemy;
}
}