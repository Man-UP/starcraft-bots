#include "FoxxyBot.h"
using namespace std;
using namespace BWAPI;

void FoxxyBot::onStart()
{
    Broodwar->printf("FoxxyBot");
	Broodwar->setLocalSpeed(FB_LOCAL_SPEED);
    
    // Find the centre of the map.
    mapCentre = Position(Broodwar->mapWidth() * TILE_SIZE / 2,
                         Broodwar->mapHeight() * TILE_SIZE / 2);
	
    // We don't have a target until we've scouted one out.
    target = NULL;

    // Start scouting.
    moveToCentre();
}

void FoxxyBot::onFrame()
{
    /* If we haven't got a target or our target has been destroyed,
     * try to aquire a new target. If a new target cannot be found,
     * start scouting again.
     */
    if (target == NULL || !target->exists())
    {
        if (tryAquireTarget())
        {
            attackTarget();
        }
        else
        {
            moveToCentre();
        }
    }
    
    drawMarkings();
}

/**
 * Target the enemy that causes our units to travel the least to
 * attack. If no enemies are visible, do nothing.
 */
Unit *FoxxyBot::tryAquireTarget()
{
    // All the enemy units we can see.
	set<Unit *> emenies = Broodwar->enemy()->getUnits();

	// If we can't see any enemies, there is no closest enemy.
	if (emenies.empty())
	{
		return NULL;
	}

	// All of our units.
	set<Unit *> units = Broodwar->self()->getUnits();

	// The closest enemy found so far.
	Unit *closestEnemy;

    /* The shortest total distance our units will have travel to
     * attack an enemy unit.
     */
    double leastTotalTravelDistance = numeric_limits<double>::max();

	for (set<Unit *>::iterator enemy = emenies.begin(); enemy != emenies.end(); enemy++)
	{
        // The total distance our units will have travel to attack enemy.
		double totalTraveldistance = 0.0;
		
        for (set<Unit *>::iterator unit = units.begin(); unit != units.end(); unit++)
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

    return target = closestEnemy;
}

void FoxxyBot::attackTarget()
{
	// All units attack the target.
    set<Unit *> units = Broodwar->self()->getUnits();
	for (set<Unit *>::iterator unit = units.begin(); unit != units.end(); unit++)
	{
        (*unit)->attackUnit(target);
	}
}

void FoxxyBot::moveToCentre()
{
    // Move all units to the centre of the map.
    set<Unit *> units = Broodwar->self()->getUnits();
	for (set<Unit *>::iterator unit = units.begin(); unit != units.end(); unit++)
	{
        (*unit)->move(mapCentre);
	}
}

void FoxxyBot::drawMarkings()
{
    if (target != NULL && target->exists())
    {
        // Mark target on map.
        Position p = target->getPosition();
        Broodwar->drawBoxMap(p.x() - 2, p.y() - 2, p.x() + 2, p.y() + 2, Colors::Red, true);
    }
}

// Callback stubs
void FoxxyBot::onEnd(bool isWinner) {}
void FoxxyBot::onNukeDetect(Position target) {}
void FoxxyBot::onPlayerLeft(Player* player) {}
void FoxxyBot::onReceiveText(Player* player, string text) {}
void FoxxyBot::onSaveGame(string gameName) {}
void FoxxyBot::onSendText(string text) {}
void FoxxyBot::onUnitCreate(Unit* unit) {}
void FoxxyBot::onUnitDestroy(Unit* unit) {}
void FoxxyBot::onUnitDiscover(Unit* discoveredUnit) {}
void FoxxyBot::onUnitEvade(Unit* unit) {}
void FoxxyBot::onUnitHide(Unit* unit) {}
void FoxxyBot::onUnitMorph(Unit* unit) {}
void FoxxyBot::onUnitRenegade(Unit* unit) {}
void FoxxyBot::onUnitShow(Unit* unit) {}
