#pragma once
#define NOMINMAX
#include <windows.h>
#include <limits>
#include <BWAPI.h>
using namespace std;
using namespace BWAPI;

#define FB_DRAGOON_WEAPON_RANGE WeaponTypes::Phase_Disruptor.maxRange()
#define FB_ZEALOT_WEAPON_RANGE WeaponTypes::Psi_Blades.maxRange()
#define FB_SCOURGE_WEAPON_RANGE WeaponTypes::Suicide_Scourge.maxRange()
#define FB_MUTALISK_WEAPON_RANGE WeaponTypes::Glave_Wurm.maxRange()
#define FB_LOCAL_SPEED 20
#define MAX_BOTS 24

class TaxoBot : public AIModule
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
    Unit *tryAcquireTarget();
	bool checkEnemy(Unit *unit);
	void movePosition(Unit *unit,Position p);

    Position mapCentre;
    Unit *target;
	enum unitType {ZEALOT, DRAGOON, MUTALISK, SCOURGE};
	int frame;
	Position unitStartPositions[MAX_BOTS],randomPositions[MAX_BOTS];
	unitType currentUnit;
};
