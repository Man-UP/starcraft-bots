#pragma once
#define NOMINMAX
#include <windows.h>
#include <limits>
#include <BWAPI.h>
using namespace std;
using namespace BWAPI;

#define FB_DRAGOON_WEAPON_RANGE WeaponTypes::Phase_Disruptor.maxRange()
#define FB_LOCAL_SPEED 20
#define forUnitSet(one, many) for(set<Unit*>::iterator one = many.begin(); one != many.end(); ++one)
#define forBulletSet(one, many) for(set<Bullet*>::iterator one = many.begin(); one != many.end(); ++one)

class TeaBot : public AIModule {
public:
   virtual void onStart();
   virtual void onEnd(bool isWinner);
   virtual void onFrame();
   virtual void onSendText(string text);
   virtual void onReceiveText(Player* player, string text);
   virtual void onPlayerLeft(Player* player);
   virtual void onNukeDetect(Position target);
   virtual void onUnitDiscover(Unit* unit);
   virtual void onUnitEvade(Unit* unit);
   virtual void onUnitShow(Unit* unit);
   virtual void onUnitHide(Unit* unit);
   virtual void onUnitCreate(Unit* unit);
   virtual void onUnitDestroy(Unit* unit);
   virtual void onUnitMorph(Unit* unit);
   virtual void onUnitRenegade(Unit* unit);
   virtual void onSaveGame(string gameName);

private:
   void attackTarget();
   void drawMarkings();
   void moveToCentre();
   void init();
   void adv();
   void engage();
   bool initDone();
   void findCentre(set<Unit*> units, double &x, double &y);
   void scourgeAttack(Unit *unit);
   void mutaAttack(Unit *unit);
   double distance(Unit *unit, Unit *enemy);
   int totalHp(Unit *unit);
   Unit* aquireTarget(Unit*);

   Position mapCentre;
   map<Unit*, Unit*> targets;
   set<Unit*> scTargets;
   set<Unit*> excluded;
   Position home;
   int state, INIT, ADV, ENGAGE;
   double x1, y1;
   int mapType, DRAGOON, MUTA, ZEALOT;
   //map<Unit*, bool> retreat;
};
