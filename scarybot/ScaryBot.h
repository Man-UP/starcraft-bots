#pragma once
#define NOMINMAX
#include <windows.h>
#include <limits>
#include <BWAPI.h>
using namespace std;
using namespace BWAPI;

#define FB_DRAGOON_WEAPON_RANGE WeaponTypes::Phase_Disruptor.maxRange()
#define FB_LOCAL_SPEED 20


class ScaryBot : public AIModule
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
    void moveToCentre(set<Unit *> units);
	void moveToPosition(set<Unit *> units, Position pos);
	void formSquads();
    Unit *tryAquireTarget(set<Unit *> units);
	bool locationCheck(set<Unit *> units, Position pos);
	//void flee(Unit unit, Position pos);
	void checkUnitStatus();
	void makeFormation();
	Position getAverageEnemyPosition();
	Position getDistanceFromUnit(Position enemyPos, Position unitPos);
	void ScaryBot::attackTargetZealot();
	Unit *ScaryBot::tryAquireTargetZealot(set<Unit *> units);
	//void flee();
	//bool isUnderAttack(Unit *unit);

    Position mapCentre;
	Position topMapCentre;
	Position bottomMapCentre;
    Unit *target1;
	Unit *target2;
	Unit *target3;
	Unit *target4;
	enum Round { DRAGOONS, ZEALOTS, MUTAS };
	Round currentRound;
	set<Unit *> squadOne;
	set<Unit *> squadTwo;
	set<Unit *> squadThree;
	set<Unit *> squadFour;
	bool atDestination;
	bool atDestination1;
	bool atDestination2;
	Position start;
	bool notFleeing [12];
	int healthShields [12][2];
	Position halfwayCentre;
	Position pointTwo;
	Position referencePos1;
	Position referencePos2;
	int halfWidth;
	int halfHeight;
	int refHeight1;
	int refHeight2;
	set<Position *> enemyPosition;
	int positionModifier;
};
