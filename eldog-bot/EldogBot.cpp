#include "EldogBot.h"
using namespace std;
using namespace BWAPI;
map<Unit *, double> enemyDistanceMap;
list<Unit *> enemiesByDistance;

set<Unit *> alphaPack;
set<Unit *> betaPack;
set<Unit *> deltaPack;

void EldogBot::onStart()
{
    Broodwar->printf("EldogBot");
	//Broodwar->setLocalSpeed(FB_LOCAL_SPEED);
	
	mapHeight = Broodwar->mapHeight() * TILE_SIZE;
	mapWidth = Broodwar->mapWidth() * TILE_SIZE;

	target = NULL;
	mostTargetedUnit = NULL;

	UnitType unitType = (*Broodwar->self()->getUnits().begin())->getType();
	// Check what match we're in
	if (unitType == UnitTypes::Protoss_Dragoon)
	{
		// Match 1
		matchType = DRAGOONS;
	}
	else if (unitType == UnitTypes::Protoss_Zealot)
	{
		// Match 2
		matchType = ZEALOTS;
	}
	else if (unitType == UnitTypes::Zerg_Mutalisk || unitType == UnitTypes::Zerg_Scourge)
	{
		// Match 3
		matchType = MUTALISK_SCOURGE;
	}
	
	// Analyze our map
	BWTA::readMap();
	BWTA::analyze();
	
	set<Unit *> units = Broodwar->self()->getUnits();

	region = BWTA::getRegion((*units.begin())->getTilePosition());
	
	int i = 0;
	for (set<Unit *>::iterator unit = units.begin(); unit != units.end(); unit ++)
	{
		switch (i++ % 3 )
		{
			case 0:
				alphaPack.insert(*unit);
				break;
			case 1:
				betaPack.insert(*unit);
				break;
			case 2:
				deltaPack.insert(*unit);
				break;
		}
	}
}
void EldogBot::onFrame()
{
    if (!Broodwar->enemy()->getUnits().empty())
    {
        attack(alphaPack);
        attack(betaPack);
        attack(deltaPack);
    }
    else
    {
        // Old random way
        //scout(alphaPack);
        //scout(betaPack);
        //scout(deltaPack);
        UNIT_ITER(alphaPack, unit)
	    {
            (*unit)->move(Position(1000,1200));
        }
        UNIT_ITER(betaPack, unit)
	    {
            (*unit)->move(Position(1000,900));
        }
        UNIT_ITER(deltaPack, unit)
	    {
            (*unit)->move(Position(1000,1500));
        }
    }
    
    drawData();
}


void EldogBot::attack(set<Unit *> units)
{
	enemiesByDistance.clear();
	enemyDistanceMap.clear();
	set<Unit *> enemies = Broodwar->enemy()->getUnits();
	UNIT_ITER(enemies, enemy)
	{
		double totalTraveldistance = 0.0;
        UNIT_ITER(units, unit)
		{
			double travel = (*unit)->getDistance(*enemy) - FB_DRAGOON_WEAPON_RANGE;
			if (travel > 0)
			{
				totalTraveldistance += travel;
			}
		}
        double averageDistance = totalTraveldistance / units.size();
		enemyDistanceMap[*enemy] = averageDistance;
		enemiesByDistance.insert(enemiesByDistance.begin(), *enemy);
	}
	enemiesByDistance.sort(compEnemies);

	list<Unit *>::const_iterator t = enemiesByDistance.begin();
	UNIT_ITER(units, unit)
	{
		// To retarget or not
		if ((*unit)->getOrderTarget() != NULL && (*unit)->isInWeaponRange((*unit)->getOrderTarget()))
		{
			continue;
		}
		(*unit)->attackUnit(*t);
	}
}


bool compEnemies(Unit *enemy1, Unit *enemy2)
{
	return enemyDistanceMap[enemy1] < enemyDistanceMap[enemy2];
}

void EldogBot::scout(set<Unit *> units)
{
	currentTask = SCOUTING;

	UNIT_ITER(units, unit)
	{
		if ((*unit)->getOrder() != Orders::Move)
		{
			scoutPosition = getRandomPosition();

			UNIT_ITER(units, unit)
			{
				(*unit)->move(scoutPosition);
			}
			return;
		}
	}
}

Position EldogBot::getRandomPosition()
{
	while (true)
	{
		int randX = rand() % mapWidth;
		int randY = rand() % mapHeight;

		Position randPosition = Position(randX, randY);
		if (region->getPolygon().isInside(randPosition))
		{
			return randPosition;
		}
	}
}

void EldogBot::onEnd(bool isWinner) {}
void EldogBot::onNukeDetect(Position target) {}
void EldogBot::onPlayerLeft(Player* player) {}
void EldogBot::onReceiveText(Player* player, string text) {}
void EldogBot::onSaveGame(string gameName) {}
void EldogBot::onSendText(string text) {}
void EldogBot::onUnitCreate(Unit* unit) {}
void EldogBot::onUnitDestroy(Unit* unit) {}
void EldogBot::onUnitDiscover(Unit* discoveredUnit) {}
void EldogBot::onUnitEvade(Unit* unit) {}
void EldogBot::onUnitHide(Unit* unit) {}
void EldogBot::onUnitMorph(Unit* unit) {}
void EldogBot::onUnitRenegade(Unit* unit) {}
void EldogBot::onUnitShow(Unit* unit) {}

void EldogBot::drawData()
{
	BWTA::Polygon regionPolygon = region->getPolygon();
	for (unsigned int vertexIndex = 0; vertexIndex < regionPolygon.size(); vertexIndex ++)
	{
		Position lineStart = regionPolygon[vertexIndex];
		Position lineEnd = regionPolygon[(vertexIndex + 1) % regionPolygon.size()];
		Broodwar->drawLineMap(lineStart.x(), lineStart.y(), lineEnd.x(), lineEnd.y(), Colors::Green);
	}

	if (currentTask == SCOUTING)
	{
		int x = scoutPosition.x();
		int y = scoutPosition.y();
		Broodwar->drawBoxMap(x - 2, y - 2, x + 2, y + 2, Colors::Blue, true);
	}

	set<Unit *> units = Broodwar->self()->getUnits();
	UNIT_ITER(units, unit)
	{
		Unit *target = (*unit)->getOrderTarget();
		if (target == NULL)
		{
			continue;
		}
        
		Position targetPosition = target->getPosition();
		int x1 = targetPosition.x();
		int y1 = targetPosition.y();
		Position unitPosition = (*unit)->getPosition();
		int x2 = unitPosition.x();
		int y2 = unitPosition.y();
		Broodwar->drawLineMap(x1, y1, x2, y2, Colors::Red);
	} 

	int index = 1;
    for (list<Unit *>::iterator enemy = enemiesByDistance.begin(); enemy != enemiesByDistance.end(); enemy ++)
	{
		Position unitPosition = (*enemy)->getPosition();
		int x = unitPosition.x();
		int y = unitPosition.y();
		Broodwar->drawTextMap(x, y, "%d", index++);
	}

	UNIT_ITER(alphaPack, alpha)
	{
		Position alphaPosition = (*alpha)->getPosition();
		Broodwar->drawCircleMap(alphaPosition.x(), alphaPosition.y(), 16, Colors::Cyan);
        Position targetPosition =(*alpha)->getTargetPosition();
        Broodwar->drawCircleMap(targetPosition.x(), targetPosition.y(), 4, Colors::Cyan, true);
        Broodwar->drawTextMap(targetPosition.x(), targetPosition.y(), "%d, %d", targetPosition.x(), targetPosition.y());
	}

	UNIT_ITER(betaPack, beta)
	{
		Position betaPosition = (*beta)->getPosition();
		Broodwar->drawCircleMap(betaPosition.x(), betaPosition.y(), 16, Colors::Purple);
        Position targetPosition =(*beta)->getTargetPosition();
        Broodwar->drawCircleMap(targetPosition.x(), targetPosition.y(), 4, Colors::Purple, true);
        Broodwar->drawTextMap(targetPosition.x(), targetPosition.y(), "%d, %d", targetPosition.x(), targetPosition.y());
	}

	UNIT_ITER(deltaPack, delta)
	{
		Position deltaPosition = (*delta)->getPosition();
		Broodwar->drawCircleMap(deltaPosition.x(), deltaPosition.y(), 16, Colors::Orange);
        Position targetPosition =(*delta)->getTargetPosition();
        Broodwar->drawCircleMap(targetPosition.x(), targetPosition.y(), 4, Colors::Orange, true);
        Broodwar->drawTextMap(targetPosition.x(), targetPosition.y(), "%d, %d", targetPosition.x(), targetPosition.y());
	}

}
