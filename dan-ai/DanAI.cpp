#include "DanAI.h"
using namespace std;
using namespace BWAPI;

void DanAI::onStart()
{
    Broodwar->printf("DanAI");
	Broodwar->setLocalSpeed(FB_LOCAL_SPEED);
    
    // Find the centre of the map.
    mapCorner = Position(Broodwar->mapWidth() * TILE_SIZE /2,
                         Broodwar->mapHeight() * TILE_SIZE /2);
	//anotherPosition = Position
	
    // We don't have a target until we've scouted one out.
    target = NULL;
	target_2 = NULL;
	target_3 = NULL;


	set<Unit *> units = Broodwar->self()->getUnits();
	int numberOfGroups = 3;
	int size = units.size();
	int index = 0;
	// specifies how many units should be in a single group
	int groupRatio = size / numberOfGroups;
	for(set<Unit *>::iterator unit = units.begin(); unit != units.end(); unit++)
	{
		Unit *nextUnit = *unit;
		if(index<=groupRatio) {
			groupAlpha.insert(*unit);
		}
		else if(index>groupRatio && index <=(groupRatio*2)) {
			groupBravo.insert(*unit);
		}
		else {
			groupDelta.insert(*unit);
		}
		index++;
	}
	for(set<Unit *>::iterator unit = groupAlpha.begin(); unit != groupAlpha.end();unit++)
	{
		Unit *someunit = *unit;
		int id = someunit->getID();
		Broodwar->printf("Alpha: %d",id);
	}
	for(set<Unit *>::iterator unit = groupBravo.begin(); unit != groupBravo.end();unit++)
	{
		Unit *someunit = *unit;
		int id = someunit->getID();
		Broodwar->printf("Bravo: %d",id);
	}
	for(set<Unit *>::iterator unit = groupDelta.begin(); unit != groupDelta.end();unit++)
	{
		Unit *someunit = *unit;
		int id = someunit->getID();
		Broodwar->printf("Delta: %d",id);
	}

	moveToCentre();

}

void DanAI::onFrame()
{
    /* If we haven't got a target or our target has been destroyed,
     * try to aquire a new target. If a new target cannot be found,
     * start scouting again.
     */
    if(target == NULL || !target->exists())
	{
		if(getNextTarget(groupAlpha, target))
		{
			attackTarget(groupAlpha, target);
		}
		else 
		{
			moveToCentre();
		}
	}
	
	 if(target_2 == NULL || !target_2->exists())
	{
		if(getNextTarget(groupBravo,target_2))
		{
			attackTarget(groupBravo, target_2);
		}
		else 
		{
			moveToCentre();
		}
	}
	 if(target_3 == NULL || !target_3->exists())
	{
		if(getNextTarget(groupDelta,target_3))
		{
			attackTarget(groupDelta, target_3);
		}
		else 
		{
			moveToCentre();
		}
	}
	//attackTarget(groupAlpha,groupBravo,groupDelta);
	drawMarkings();
	
}

/**
 * Target the enemy that causes our units to travel the least to
 * attack. If no enemies are visible, do nothing.
 */
/**
* The code in method getNextTarget() was mostly copied from
* FoxxyBot.cpp (Pete's code)
**/
Unit *DanAI::getNextTarget(set<Unit *> reqGroup, Unit *reqTarget)
{
    // get all the enemies
	set<Unit *> emenies = Broodwar->enemy()->getUnits();

	// If we can't see any enemies, there is no closest enemy.
	if (emenies.empty())
	{
		//reqTarget = NULL;
		return NULL;
	}

	// The closest enemy found so far.
	Unit *closestEnemy = *emenies.begin();

    /* The shortest total distance our units will have travel to
     * attack an enemy unit.
     */
    double leastTotalTravelDistance = numeric_limits<double>::max();

	for (set<Unit *>::iterator enemy = emenies.begin(); enemy != emenies.end(); enemy++)
	{
        // The total distance our units will have travel to attack enemy.
		double totalTraveldistance = 0.0;
		
        for (set<Unit *>::iterator unit = reqGroup.begin(); unit != reqGroup.end(); unit++)
		{
            /* The distance our unit has to travel to attack 'enemy'.
             * If less than zero, our unit can attack from its
             * current position.
             */
            double travelDistance = (*unit)->getDistance(*enemy) - FB_DRAGOON_WEAPON_RANGE;

            if (travelDistance > 0)
            {
                totalTraveldistance += travelDistance;
            }
		}

		/* Would attacking 'enemy' cause our units to travel less than all
         * other enemies so far?
         */
		if (totalTraveldistance <= leastTotalTravelDistance)
		{
			closestEnemy = *enemy;
			leastTotalTravelDistance = totalTraveldistance;
		}
	}
	reqTarget = closestEnemy;
    return closestEnemy;
}

/**
*  Gather around our units in groups of four, 
*  then each groupt attacks different targets
**/
void DanAI::attackTarget(set<Unit *> alpha, set<Unit *> bravo, set<Unit *> delta)
{
	
	// the group alpha attacks one target
	for(set<Unit *>::iterator unit = alpha.begin(); unit != alpha.end(); unit++) 
	{ 
		(*unit)->attackUnit(target);
	}
	// the group bravo attacks another target
	for(set<Unit *>::iterator unit = bravo.begin(); unit != bravo.end(); unit++) 
	{ 
		(*unit)->attackUnit(target_2);
	}
	// the group delta attacks third target
	for(set<Unit *>::iterator unit = delta.begin(); unit != delta.end(); unit++) 
	{ 
		(*unit)->attackUnit(target_3);
	}

}

void DanAI::attackTarget(set<Unit *> units, Unit *reqTarget)
{
	
	// the group alpha attacks one target
	for(set<Unit *>::iterator unit = units.begin(); unit != units.end(); unit++) 
	{ 
		(*unit)->attackUnit(reqTarget);
	}
}

/**
* The code in method moveToCentre() was mostly copied from
* FoxxyBot.cpp (Pete's code)
**/
void DanAI::moveToCentre()
{
    // Move all units to the centre of the map.
    set<Unit *> units = Broodwar->self()->getUnits();
	for(set<Unit *>::iterator unit = units.begin(); unit != units.end(); unit++)
	//for(set<Unit *>::iterator unit = groupAlpha.begin(); unit != groupAlpha.end(); unit++)
	{
        (*unit)->move(mapCorner);
	}
}
void DanAI::drawMarkings()
{
    if (target != NULL && target->exists())
    {
        // Mark target on map.
		Unit *bla = *groupBravo.begin();
        Position p1 = bla->getPosition();//make this position of shooter.
        Position p2 = target->getPosition();
		Broodwar->drawLineMap(p1.x() - 2, p1.y() - 2, p2.x() + 2, p2.y() + 2, Colors::Red);
        
		Position p3 = target_2->getPosition();//make this position of shooter.
        Position p4 = target_2->getPosition();
		Broodwar->drawLineMap(p3.x() - 2, p3.y() - 2, p4.x() + 2, p4.y() + 2, Colors::Red);

		Position p5 = target_3->getPosition();//make this position of shooter.
        Position p6 = target_3->getPosition();
		Broodwar->drawLineMap(p5.x() - 2, p5.y() - 2, p6.x() + 2, p6.y() + 2, Colors::Red); 
    }
}

// Callback stubs
void DanAI::onEnd(bool isWinner) {}
void DanAI::onNukeDetect(Position target) {}
void DanAI::onPlayerLeft(Player* player) {}
void DanAI::onReceiveText(Player* player, string text) {}
void DanAI::onSaveGame(string gameName) {}
void DanAI::onSendText(string text) {}
void DanAI::onUnitCreate(Unit* unit) {}
void DanAI::onUnitDestroy(Unit* unit) {}
void DanAI::onUnitDiscover(Unit* discoveredUnit) {}
void DanAI::onUnitEvade(Unit* unit) {}
void DanAI::onUnitHide(Unit* unit) {}
void DanAI::onUnitMorph(Unit* unit) {}
void DanAI::onUnitRenegade(Unit* unit) {}
void DanAI::onUnitShow(Unit* unit) {}
