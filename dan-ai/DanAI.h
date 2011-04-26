#pragma once
#define NOMINMAX
#include <windows.h>
#include <limits>
#include <BWAPI.h>
using namespace std;
using namespace BWAPI;

#define FB_DRAGOON_WEAPON_RANGE WeaponTypes::Phase_Disruptor.maxRange()
#define FB_LOCAL_SPEED 20


class DanAI : public AIModule
{
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
    Unit *getNetTarget();
	Unit *getNextTarget(set<Unit *> reqGroup, Unit *reqTarget);
	void attackTarget(set<Unit *> alpha, set<Unit *> bravo, set<Unit *> delta);
	void attackTarget(set<Unit *> units, Unit *reqTarget);
	void DrawMarkings();

    Position mapCorner;
	Position anotherPosition;
    Unit *target;
	Unit *target_2;
	Unit *target_3;
	set<Unit *> groupAlpha;
	set<Unit *> groupBravo;
	set<Unit *> groupDelta;
};
