void TeaBot::init() {
   if (mapType == MUTA) {
      state = ADV;
   } else if (mapType == DRAGOON) {
      state = ADV;
   } else /*mapType == ZEALOT*/ {
      state = ADV;
   }
}

bool TeaBot::initDone() {
   if (mapType == MUTA) {
      return (Broodwar->unitsOnTile((int)x1, (int)y1).size() == Broodwar->self()->getUnits().size());
   } else if (mapType == DRAGOON) {
      return true;
   } else {
      return true;
   }
}