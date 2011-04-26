#pragma once
#define NOMINMAX
#include <windows.h>
#include <limits>
#include <BWAPI.h>
#include <BWTA.h>
using namespace std;
using namespace BWAPI;

#define FB_DRAGOON_WEAPON_RANGE WeaponTypes::Phase_Disruptor.maxRange()
#define FB_LOCAL_SPEED 20
#define UNIT_ITER(units, unit) for (set<Unit *>::iterator unit = units.begin(); unit != units.end(); unit++)

bool compEnemies(Unit *enemy1, Unit *enemy2);
extern map<Unit *, double> enemyDistanceMap;
extern list<Unit *> enemiesByDistance;

class EldogBot : public AIModule
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
	void drawData();
	void setScoutPosition();
	void attack(set<Unit *>);
	void scout(set<Unit *>);
	Position getRandomPosition();

	int mapHeight;
	int mapWidth;
	BWTA::Region *region;
	Unit *target;
	Position scoutPosition;
	enum MatchType { DRAGOONS, ZEALOTS, MUTALISK_SCOURGE };
	enum Task { ATTACKING, SCOUTING };
	Task currentTask;
	MatchType matchType;

	Unit *mostTargetedUnit;
    Unit *tryAquireTarget();
};
