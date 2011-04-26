void TeaBot::adv() {
   if (!Broodwar->enemy()->getUnits().empty()) {
      state = ENGAGE;
      engage();
      return;
   }
   moveToCentre();
}
