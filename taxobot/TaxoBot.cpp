#include "TaxoBot.h"
using namespace std;
using namespace BWAPI;

void TaxoBot::onStart()
{
	Broodwar->printf("TaxoBot");
	Broodwar->setLocalSpeed(FB_LOCAL_SPEED/5);
    // Find the centre of the map.
    mapCentre = Position(Broodwar->mapWidth() * TILE_SIZE / 2,
                         Broodwar->mapHeight() * TILE_SIZE / 2);
    // We don't have a target until we've scouted one out.
    target = NULL;
	
    /* Find the unit type */
	UnitType type = (*Broodwar->self()->getUnits().begin())->getType();
	if (type == UnitTypes::Protoss_Dragoon){
		Broodwar->sendText("We're no strangers to loveeeee");
		currentUnit = DRAGOON;
	}
	else if (type == UnitTypes::Protoss_Zealot){
		currentUnit = ZEALOT;
	}
	else{
		currentUnit = MUTALISK;
		Broodwar->sendText("We're no strangers to loveeeee");
	}
	
	/* Store the initial positions of each unit */
	int i =0;
	set<Unit *> units = Broodwar->self()->getUnits();
	for (set<Unit *>::iterator unit = units.begin(); unit!=units.end();unit++){
		unitStartPositions[i] = (*unit)->getInitialPosition();
		unitStartPositions[i].x() = unitStartPositions[i].x() * TILE_SIZE/(i+1);
		unitStartPositions[i].y() = unitStartPositions[i].y() * TILE_SIZE/(i+1);
		if (!unitStartPositions[i].isValid())
			unitStartPositions[i].makeValid();
		i++;
	}
	
	/* An array of positions for units to move to later on */
	int rand =1;
	for (i = 0;i<MAX_BOTS;i++){
		randomPositions[i] = Position(Broodwar->mapWidth() * TILE_SIZE / rand,
										Broodwar->mapHeight() * TILE_SIZE / rand);
		if (!randomPositions[i].isValid())
			randomPositions[i].makeValid();
		rand++;
	}
	// initialise frame count
	frame = 0;
}

void TaxoBot::onFrame()
{
    /* If we haven't got a target or our target has been destroyed,
     * try to aquire a new target. If a new target cannot be found,
     * start scouting again.
     */
	static const int FPS = Broodwar->getFPS();
	static int count = 1;
	
	set<Unit *> units = Broodwar->self()->getUnits();
	Unit *temp;
	if (!(units.empty())){
		temp = (*units.begin());
		if (target == NULL || !target->exists())
			for (set<Unit *>::iterator unit = units.begin(); unit!=units.end();unit++){
				if ((*unit)->isInWeaponRange(target)){
					if ((*unit)->getGroundWeaponCooldown() == 0)
						(*unit)->attackUnit(target);
					else (*unit)->move(randomPositions[count%MAX_BOTS]);
				}
				else
					if (tryAcquireTarget())
						(*unit)->attackUnit(target);
					else
						if (currentUnit == ZEALOT)
							movePosition(*unit,temp->getPosition().makeValid());
						else if (currentUnit == DRAGOON)
							if (temp->isInWeaponRange(target))
								movePosition(*unit,temp->getPosition().makeValid());
							else movePosition(*unit,randomPositions[count%MAX_BOTS].makeValid());
						else if (currentUnit == MUTALISK){
							static int j = 0;
							movePosition(*unit, randomPositions[j%MAX_BOTS].makeValid());
							j++;
						}
				if ((*unit)->exists())
					temp = (*unit);
			}
	}
			
    count++;
    drawMarkings();
}

/**
 * Target the enemy that causes our units to travel the least to
 * attack. If no enemies are visible, do nothing.
 */
Unit *TaxoBot::tryAcquireTarget()
{
    // All the enemy units we can see.
	set<Unit *> enemies = Broodwar->enemy()->getUnits();

	// If we can't see any enemies, there is no closest enemy.
	if (enemies.empty())
		return NULL;

	// All of our units.
	set<Unit *> units = Broodwar->self()->getUnits();
	
	// The closest enemy found so far.
	Unit *closestEnemy;

    /* The shortest total distance our units will have travel to
     * attack an enemy unit.
     */
    double leastTotalTravelDistance = numeric_limits<double>::max();

	for (set<Unit *>::iterator enemy = enemies.begin(); enemy != enemies.end(); enemy++)
	{
        // The total distance our units will have travel to attack enemy.
		double totalTraveldistance = 0.0;
		int i = 0;
        for (set<Unit *>::iterator unit = units.begin(); unit != units.end(); unit++)
		{
            /* The distance our unit has to travel to attack 'enemy'.
             * If less than zero, our unit can attack from its
             * current position.
             */
			UnitType unit_type = (*unit)->getType();

			double travelDistance;
			if (unit_type == UnitTypes::Zerg_Mutalisk)
				travelDistance = (*unit)->getDistance(*enemy) - FB_MUTALISK_WEAPON_RANGE;
			else if (unit_type == UnitTypes::Zerg_Scourge)
				travelDistance = (*unit)->getDistance(*enemy) - FB_SCOURGE_WEAPON_RANGE;
			else if (unit_type == UnitTypes::Protoss_Dragoon)
				travelDistance = (*unit)->getDistance(*enemy) - FB_DRAGOON_WEAPON_RANGE;
			else if (unit_type == UnitTypes::Protoss_Zealot)
				travelDistance = (*unit)->getDistance(*enemy) - FB_ZEALOT_WEAPON_RANGE;
	        if (travelDistance > 0)
                totalTraveldistance += travelDistance;
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

/* Move a given unit to a specified point */
void TaxoBot::movePosition(Unit *unit, Position p){	unit->move(p);}

void TaxoBot::drawMarkings()
{
    if (target != NULL && target->exists())
    {
        // Mark target on map.
        Position p = target->getPosition();
        Broodwar->drawBoxMap(p.x() - 2, p.y() - 2, p.x() + 2, p.y() + 2, Colors::Red, true);
    }
}
void TaxoBot::onUnitDestroy(Unit* unit) {

	/*	Run to a random position when a unit is destroyed */
	set<Unit *> units = Broodwar->self()->getUnits();
	static int count=0;
	for (set<Unit *>::iterator unitw = units.begin(); unitw != units.end(); unitw++)
		(*unitw)->attackMove(randomPositions[count%MAX_BOTS]);
	count++;
	static int textID = 1;
	static int loop = 0;
	if (checkEnemy(unit)){
		switch (textID) {
			case 1:
				Broodwar->sendText("You know the rules and so do I!"); break;
			case 2:
				Broodwar->sendText("A full commitment's what I'm thinking of"); break;
			case 3:
				Broodwar->sendText("You wouldn't get this from any other guy!");break;
			case 4:
				Broodwar->sendText("I ...just want to tell you how I'm feeling");break;
			case 5:
				Broodwar->sendText("Gotta make you understand");break;
			case 6:
				Broodwar->sendText("NEVER GONNNA GIVE YOU UP");	break;
			case 7:
				Broodwar->sendText("NEVER GONNA LET YOU DOWN");	break;
			case 8:
				Broodwar->sendText("NEVER GONNA TURN AROUND AND...DESERT YOU");	break;
			case 9:
				Broodwar->sendText("NEVER GONNA MAKE YOU CRYYY!");break;
			case 10:
				Broodwar->sendText("NEVER GONNA SAY GOOOODBYEEEE");	break;
			case 11:
				Broodwar->sendText("NEVER GONNA TELL A LIE AND HURT YOU");	break;
			case 12:
				if (currentUnit == DRAGOON || loop == 1){ loop = 0; break;}
				Broodwar->sendText("We've known each other for so long");break;
			case 13:
				Broodwar->sendText("Your heart's been aching but you're too shy to say it.");break;
			case 14:
				Broodwar->sendText("Inside we both know what's been going on,");break;
			case 15:
				Broodwar->sendText("We know the game and we're gonna play it.");break;
			case 16:
				Broodwar->sendText("Annnnnd if you ask me how I'm feeling,");break;
			case 17:
				Broodwar->sendText("Don't tell me you're too blind to see");
				textID=5;
				loop = 1; break;
			default:
				textID=1;	break;
			}
		textID++;
	}

}

void TaxoBot::onEnd(bool isWinner) {
    if (isWinner)
		Broodwar->sendText("I'M THE JUGGERNAUT B17CH");
	else
		Broodwar->sendText("LET ME FINISH MY SONG!");
}

/* Check if a unit is an enemy */
bool TaxoBot::checkEnemy(Unit *unit){
	Player *player = unit->getPlayer();
	return Broodwar->self()->isEnemy(player);
}

void TaxoBot:: onUnitDiscover(Unit *discoveredUnit){
	if (checkEnemy(discoveredUnit))
		target = discoveredUnit;
}

// Callback stubs
void TaxoBot::onNukeDetect(Position target) {}
void TaxoBot::onPlayerLeft(Player* player) {}
void TaxoBot::onReceiveText(Player* player, string text) {}
void TaxoBot::onSaveGame(string gameName) {}
void TaxoBot::onSendText(string text) {}
void TaxoBot::onUnitCreate(Unit* unit) {}
void TaxoBot::onUnitEvade(Unit* unit) {}
void TaxoBot::onUnitHide(Unit* unit) {}
void TaxoBot::onUnitMorph(Unit* unit) {}
void TaxoBot::onUnitRenegade(Unit* unit) {}
void TaxoBot::onUnitShow(Unit* unit) {}