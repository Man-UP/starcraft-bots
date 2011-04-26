#include "Group.h"
#define PI 3.14159265
#define TAU 2*PI
using namespace std;
using namespace BWAPI;

Group::Group() {
  target = NULL;
  centroid = Position(0,0);
  isAttacking = false;
  formation = FormMess;
  formation_parameter = 3 * PI / 4;

  //dragoonRange = WeaponTypes::Phase_Disruptor.maxRange();
  //zealotRange = UnitTypes::Protoss_Zealot.groundWeapon().maxRange();
  //mutaliskRange = UnitTypes::Zerg_Mutalisk.groundWeapon().maxRange();
  //scourgeRange = UnitTypes::Zerg_Scourge.groundWeapon().maxRange();
  //switch (groupUnit)
  //{
  //case UnitTypes::Protoss_Dragoon:
  //  groupRange = dragoonRange;
  //  break;
  //case UnitTypes::Zerg_Mutalisk:
  //  groupRange = mutaliskRange;
  //  break;
  //case UnitTypes::Protoss_Zealot:
  //  groupRange = zealotRange;
  //  break;
  //case UnitTypes::Zerg_Scourge:
  //  groupRange = scourgeRange;
  //  break;
  //}

}

void Group::addDude(Unit* dude, DudeInfo info) {
  groupDudes.insert(dude);
  dudeInfoMap[dude] = info;
  
  groupRange = dude->getType().groundWeapon().maxRange();
}

void Group::attack(Unit* new_target) {

  if (new_target)
  {
    target = new_target;
    group_state = Attack;
    enemyClass.setGroupNo(target,1); //FIXME SERIOUSLY
    for (set<Unit*>::iterator dude = groupDudes.begin(); dude != groupDudes.end(); dude++) {
      dudeInfoMap[*dude].state = DudeStartAttack;
      dudeInfoMap[*dude].target = target;
    }
    if (game_type != Zealots) ;
      //planAttack(target);
  }
}

void Group::move(Position new_destination) {

  destination = new_destination;
  group_state = Move;

}

void Group::onFrame() {

  updateCentroid();

  for (set<Position>::iterator circle = circles.begin(); circle != circles.end(); circle++) {
    Broodwar->drawCircleMap((*circle).x(), (*circle).y(), 20, Color(0, 0, 255), true);
  }

  circles.clear();

  if (target != NULL) {
    Broodwar->drawCircleMap(target->getPosition().x(), target->getPosition().y(), 20, Color(255, 0, 0), false);
    if (game_type != Zealots && Broodwar->getFrameCount() % 24 == 0) ;
      //planAttack(target);
  }

  switch (group_state) {
    case Move:
      formation_move();
      break;
    case Attack:
      Position target_pos = target->getPosition();

      for (set<Unit *>::iterator dude = groupDudes.begin(); dude != groupDudes.end(); dude++) {
        DudeInfo info = dudeInfoMap[(*dude)];
        double distance_to_target = target_pos.getDistance((*dude)->getPosition());
        switch (info.state) {
          case DudeStartAttack:
            (*dude)->attackUnit(target);
            dudeInfoMap[*dude].state = DudeAttacking;
            break;
          case DudeAttacking:
            break;
          case DudeMoving:
            if (info.moveTarget.getDistance((*dude)->getPosition()) < 32) {
              (*dude)->attackUnit(target);
              dudeInfoMap[*dude].state = DudeAttacking;
            } else {
              (*dude)->move(info.moveTarget);
              Broodwar->drawLineMap((*dude)->getPosition().x(), (*dude)->getPosition().y(), info.moveTarget.x(), info.moveTarget.y(), Color(0, 255, 0));
            }
            break;
        }
      }
      break;
  }

}

void Group::setFormation(Formation new_formation, double parameter) {
  formation = new_formation;
  formation_parameter = parameter;
}

void Group::formation_move() {
  
  switch (formation) {
    case FormMess:
      for (set<Unit *>::iterator dude = groupDudes.begin(); dude != groupDudes.end(); dude++) {
        dudeInfoMap[*dude].moveTarget = destination;
      }
      break;
    case FormLine:
      int group_size = groupDudes.size();
      int xDelta = (int)floor(((destination.x() - centroid.x()) * 70) / destination.getDistance(centroid));
      int yDelta = (int)floor(((destination.y() - centroid.y()) * 70) / destination.getDistance(centroid));
      Position start_spot = Position(centroid.x() + xDelta - (int)(floor(group_size * 32 * cos(formation_parameter))), centroid.y() + yDelta - (int)floor(group_size * 32 * sin(formation_parameter)));

      //circles.insert(start_spot);
      int i = 0;
      for (set<Unit *>::iterator dude = groupDudes.begin(); dude != groupDudes.end(); dude++) {
        Position next_unit = Position(start_spot.x() + (int)(floor(i * 64 * cos(formation_parameter))), start_spot.y() + (int)floor(i * 64 * sin(formation_parameter)));
        dudeInfoMap[*dude].moveTarget = next_unit;
        //circles.insert(next_unit);
        i++;
      }
      break;
  }
  for (set<Unit *>::iterator dude = groupDudes.begin(); dude != groupDudes.end(); dude++) {
    Position dest = dudeInfoMap[*dude].moveTarget;
    (*dude)->move(dest);
    Broodwar->drawLineMap((*dude)->getPosition().x(), (*dude)->getPosition().y(), dest.x(), dest.y(), Color(0, 255, 0));
  }
}

void Group::updateCentroid()
{
  int xSum = 0;
  int ySum = 0;
  int counter = 0;
  for (set<Unit *>::iterator dude = groupDudes.begin(); dude != groupDudes.end(); dude++) 
  {
      if ((*dude)->exists())
      {
        xSum += (*dude)->getPosition().x();
        ySum += (*dude)->getPosition().y();
        counter++;
      }
  }
  if (counter)
  {
    int xMean = xSum / counter;
    int yMean = ySum / counter;

    centroid = Position(xMean,yMean);
  }
}

void Group::planAttack(Unit *target)
{
    //resolution - 10px x 10px
    //grouprange in px
    int res = 32;

    Position centrePosition = target->getPosition();
    Position topLeft = centrePosition + Position(-groupRange, -groupRange);
    
    int matrixSize = 2 * groupRange / res + 1;
    int **threatMatrix = new int*[matrixSize];
    bool **occupiedMatrix = new bool*[matrixSize];

     for (int i = 0 ;i< matrixSize ; i++) 
      {
          threatMatrix[i] = new int[matrixSize];
          occupiedMatrix[i] = new bool[matrixSize];
          for(int j=0; j< matrixSize; j++)   
        {
            threatMatrix[i][j] = 0;
            occupiedMatrix[i][j] = false;
        } 
            
    }    
    int matrixCentre = matrixSize/2 +1;
    
    threatMatrix[matrixCentre][matrixCentre] = 1000;
    occupiedMatrix[matrixCentre][matrixCentre] = true;
    
    //setup done

    set<EnemyUnit *> enemySet = enemyClass.getSet();
   
    for (set<EnemyUnit *>::iterator enemyItr = enemySet.begin(); enemyItr != enemySet.end(); enemyItr++)
    {
        Unit * enemyDude = (*enemyItr)->dude;
        
        if (!(*enemyItr)->dead)
        {
            Position currentEnemyPos = enemyDude->getPosition();
            int currentEnemyDamage = enemyDude->getType().groundWeapon().damageAmount();
            int currentEnemyRange = enemyDude->getType().groundWeapon().maxRange();
            for (int i = 0; i < matrixSize; i++)
            {
                for (int j = 0; j < matrixSize; j++)
                {
                    Position currentMatrixPos = Position(topLeft) + Position(i*res,j*res);
                    if (currentMatrixPos.getDistance(currentEnemyPos) <= currentEnemyRange)
                    {
                        threatMatrix[i][j] += currentEnemyDamage;
                    }//in range
                }//for j
            }//for i
            int enemyI = (currentEnemyPos.x() - topLeft.x())/res;
            int enemyJ = (currentEnemyPos.y() - topLeft.y())/res;
            if ((enemyI < matrixSize) && (enemyJ < matrixSize) && (enemyI >= 0) && (enemyJ >=0))
              occupiedMatrix[enemyI][enemyJ] = true; 
        }//if not dead
    }//iter over enemies, fill threat matrix
    
    for (set<Unit *>::iterator dudeItr = groupDudes.begin(); dudeItr != groupDudes.end(); dudeItr++)
    {
        double bestScore = numeric_limits<double>::max();
        int bestI = 0;
        int bestJ = 0;
        Unit * dude = *dudeItr;
        
        for (int i = 0; i < matrixSize; i++)
        {
            for (int j = 0; j < matrixSize; j++)
            {
                if (!occupiedMatrix[i][j]) 
                {
                    Position currentMatrixPos = Position(topLeft) + Position(i*res,j*res);
                    double currentScore = positionScore(threatMatrix[i][j],dude->getPosition().getDistance(currentMatrixPos),dude);
                    if (currentScore < bestScore)
                    {
                        bestScore = currentScore;
                        bestI = i;
                        bestJ = j;
                    }
                }           
            }//for j
        }//for i
        occupiedMatrix[bestI][bestJ] = true;
        dudeInfoMap[dude].moveTarget = Position(topLeft) + Position(bestI*res + res / 2,bestJ*res + res / 2);
        dudeInfoMap[dude].state = DudeMoving;
    }

    for (int i = 0; i < matrixSize; i++)
        {
            for (int j = 0; j < matrixSize; j++)
            {
                if (occupiedMatrix[i][j]) 
                {
                  Broodwar->drawBoxMap(topLeft.x() + i*res, topLeft.y() + j*res, topLeft.x() + (i+1) * res, topLeft.y() + (i+1) * res, Color(255, 0, 0));
                } else {
                  Broodwar->drawBoxMap(topLeft.x() + i*res, topLeft.y() + j*res, topLeft.x() + (i+1) * res, topLeft.y() + (i+1) * res, Color(0, 255, 0));
                }
            }//for j
        }//for i

    for (int i = 0 ;i< matrixSize ; i++) 
    {
          delete[] threatMatrix[i];
          delete[] occupiedMatrix[i];
    }

    delete[] threatMatrix;
    delete[] occupiedMatrix;
}

double Group::positionScore(int squareThreat, double distance, Unit *dude)
{
    double speed = dude->getType().topSpeed();
    int health = dude->getHitPoints();
    int shield = dude->getShields();
    
    double time = distance / speed;
    double hits = (health + shield) / (double) squareThreat;
    
    return time / hits;
}//positionScore
