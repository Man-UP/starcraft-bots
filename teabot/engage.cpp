void TeaBot::engage() {
   if (Broodwar->enemy()->getUnits().empty()) {
      state = ADV;
      adv();
      return;
   }
   map<Unit*,int> attacked;
   set<Unit*> units = Broodwar->self()->getUnits();
   set<Unit*> enemies = Broodwar->enemy()->getUnits();
   double x2, y2;
   findCentre(enemies, x2, y2);
   forUnitSet (enemy, enemies) {
      Unit *target = (*enemy)->getOrderTarget();
      if (target && target->exists())
         if (target->getPlayer() == Broodwar->self())
            if (attacked.find(target) != attacked.end())
               ++attacked[target];
            else
               attacked[target] = 1;
   }
   if (mapType == ZEALOT) {
      forUnitSet (unit, units)
         if (targets[*unit] == NULL || !targets[*unit]->exists() || (*unit)->isAccelerating()) {
            Unit *newTarget = aquireTarget(*unit);
            if (newTarget != targets[*unit]) {
               targets[*unit] = newTarget;
               attackTarget();
            }
         }
   } else if (mapType == DRAGOON) {
      Unit *target = aquireTarget(NULL);
      forUnitSet (unit, units) {
         if (attacked.find(*unit) != attacked.end() && attacked[*unit] > 1) {
            Position loc = Position(2 * ((*unit)->getPosition().x()) - (int)x2, 2 * ((*unit)->getPosition().y()) - (int)y2);
            (*unit)->move(loc);
         }
         else if (targets[*unit] == NULL || !targets[*unit]->exists() || (*unit)->isAccelerating()) {
            if (target != targets[*unit]) {
               targets[*unit] = target;
               attackTarget();
            }
         }
      }
   } else if (mapType == MUTA) {
      forUnitSet (unit, units) {
         if ((*unit)->getType() == UnitTypes::Zerg_Mutalisk) {
            if (attacked.find(*unit) != attacked.end() && attacked[*unit] > 2) {
               Position loc = Position(2 * ((*unit)->getPosition().x()) - (int)x2, 2 * ((*unit)->getPosition().y()) - (int)y2);
               (*unit)->move(loc);
            } else {
               mutaAttack(*unit);
               attackTarget();
            }
         }
         else {
            scourgeAttack(*unit);
            attackTarget();
         }
      }
   }
}
void TeaBot::scourgeAttack(Unit *unit) {
   set<Unit*> enemies = Broodwar->enemy()->getUnits();
   forUnitSet (enemy, enemies) {
      if ((*enemy) == NULL || (*enemy)->exists())
         continue;
/*      if (scTargets.find(*enemy) != scTargets.end())
         continue;*/
      if ((*enemy)->getType() == UnitTypes::Zerg_Scourge)
         continue;
      targets[unit] = (*enemy);
      scTargets.insert(*enemy);
      break;
   }
}
void TeaBot::mutaAttack(Unit *unit) {
   set<Unit*> enemies = Broodwar->enemy()->getUnits();
   Unit *closestEnemy;
   double leastTravelDistance = numeric_limits<double>::max();
   int minHp = numeric_limits<int>::max();
   int bestId = 0;
   forUnitSet (enemy, enemies) {
      double travelDistance = distance(unit, *enemy);
      int hp = totalHp(*enemy);
      int thisId = (*enemy)->getID();
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
   targets[unit] = closestEnemy;
}