#pragma once
#define NOMINMAX
#include "Group.h"
#include "Enemy.h"
#include <math.h>
#include <windows.h>
#include <limits>
#include <BWAPI.h>
#include <BWTA.h>
using namespace std;
using namespace BWAPI;

#define FB_DRAGOON_WEAPON_RANGE WeaponTypes::Phase_Disruptor.maxRange()
#define FB_LOCAL_SPEED 20

extern bool analyzed;
extern bool analysis_just_finished;
DWORD WINAPI AnalyzeThread();

class CraigacpBot : public AIModule
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
    Unit *tryAquireTarget();

    Position mapCentre;
    Unit *target;
    Group* dudeGroups;
    int group_count;

};
