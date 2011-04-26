#pragma once
#define NOMINMAX
#include <windows.h>
#include <limits>
#include <BWAPI.h>
#include <time.h>
#include <math.h>
using namespace std;
using namespace BWAPI;

#define FB_DRAGOON_WEAPON_RANGE WeaponTypes::Phase_Disruptor.maxRange()
#define FB_MUTA_WEAPON_RANGE WeaponTypes::Glave_Wurm.maxRange()
#define FB_LOCAL_SPEED 20


class GreenBot : public AIModule
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
	void moveToStartPositions();
	void moveToStartPositionsGroup();
    void attackTarget(Unit * unit, Unit* target);
    bool findTarget(int group);
	void danceAway(Unit * unit, Unit * target, int reach);
	void wander(int group);
	bool damaged(Unit * unit);
	void drawMarkings();

};
