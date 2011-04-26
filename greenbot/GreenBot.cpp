#include "GreenBot.h"
using namespace std;
using namespace BWAPI;

map<int,Unit*> targets;
map<int,set<Unit*>> groups;
map<Unit *, bool> isAttacking;
map<Unit *,int> record;
map<int,Position> lastWander;
bool GameOn = false;
vector<Position> positions;
int WanderStep = 500;

// gameType
// 0 = dragoons
// 1 = zealots
// 2 = mutalisks
int gameType = 0;
int stackCountDown = 50;

bool compare_units (Unit* first, Unit* second)
{
	Position p1 = (*first).getPosition();
	Position p2 = (*second).getPosition();
  //return ((p1.y()<p2.y())||(p1.x()<p2.x()));
	return (p1.x() > p2.x());
}

Position closestIntersectionPoint(Position p1, Position p2,Position me,int reach){
	Broodwar->printf("Calculate Intersects");
	double x0 = (double) p1.x();
	double y0 = (double) p1.y();
	double x1 = (double) p2.x();
	double y1 = (double) p2.y();
	double range;
	if(gameType ==0){ range = FB_DRAGOON_WEAPON_RANGE;}
	else{ range = FB_MUTA_WEAPON_RANGE;}
	double r0 = range+reach;
	double r1 = range+20+reach;
	double e = x1 -x0;
	double f = y1 - y0;
	double p = sqrt((e*e)+(f*f));
	double k = ((p*p)+(r0*r0)-(r1*r1))/(2*p);

	double x3a = x0 + ((e*k)/p) + ((f/p)*sqrt((r0*r0)-(k*k)));
	double y3a = y0 + ((f*k)/p) - ((e/p)*sqrt((r0*r0)-(k*k)));
	double x3b = x0 + ((e*k)/p) - ((f/p)*sqrt((r0*r0)-(k*k)));
	double y3b = y0 + ((f*k)/p) + ((e/p)*sqrt((r0*r0)-(k*k)));
	
	Position A = Position(x3a,y3a);
	Position B = Position(x3b,y3b);
	Position Best;

	if(A.getDistance(me) > B.getDistance(me)){
		Best= B;
	}
	else{ Best= A;}
	//Broodwar->drawCircleMap(x0,y0, 20, Colors::Green, false);
	//Broodwar->drawCircleMap(x1,y1, 20, Colors::Green, false);
	//Broodwar->drawCircleMap(x3a,y3a, 20, Colors::Green, false);
	//Broodwar->drawCircleMap(x3b,y3b, 20, Colors::Green, false);
	//Broodwar->drawCircleMap(me.x(),me.y(), 20, Colors::Green, false);
	//Broodwar->drawLineMap(me.x(), me.y(), x3a, y3a,Colors::Green);
	//Broodwar->drawLineMap(me.x(), me.y(), x3b, y3b,Colors::Green);
	//Broodwar->drawCircleMap(Best.x(),Best.y(), 20, Colors::Red, false);
	return Best;


}
void sort_unitsFirst(set<Unit*> unsorted)
{
	list<Unit*> sorted;
	for(set<Unit*>::iterator unit =unsorted.begin();unit!=unsorted.end();unit++)
	{
		sorted.push_back(*unit);
		isAttacking[*unit] = false;
		record[*unit] = (*unit)->getShields()+(*unit)->getHitPoints();
	}
	sorted.sort(compare_units);
	int gNum=3;
	//if(gameType==0){ gNum = 4;}
	//else{ gNum =3;}
	int n = Broodwar->self()->getUnits().size()/gNum;
	int index=0;
	set<Unit*> groupOne;
	set<Unit*> groupTwo;
	set<Unit*> groupThree;
	set<Unit*> groupFour;
	for(list<Unit*>::const_iterator i=sorted.begin();i!=sorted.end();i++)
    {
       if(index<n){groupOne.insert(*i);}
	   else if(index<(2*n)){groupTwo.insert(*i);}
	   else if(index<(3*n)){groupThree.insert(*i);}
	   else { groupFour.insert(*i);}
	   index++;
    }
	targets[1]=NULL;
	targets[2]=NULL;
	targets[3]=NULL;
	//if(gameType==0){targets[4]=NULL;}
	groups[1] = groupOne;
	groups[2] = groupTwo;
	groups[3] = groupThree;
	lastWander[1] = positions[0];
	lastWander[2] = positions[1];
	lastWander[3] = positions[2];
	//if(gameType==0){groups[4] = groupFour;}
	if(gameType==10){
		Broodwar->printf("Go Find");
		int height = Broodwar->mapHeight() * TILE_SIZE;
		int width = Broodwar->mapWidth() * TILE_SIZE;
		for(set<Unit*>::iterator unit = groupOne.begin();unit!=groupOne.end();unit++){
			if((*unit)->getPosition().x()>(width/2)){
				(*unit)->move(Position(0,height/2));
			}
			else{
			(*unit)->move(Position(width,height/2));
			}
		}
	}
}
void sort_unitsSecond(set<Unit *> units){
	set<Unit *> muta;
	Position mPos = Position(0,0);
	bool mPosSet = false;
	int s = 2;
	for(set<Unit *>::iterator unit = units.begin();unit!=units.end();unit++){
		isAttacking[*unit] = false;
		record[*unit] = (*unit)->getHitPoints();
		int name = (*unit)->getType().getID();
		if(name==43){
			muta.insert(*unit);
			if(!mPosSet){ mPos = (*unit)->getPosition(); mPosSet=true;}
			else{ (*unit)->move(mPos);}
		}
		else{ 
			set<Unit *> scourge;
			scourge.insert(*unit);
			targets[s] = NULL;
			groups[s]=scourge;
			lastWander[s] = positions[s%3];
			s++;
		}
	}
	targets[1] = NULL;
	groups[1] = muta;
	lastWander[1] = positions[0];
}


void GreenBot::onStart()
{
    Broodwar->printf("GreenBot");
	Broodwar->setLocalSpeed(FB_LOCAL_SPEED);
	srand ( time(NULL) ); 

	positions.push_back(Position(WanderStep,WanderStep));
	positions.push_back(Position(-WanderStep,-WanderStep));
	positions.push_back(Position(WanderStep,-WanderStep));
	positions.push_back(Position(-WanderStep,WanderStep));
	positions.push_back(Position(WanderStep,WanderStep));
	positions.push_back(Position(-WanderStep,-WanderStep));
	positions.push_back(Position(WanderStep,-WanderStep));
	positions.push_back(Position(-WanderStep,WanderStep));


	set<Unit*> units = Broodwar->self()->getUnits();
	if(units.size() == 12){ gameType=0;Broodwar->printf("Dragoons, Yay!");}
	else if(units.size() == 9){gameType=1;}
	else { gameType=2;}
	if(gameType == 0 || gameType == 1){ sort_unitsFirst(units);}
	else{ 
		sort_unitsSecond(units);
	}

    //moveToStartPositionsGroup();

}


void GreenBot::onFrame()
{
    /* For each unit go through a state machine
     * And do stuff, fun stuff
     */
	if(gameType == 2 && stackCountDown>0){ stackCountDown--;}
	else{
		for (map<int,Unit *>::iterator group = targets.begin(); group != targets.end(); group++){
					if(!findTarget(group->first)){wander(group->first);}
					else
					{
						set<Unit *> units = groups[group->first];
						for(set<Unit *>::iterator unit = units.begin(); unit != units.end(); unit++){
							if(damaged(*unit)){danceAway(*unit,targets[group->first],400);}
							else{attackTarget(*unit,targets[group->first]);}
						}
					}
		}
	}
	drawMarkings();
}

bool GreenBot::findTarget(int group)
{
	Broodwar->printf("findTarget");
	Unit * currentTarget = targets[group];

	if(gameType==2 && group==1){
			Unit * stackRep=NULL;
			set<Unit*> groupUnits = groups[1];
			for(set<Unit *>::iterator ui = groupUnits.begin();ui!=groupUnits.end();ui++){
				if(*ui !=NULL && (*ui)->exists()){stackRep=(*ui);break;}	
			}
			if(stackRep!=NULL && stackRep->exists()){
				set<Unit*> them = Broodwar->enemy()->getUnits();
				set<Unit*> scourge;
				for(set<Unit*>::iterator unit = them.begin();unit!=them.end();unit++){
					if(*unit != NULL && (*unit)->exists()){
						if((*unit)->getType().getID()!=43){
							if((*unit)->getPosition().getDistance(stackRep->getPosition())<FB_MUTA_WEAPON_RANGE){
								scourge.insert(*unit);
							}
						}
					}
				}
				Unit * newTarget = NULL;
				double distance = numeric_limits<double>::max();
				for(set<Unit*>::iterator unit = scourge.begin();unit!=scourge.end();unit++){
					if (*unit!=NULL && (*unit)->exists() &&(*unit)->getDistance(stackRep) < distance){newTarget=(*unit);}
				}
				if(newTarget!=NULL){
					targets[group]=newTarget;
					Broodwar->printf("Zap Scourge");
					if(newTarget!=currentTarget){
						set<Unit*> units = groups[group];
						for(set<Unit*>::iterator unit = units.begin();unit!=units.end();unit++)
						{
							if(*unit!=NULL && (*unit)->exists()){isAttacking[*unit] = false;}
						}
					}
					return true;
				}
			}
									
	}

	set<Unit*> units = groups[group];
	set<Unit *> them = Broodwar->enemy()->getUnits();
	double distance = numeric_limits<double>::max();
	Unit *closestEnemy = NULL;
	Unit *nextClosestEnemy = NULL;
	Unit *nextNextClosestEnemy = NULL;
    double leastTotalTravelDistance = numeric_limits<double>::max();
	for (set<Unit *>::iterator enemy = them.begin(); enemy != them.end(); enemy++){
		bool untargetted = true;
		if((gameType==2 && group!=1)|| gameType==1){
				bool untargetted = true;
				for(map<int,Unit *>::iterator gp = targets.begin(); gp != targets.end(); gp++){
					if((gp->second)!=NULL && (gp->second)->exists() && group!=1 && (gp->second)==(*enemy)){ 
						set<Unit *> those = groups[gp->first];
						for(set<Unit *>::iterator scThat = those.begin(); scThat != those.end();scThat++){
							if(*scThat != NULL && (*scThat)->exists()){untargetted = false;}
						}
					}
				}
		}
		if(!untargetted){continue;}
		double totalTraveldistance = 0.0;
        for (set<Unit *>::iterator unit = units.begin(); unit != units.end(); unit++){
			if((*unit)!=NULL && (*unit)->exists()){
				double travelDistance;
				if(gameType==0){ travelDistance = (*unit)->getDistance(*enemy) - FB_DRAGOON_WEAPON_RANGE;}
				else if(gameType==2){travelDistance = (*unit)->getDistance(*enemy) - FB_MUTA_WEAPON_RANGE;}
				else if(gameType==1){travelDistance = (*unit)->getDistance(*enemy);}
				if (travelDistance > 0){totalTraveldistance += travelDistance;}
			}
		}
		if (totalTraveldistance <= leastTotalTravelDistance)
		{
				nextNextClosestEnemy = nextClosestEnemy;
				nextClosestEnemy = closestEnemy;
				closestEnemy = *enemy;
				leastTotalTravelDistance = totalTraveldistance;	
		}
	}
	if(closestEnemy!=NULL){
		
		int one=1000;
		int two=1000;
		int three = 1000;
		one = closestEnemy->getHitPoints();
		if(nextClosestEnemy!=NULL){ two = nextClosestEnemy->getHitPoints();}
		//if(nextNextClosestEnemy!=NULL){ three = nextNextClosestEnemy->getHitPoints();}
		if(three<two){ nextClosestEnemy = nextNextClosestEnemy;two=three;}
		if(two<one){ closestEnemy = nextClosestEnemy;}
		targets[group] = closestEnemy;
		if(closestEnemy != currentTarget)
		{
			set<Unit*> units = groups[group];
			for(set<Unit*>::iterator unit = units.begin();unit!=units.end();unit++)
			{
				if(*unit!=NULL && (*unit)->exists()){
					if(gameType==0){danceAway(*unit,closestEnemy,0);}
					if(gameType==2 && group==1){danceAway(*unit,closestEnemy,0);}
					isAttacking[*unit] = false;
				}
			}
		}

		return true;
	}
    return false;
}

void GreenBot::danceAway(Unit * unit, Unit* target, int reach)
{
	Broodwar->printf("danceAway");
	if(unit !=NULL && unit->exists() && target !=NULL && target->exists()){
		if(gameType==0 || (gameType==2 && (unit->getType().getID())==43)){
			int range;
			if(gameType==0){range = FB_DRAGOON_WEAPON_RANGE;}
			else{ range = FB_MUTA_WEAPON_RANGE;}
			int step = reach + range;
			int targetX = target->getPosition().x();
			int targetY = target->getPosition().y();
			if(unit->getDistance(target)<(reach+range)){
				isAttacking[unit] = false;
				//Broodwar->printf("I'm too close! Dance away...");
				set<Unit*> them = Broodwar->enemy()->getUnits();
				Unit * nextClosest=NULL;
				double distance = numeric_limits<double>::max();
				for(set<Unit*>::iterator that = them.begin();that!=them.end();that++){
					if(*that!=NULL && (*that)->exists()){
						double newD = (*that)->getDistance(unit);
						if(*that != target && newD<distance){
							nextClosest = *that;
							distance = newD;
						}
					}
				}
				if(nextClosest!=NULL && nextClosest->exists()){
					Position newPos = closestIntersectionPoint(target->getPosition(),nextClosest->getPosition(),unit->getPosition(),reach);
					unit->move(newPos);
				}
				else{
					if(unit->getPosition().x() > targetX){
						if(unit->getPosition().y() > targetY){unit->move(Position(targetX+step,targetY+step));}
						else{unit->move(Position(targetX+step,targetY-step));}
					}
					else{
						if(unit->getPosition().y() > targetY){unit->move(Position(targetX-step,targetY+step));}
						else{unit->move(Position(targetX-step,targetY-step));}
					}
				}
			}
		}
	}

}
void GreenBot::wander(int group) 
{
	Broodwar->printf("wander");
	Order order = (*groups[group].begin())->getOrder();
	if(order!=Orders::Move){
		set<Unit*> units = groups[group];
		if(gameType==1){
			for (map<int,Unit *>::iterator gp = targets.begin(); gp != targets.end(); gp++){
				for(set<Unit *>::iterator u = groups[gp->first].begin();u!=groups[gp->first].end();u++){
					if(*u != NULL && (*u)->exists()){units.insert(*u);}
				}
			}
		}

			int randSelectOne = rand()%6; 
			int randSelectTwo = rand()%6;
			vector<Position> thesePositions = positions;
			vector<Position>::iterator remove = thesePositions.begin();
			bool found = false;
			for(vector<Position>::iterator post = thesePositions.begin(); post!=thesePositions.end();post++){
				if(*post==lastWander[group]){remove=post;found=true;break;}
			}
			if(found){thesePositions.erase(remove);}
			Position posOne = thesePositions[randSelectOne];
			Position posTwo = thesePositions[randSelectTwo];

			Position newPos;
		double distanceOne = 0;
		double distanceTwo = 0;
		for (map<int,Unit *>::iterator gp = targets.begin(); gp != targets.end(); gp++){
			if(gp->first!=group){
				set<Unit *> unitsInGroup = groups[gp->first];
				for(set<Unit *>::iterator up = unitsInGroup.begin();up!=unitsInGroup.end();up++){
					for(set<Unit*>::iterator unit = units.begin(); unit != units.end();unit++){
						if(*up != NULL && *unit !=NULL && (*up)->exists() && (*unit)->exists()){
							newPos = (*unit)->getPosition()+posOne;
							distanceOne += (*up)->getPosition().getApproxDistance(newPos);
							newPos = (*unit)->getPosition()+posTwo;
							distanceTwo += (*up)->getPosition().getApproxDistance(newPos);
						}
					}
				}
			}
		}
		Position pos;
		if(distanceOne < distanceTwo){pos = posOne;}
		else{ pos = posTwo; }
		lastWander[group]=pos;
		for(set<Unit*>::iterator unit = units.begin(); unit != units.end();unit++){
			if(*unit !=NULL && (*unit)->exists()){
				newPos = (*unit)->getPosition()+pos;
				newPos = newPos.makeValid();
				(*unit)->move(newPos);
			}
		}
	}
}


void GreenBot::attackTarget(Unit* unit, Unit* target)
{
	Broodwar->printf("attack");
	//if(gameType==2 && unit->getType().getID()!=43){Broodwar->printf("A scourge told to attack");}
	//Broodwar->printf("attackTarget");
	if(target != NULL && target->exists()){
			if (unit != NULL && unit->exists()&& !(isAttacking[unit])){
				//Broodwar->printf("shoot");
				//if(gameType==2 && unit->getType().getID()!=43){Broodwar->printf("A scourge shoots");}
				unit->attackUnit(target);
				isAttacking[unit] = true;
			}
	}
}
bool GreenBot::damaged(Unit* unit)
{
	Broodwar->printf("damaged");
	//Broodwar->printf("%d",unit->getShields());
	if (gameType==0 && unit != NULL && unit->exists()){
			int diff = (record[unit]-(unit->getShields()+unit->getHitPoints()));
			//Broodwar->printf("%d",diff);
			if(diff>20){
				record[unit] = (unit->getShields()+unit->getHitPoints());
				Position p = unit->getPosition();
				Broodwar->drawBoxMap(p.x() - 5, p.y() - 5, p.x() + 5, p.y() + 5, Colors::Red, true);
				return true;
			}
		}
		if (gameType==2 && unit != NULL && unit->exists()){
			if(((unit->getType().getID())==43)){
				int diff = (record[unit]-(unit->getHitPoints()));
				//Broodwar->printf("%d",diff);
				if(diff>20){
					record[unit] = unit->getHitPoints();
					Position p = unit->getPosition();
					Broodwar->drawBoxMap(p.x() - 5, p.y() - 5, p.x() + 5, p.y() + 5, Colors::Red, true);
					return true;
				}
			}
		}
	return false;
}


void GreenBot::moveToStartPositionsGroup(){

	Position offSetOne (500,-200);
	Position offSetTwo (200,0);
	Position offSetThree (500,200);
	for(std::set<Unit*>::const_iterator i=groups[1].begin();i!=groups[1].end();i++)
    {
       (*i)->move(((*i)->getPosition())+offSetOne);
    }
	for(std::set<Unit*>::const_iterator i=groups[2].begin();i!=groups[2].end();i++)
    {
       (*i)->move(((*i)->getPosition())+offSetTwo);
    }
	for(std::set<Unit*>::const_iterator i=groups[3].begin();i!=groups[3].end();i++)
    {
       (*i)->move(((*i)->getPosition())+offSetThree);
    }

}

void GreenBot::moveToStartPositions()
{
	// There are 12 dragoons
	// the 12 start locations are 5 points along the left and bottom edge, the far corner and the centre
	// Starting with the furtherst points find the closest dragoon and move that one there
	// remove that dragoon from consideration for futher points

	// create a list of positions
	list<Position> positions;
	int height = Broodwar->mapHeight() * TILE_SIZE;
	int width = Broodwar->mapWidth() * TILE_SIZE;
	int step = height/6;
	int start = 2*step;
	for(int i=0;i<3;i++){
		Position pos (width/2, start+(i*step));
		positions.push_back(pos); 
		positions.push_back(pos); 
		positions.push_back(pos); 
		positions.push_back(pos); 
	}

    // For each Position find the unit closest to that position and move the unit there
	set<Unit *> units (Broodwar->self()->getUnits());
	for(list<Position>::iterator pos = positions.begin(); pos != positions.end(); pos++)
	{
		Unit* choice = NULL;
		double distance = numeric_limits<double>::max();
		for (set<Unit *>::iterator unit = units.begin(); unit != units.end(); unit++)
		{
			double newDistance = (*unit)->getPosition().getDistance(*pos);
			bool moving = (*unit)->isMoving();
			if(newDistance<distance)
			{
				if(!moving)
				{
					choice = *unit;
					distance = newDistance;
				}
				
			}
		}
		if(choice!=NULL){ choice->move(*pos);}
		else{ Broodwar->printf("No Choice!");}
		units.erase(choice);
	}
}

void GreenBot::drawMarkings()
{
	Broodwar->printf("drawMarkings");
	//Unit * unit=NULL;
	//for(set<Unit *>::const_iterator u = Broodwar->self()->getUnits().begin();u!=Broodwar->self()->getUnits().end();u++){
	//	if(*u !=NULL && (*u)->exists()){ unit = *u; break;}
	//}
	//Unit * target = NULL;
	//if(unit!=NULL){Unit * target = unit->getTarget();}
	//if(target!=NULL){
	//	set<Unit*> them = Broodwar->enemy()->getUnits();
	//				Unit * nextClosest=NULL;
	//				double distance = numeric_limits<double>::max();
	//				for(set<Unit*>::iterator that = them.begin();that!=them.end();that++){
	//					double newD = (*that)->getDistance(unit);
	//					if(*that != target && newD<distance){
	//						nextClosest = *that;
	//						distance = newD;
	//					}
	//				}
	//				if(nextClosest!=NULL){
	//					closestIntersectionPoint(target->getPosition(),nextClosest->getPosition(),unit->getPosition(),0);
	//				}
	//}

	for (map<int,Unit *>::iterator group = targets.begin(); group != targets.end(); group++){
		if( group->second != NULL && group->second->exists()){
			set<Unit *> units = groups[group->first];
			Position pYou = group->second->getPosition();
			for(set<Unit*>::iterator unit = units.begin(); unit!=units.end();unit++){
				if ((*unit) != NULL && (*unit)->exists())
				{
					// Mark target on map.
					Position pMe = (*unit)->getPosition();
					Broodwar->drawLineMap(pMe.x(), pMe.y(), pYou.x(), pYou.y(),Colors::Green);
				}
			}
		}
	}
}

void GreenBot::onUnitDiscover(Unit* discoveredUnit) {
	if(discoveredUnit->getPlayer()!=Broodwar->self() && !GameOn){ 
		//Broodwar->printf("Game On!!");
		//GameOn = true;
	}
}

void GreenBot::onUnitDestroy(Unit* unit) {
	for(map<int,Unit*>::iterator group = targets.begin();group!=targets.end();group++){
		if(unit == group->second){
			set<Unit*> units = groups[group->first];
			for(set<Unit*>::iterator unit = units.begin();unit!=units.end();unit++)
			{
				isAttacking[*unit] = false;
			}
		}
	}

}



// Callback stubs
void GreenBot::onEnd(bool isWinner) {}
void GreenBot::onNukeDetect(Position target) {}
void GreenBot::onPlayerLeft(Player* player) {}
void GreenBot::onReceiveText(Player* player, string text) {}
void GreenBot::onSaveGame(string gameName) {}
void GreenBot::onSendText(string text) {}
void GreenBot::onUnitCreate(Unit* unit) {}

void GreenBot::onUnitEvade(Unit* unit) {}
void GreenBot::onUnitHide(Unit* unit) {}
void GreenBot::onUnitMorph(Unit* unit) {}
void GreenBot::onUnitRenegade(Unit* unit) {}
void GreenBot::onUnitShow(Unit* unit) {}
