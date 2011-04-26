#include "ScaryBot.h"
using namespace std;
using namespace BWAPI;

void ScaryBot::onStart()
{

    Broodwar->printf("ScaryBot");
	Broodwar->setLocalSpeed(FB_LOCAL_SPEED);

	// Find the round type. Assign to the enum type in the header file
	if ((*Broodwar->self()->getUnits().begin())->getType() == UnitTypes::Protoss_Dragoon)
	{
		Broodwar->printf("Round is Dragoons");
		currentRound = DRAGOONS;
	} else if((*Broodwar->self()->getUnits().begin())->getType() == UnitTypes::Protoss_Zealot){
		Broodwar->printf("Round is Zealot");
		currentRound = ZEALOTS;
	} else if((*Broodwar->self()->getUnits().begin())->getType() == UnitTypes::Zerg_Mutalisk){
		Broodwar->printf("Round is Mutalisk & Scourge");
		currentRound = MUTAS;
	} else if((*Broodwar->self()->getUnits().begin())->getType() == UnitTypes::Zerg_Scourge){
		Broodwar->printf("Round is Mutalisk & Scourge");
		currentRound = MUTAS;
	} else {
		Broodwar->printf("Unit not recognised");
	}
    


	// Form Squads
	formSquads();

	int count = 0;
	set<Unit *> units = Broodwar->self()->getUnits();
	for(set<Unit *>::iterator unit = units.begin(); unit != units.end(); unit++)
	{
		int unitID = (*unit)->getID();
		healthShields[unitID][0] = (*unit)->getHitPoints();
		healthShields[unitID][1] = (*unit)->getShields();
	}


    // Find the centre of the map.
    mapCentre = Position(Broodwar->mapWidth() * TILE_SIZE / 2,
                         Broodwar->mapHeight() * TILE_SIZE / 2);

	int deltaX = mapCentre.x() - (*Broodwar->self()->getUnits().begin())->getPosition().x();

	positionModifier = 0;

	if (deltaX <0)
	{
		positionModifier = 7;
	}
	else if (deltaX > 0)
	{
		positionModifier = -7;
	}
	else 
	{
		Broodwar->printf("Something has gone wrong");
	}


	topMapCentre = Position((Broodwar->mapWidth() * TILE_SIZE) * 0.35, (Broodwar->mapHeight() * TILE_SIZE)*0.65);
	bottomMapCentre = Position((Broodwar->mapWidth() * TILE_SIZE * 0.35), (Broodwar->mapHeight() * TILE_SIZE) * 0.35);

	halfWidth = (Broodwar->mapWidth() * TILE_SIZE) / 2;
	halfHeight = (Broodwar->mapWidth() * TILE_SIZE) / 2;

	halfwayCentre = Position(halfWidth /2,halfHeight);

	pointTwo = Position(halfWidth/2,
		                halfHeight + (TILE_SIZE * 5));

	refHeight1 = halfHeight + TILE_SIZE * 11;
	
	referencePos1 = Position((halfWidth + TILE_SIZE*positionModifier ), refHeight1);

	    //set<Unit *> units = Broodwar->self()->getUnits();


    // We don't have a target until we've scouted one out.
    target1 = NULL;
	target2 = NULL;
	target3 = NULL;
	target4 = NULL;

    // Start scouting.
	//moveToPosition(squadOne, topMapCentre);
	//moveToPosition(squadTwo, bottomMapCentre);
	//atDestination = false;

	//moveToPosition(squadOne, halfwayCentre);
	//moveToPosition(unit1);
	//moveToPosition(squadTwo, pointTwo);



	makeFormation();
}

void ScaryBot::onFrame()
{
	/*
	if (currentRound == ZEALOTS) {
		
	} else if (currentRound == DRAGOONS) {
		
	} else if (currentRound == MUTAS) {
		
	} else {
		Broodwar->printf("Round not determined");
	}
	
     If we haven't got a target or our target has been destroyed,
     * try to aquire a new target. If a new target cannot be found,
     * start scouting again.
    */
	/*
	if (!atDestination1)
	{
		atDestination1 = locationCheck(squadOne, topMapCentre);
	}
	
	if (!atDestination2)
	{
		atDestination2 = locationCheck(squadOne, topMapCentre);
	}
	*/
	if (currentRound != ZEALOTS)
	{
		checkUnitStatus();
	}

	if (target1 == NULL || !target1->exists())
    {
		if(currentRound == DRAGOONS)     
			target1 = tryAquireTarget(squadOne);
		else if(currentRound == ZEALOTS)
			target1 = tryAquireTargetZealot(squadOne);

				

		if (target1 != NULL)
        {
			if(currentRound == DRAGOONS)
				attackTarget();
			else if(currentRound == ZEALOTS)
				attackTargetZealot();
		}
        else if (atDestination)
        {
            //moveToCentre(squadOne);
        }
    }

	if (target2 == NULL || !target2->exists())
    {
        if(currentRound == DRAGOONS)
			target2 = tryAquireTarget(squadTwo);
		else if(currentRound == ZEALOTS)
			target2 = tryAquireTargetZealot(squadTwo);


		if (target2 != NULL)
        {
			if(currentRound == DRAGOONS)
				attackTarget();
			else if(currentRound == ZEALOTS)
				attackTargetZealot();

        }
        else if (atDestination)
        {
            //moveToCentre(squadOne);
        }
    }

	if (target3 == NULL || !target3->exists())
    {	
		if(currentRound == ZEALOTS)
			target3 = tryAquireTargetZealot(squadThree);
		if (target3 != NULL)
        {
            attackTargetZealot();
        }
        else if (atDestination)
        {
            //moveToCentre(squadOne);
        }
    }

	if (target4 == NULL || !target4->exists())
    {
		set<Unit *> units = Broodwar->self()->getUnits();
		for (set<Unit *>::iterator unit = units.begin(); unit != units.end(); unit++)
			{
			target4 = tryAquireTargetZealot(units);
			if (target4 != NULL)
			{
				attackTargetZealot();
			}
			else if (atDestination)
			{
				//moveToCentre(squadOne);
			}
		}
    }

    drawMarkings();
}

/**
 * Target the enemy that causes our units to travel the least to
 * attack. If no enemies are visible, do nothing.
 */
Unit *ScaryBot::tryAquireTarget(set<Unit *> units) 
{
    // All the enemy units we can see.
	set<Unit *> emenies = Broodwar->enemy()->getUnits();

	// If we can't see any enemies, there is no closest enemy.
	if (emenies.empty())
	{
		return NULL;
	}

	// The closest enemy found so far.
	Unit *closestEnemy = NULL;
	
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
			if ((*enemy)->exists())
			{
				if ((*unit)->exists() && target1 != NULL)
				{
					//if ((*enemy)->getID() != target1->getID() && (*enemy)->getID() != target2->getID() && (*enemy)->getID() != target3->getID() && (*enemy)->getID() != target4->getID())
					//{
						double travelDistance = (*unit)->getDistance(*enemy) - FB_DRAGOON_WEAPON_RANGE;
						if (travelDistance > 0)
						{
							totalTraveldistance += travelDistance;
						}
		
					//}
				}
			}
		}
		//Broodwar->printf("target found %d", &totalTraveldistance);
		/* Would attacking 'enemy' cause our units to travel less than all
         * other enemies so far?
         */
		if (totalTraveldistance <= leastTotalTravelDistance)
		{
			closestEnemy = *enemy;
			leastTotalTravelDistance = totalTraveldistance;
		}
	}

	return closestEnemy;
}

void ScaryBot::attackTarget()
{
	// All units attack the target.
    // set<Unit *> units = Broodwar->self()->getUnits();

	for(set<Unit *>::iterator unit = squadOne.begin(); unit != squadOne.end(); unit++)
	{
        (*unit)->attackUnit(target1);
	}
	for(set<Unit *>::iterator unit = squadTwo.begin(); unit != squadTwo.end(); unit++)
	{
        (*unit)->attackUnit(target2);
	}
	if (currentRound == ZEALOTS)
	{
		for(set<Unit *>::iterator unit = squadThree.begin(); unit != squadThree.end(); unit++)
		{
			(*unit)->attackUnit(target3);
		}
		for(set<Unit *>::iterator unit = squadFour.begin(); unit != squadFour.end(); unit++)
		{
			(*unit)->attackUnit(target4);
		}
	}
}


void ScaryBot::formSquads()
{
	// A counter
	size_t count = 0;

	// All units attack the target.
    set<Unit *> units = Broodwar->self()->getUnits();
	for(set<Unit *>::iterator unit = units.begin(); unit != units.end(); unit++)
	{
		if (currentRound == ZEALOTS)
		{
			if (count <4)
			{
				squadOne.insert(*unit);
				count++;
			
				/*int allSquad =0;
				for(set<Unit *>::iterator unit = units.begin(); unit != units.end(); unit++)
				squadFour.insert(*unit);
				*/

			}
			else if (3 < count && count < 9)
			{
				squadTwo.insert(*unit);
				count++;
			}
			else if (4 < count && count < 7)
			{
				squadThree.insert(*unit);
				count++;
			}
			else if (6 < count && count < 9)
			{
				squadFour.insert(*unit);
				count++;
			}
		}
		else if (currentRound == DRAGOONS)
		{
			if (count <  6)
			{
				squadOne.insert(*unit);
				count++;
			} else if (5 < count && count < 12 )
			{
				squadTwo.insert(*unit);
				count++;
			}
			else
			{
			}
		}
		else if (currentRound == MUTAS)
		{
			squadFour.insert(*unit);
			count++;
			currentRound = ZEALOTS;
		}
	}
}

void ScaryBot::moveToCentre(set<Unit *> units)
{
    // Move all units to the centre of the map.
	for(set<Unit *>::iterator unit = units.begin(); unit != units.end(); unit++)
	{
        (*unit)->move(mapCentre);
	}
}


void ScaryBot::makeFormation()
{
	set<Unit *> units = Broodwar->self()->getUnits();
	int tileCounter = 1;
	// Sort the units by distance to the target

	for(set<Unit *>::iterator unit = units.begin(); unit!=units.end(); unit++)
	{
		set<Unit *> individual;
		individual.insert(*unit);
		moveToPosition(individual, referencePos1);

		referencePos1 = Position((halfWidth + TILE_SIZE*positionModifier ), refHeight1 - (TILE_SIZE*tileCounter*1.2));
		tileCounter++;
	}
	
}

void ScaryBot::moveToPosition(set<Unit *> units, Position pos)
{
	//move a set of units to a given position
	for(set<Unit *>::iterator unit = units.begin(); unit != units.end(); unit++)
	{
        (*unit)->move(pos);
	}
}

bool ScaryBot::locationCheck(set<Unit *> units, Position pos)
{
	//move a set of units to a given position
	for(set<Unit *>::iterator unit = units.begin(); unit != units.end(); unit++)
	{
        if ((*unit)->getDistance(pos) < 3000)
			return true;
	}
}


void ScaryBot::drawMarkings()
{
    if (target1 != NULL && target1->exists())
    {
        // Mark target on map.
		for(set<Unit *>::iterator unit = squadOne.begin(); unit != squadOne.end(); unit++)
		{
			Broodwar->drawLineMap((*unit)->getPosition().x(), (*unit)->getPosition().y(), (*unit)->getTargetPosition().x(), (*unit)->getTargetPosition().y(), Colors::Green);
		}
	}
    if (target2 != NULL && target2->exists())
    {
		for(set<Unit *>::iterator unit = squadTwo.begin(); unit != squadTwo.end(); unit++)
		{
			Broodwar->drawLineMap((*unit)->getPosition().x(), (*unit)->getPosition().y(), (*unit)->getTargetPosition().x(), (*unit)->getTargetPosition().y(), Colors::Green);
		}
	}
	if (target3 != NULL && target3->exists())
    {
		for(set<Unit *>::iterator unit = squadThree.begin(); unit != squadThree.end(); unit++)
		{
			Broodwar->drawLineMap((*unit)->getPosition().x(), (*unit)->getPosition().y(), (*unit)->getTargetPosition().x(), (*unit)->getTargetPosition().y(), Colors::Green);
		}
	}
	if (target4 != NULL && target4->exists())
    {
		for(set<Unit *>::iterator unit = squadFour.begin(); unit != squadFour.end(); unit++)
		{
			Broodwar->drawLineMap((*unit)->getPosition().x(), (*unit)->getPosition().y(), (*unit)->getTargetPosition().x(), (*unit)->getTargetPosition().y(), Colors::Green);
		}
	}
        // Position p = target1->getPosition();
        // Broodwar->drawBoxMap(p.x() - 2, p.y() - 2, p.x() + 2, p.y() + 2, Colors::Red, true);
}

Position ScaryBot::getAverageEnemyPosition()
{
	// All the enemy units we can see.
	set<Unit *> emenies = Broodwar->enemy()->getUnits();
    double averageX = 0;
	double averageY = 0;
	int count = 0;


	for (set<Unit *>::iterator enemy = emenies.begin(); enemy != emenies.end(); enemy++)
	{
  		int thisUnitX = (*enemy)->getPosition().x();
		int thisUnitY = (*enemy)->getPosition().y();
		averageX += thisUnitX;
		averageY += thisUnitY;
		count ++;
	}
    
	averageX /= count;
	averageY /= count;

	Position averageEnemyPosition = Position(averageX, averageY);

	return averageEnemyPosition;
}





Position ScaryBot::getDistanceFromUnit(Position enemyPos, Position unitPos)
{
	int xDifference = (enemyPos.x() - unitPos.x());
	int yDifference = (enemyPos.y() - unitPos.y());
		
	return Position(xDifference, yDifference);
}

void ScaryBot::checkUnitStatus()
{
	Position start = Position((Broodwar->mapWidth() * TILE_SIZE * 0.3), (Broodwar->mapHeight() * TILE_SIZE) * 0.5);
	set<Unit *> units = Broodwar->self()->getUnits();
	for(set<Unit *>::iterator unit = units.begin(); unit != units.end(); unit++)
	{
		int unitID = (*unit)->getID();
		if (((*unit)->getHitPoints()+ 40 ) < healthShields[unitID][0] || ((*unit)->getShields() +30 ) < healthShields[unitID][1])
		{
			int x = (*unit)->getPosition().x();
			int y = (*unit)->getPosition().y();
			
			Position enemyPosition = getAverageEnemyPosition();

			Position deltaPos = getDistanceFromUnit(enemyPosition, ((*unit)->getPosition()));
			
			double distance = sqrt(deltaPos.x() *deltaPos.x()  + (double)deltaPos.y()*deltaPos.y());
			
			double scale = 1 / (distance / 100);
			
			int deltaX = scale * deltaPos.x();
			int deltaY = scale * deltaPos.y();



			Position pos = Position(x - deltaX, y - deltaY); // Current Retreat @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ :) Tom was here
			(*unit)->move(pos);
			healthShields[unitID][0] = (*unit)->getHitPoints();
			healthShields[unitID][1] = (*unit)->getShields();
		}

	}
}

Unit *ScaryBot::tryAquireTargetZealot(set<Unit *> units) 
{
    // All the enemy units we can see.
	set<Unit *> emenies = Broodwar->enemy()->getUnits();

	// If we can't see any enemies, there is no closest enemy.
	if (emenies.empty())
	{
		return NULL;
	}

	// The closest enemy found so far.
	Unit *closestEnemy = NULL;
	
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
			if ((*enemy)->exists())
			{
				if ((*unit)->exists() && target1 != NULL)
				{
					//if ((*enemy)->getID() != target1->getID() && (*enemy)->getID() != target2->getID() && (*enemy)->getID() != target3->getID() && (*enemy)->getID() != target4->getID())
					//{
						double travelDistance = (*unit)->getDistance(*enemy);
						if (travelDistance > 0)
						{
							totalTraveldistance += travelDistance;
						}
		
					//}
				}
			}
		}
		//Broodwar->printf("target found %d", &totalTraveldistance);
		/* Would attacking 'enemy' cause our units to travel less than all
         * other enemies so far?
         */
		if (totalTraveldistance <= leastTotalTravelDistance)
		{
			closestEnemy = *enemy;
			leastTotalTravelDistance = totalTraveldistance;
		}
	}

	return closestEnemy;
}// aquire target for zealot


void ScaryBot::attackTargetZealot()
{
	// All units attack the target.
    // set<Unit *> units = Broodwar->self()->getUnits();

	for(set<Unit *>::iterator unit = squadOne.begin(); unit != squadOne.end(); unit++)
	{
        (*unit)->attackUnit(target1);
	}
	for(set<Unit *>::iterator unit = squadTwo.begin(); unit != squadTwo.end(); unit++)
	{
        (*unit)->attackUnit(target2);
	}
	if (currentRound == ZEALOTS)
	{
		for(set<Unit *>::iterator unit = squadThree.begin(); unit != squadThree.end(); unit++)
		{
			(*unit)->attackUnit(target3);
		}
		for(set<Unit *>::iterator unit = squadFour.begin(); unit != squadFour.end(); unit++)
		{
			(*unit)->attackUnit(target4);
		}
	}
}// attack target for zealots


// Callback stubs
void ScaryBot::onEnd(bool isWinner) {}
void ScaryBot::onNukeDetect(Position target1) {}
void ScaryBot::onPlayerLeft(Player* player) {}
void ScaryBot::onReceiveText(Player* player, string text) {}
void ScaryBot::onSaveGame(string gameName) {}
void ScaryBot::onSendText(string text) {}
void ScaryBot::onUnitCreate(Unit* unit) {}
void ScaryBot::onUnitDestroy(Unit* unit) {}
void ScaryBot::onUnitDiscover(Unit* discoveredUnit) {}
void ScaryBot::onUnitEvade(Unit* unit) {}
void ScaryBot::onUnitHide(Unit* unit) {}
void ScaryBot::onUnitMorph(Unit* unit) {}
void ScaryBot::onUnitRenegade(Unit* unit) {}
void ScaryBot::onUnitShow(Unit* unit) {}
