#include "Enemy.h"

using namespace std;
using namespace BWAPI;

Enemy::Enemy()
{
  enemySet = set<EnemyUnit *>();
}//constructor


//public methods
void Enemy::onFrame()
{
    for (set<EnemyUnit *>::iterator i = enemySet.begin(); i != enemySet.end(); i++)
    {
        updateEnemy(*i);
    }
}//onFrame()

Unit* Enemy::getClosestEnemy(Position ourDudePos)
{
    Unit *enemyDude = NULL;
    double curDist = 1000000.0;
    for (set<EnemyUnit *>::iterator i = enemySet.begin(); i != enemySet.end(); i++)
    {
      if (!(*i)->dead)
      {
        double newDist = (*i)->lastKnownPosition.getDistance(ourDudePos);
        if (newDist < curDist)
        {
            curDist = newDist;
            enemyDude = (*i)->dude;
        }
      }
    }

    return enemyDude;
}//getClosestEnemy()

Unit* Enemy::getClosestValidEnemy(Position ourDudePos, int groupNo)
{
    Unit *enemyDude = NULL;
    double curDist = 1000000.0;
    for (set<EnemyUnit *>::iterator i = enemySet.begin(); i != enemySet.end(); i++)
    {
      if (!(*i)->dead)
      {
        double newDist = (*i)->lastKnownPosition.getDistance(ourDudePos);
        if ((newDist < curDist) && (*i)->targetingGroup == -1)
        {
            curDist = newDist;
            enemyDude = (*i)->dude;
        }
      }
    }

    return enemyDude;
}//getClosestValidEnemy()

Unit* Enemy::getWeakestEnemy()
{
    Unit *enemyDude = NULL;
    int curHealth = 1000;
    for (set<EnemyUnit *>::iterator i = enemySet.begin(); i != enemySet.end(); i++)
    {
      if (!(*i)->dead)
      {
        int newHealth = (*i)->lastKnownHealth;
        if (newHealth < curHealth)
        {
          curHealth = newHealth;
          enemyDude = (*i)->dude;
        }
      }
    }

    return enemyDude;
}//getWeakestEnemy()


void Enemy::setGroupNo(Unit *dude, int groupNo)
{
  EnemyUnit *foundDude = findEnemy(dude);
  if (foundDude != NULL)
  {
    foundDude->targetingGroup = groupNo;
  }
}


int Enemy::numVisibleEnemies()
{
    int counter = 0;
    for (set<EnemyUnit *>::iterator i = enemySet.begin(); i != enemySet.end(); i++)
    {
      if (!(*i)->dead)
      {
        counter++;
      }
    }

    return counter;
}

void Enemy::addEnemy(Unit* newDude)
{
    bool found = false;
    
    for (set<EnemyUnit *>::iterator i = enemySet.begin(); i != enemySet.end(); i++)
    {
        if ((*i)->dude == newDude)
        {
            //found = true;
            return;
        }
    }
    
    if (!found)
    {
        //class EnemyUnit
        //{
        //public:
        //    Unit *dude;
        //    int lastKnownHealth;
        //    Position lastKnownPosition;
        //    bool visible;
        //    Unit *lastKnownTarget;
        //};
        EnemyUnit *newEnemy = new EnemyUnit();
        newEnemy->dude = newDude;
        newEnemy->lastKnownHealth = newDude->getHitPoints();
        newEnemy->visible = true;
        newEnemy->dead = false;
        newEnemy->targetingGroup = -1;
        newEnemy->lastKnownTarget = newDude->getTarget();
        newEnemy->lastKnownPosition = newDude->getPosition();
        enemySet.insert(newEnemy);
    }
}//addEnemy

EnemyUnit *Enemy::findEnemy(Unit *enemyDude)
{
    for (set<EnemyUnit *>::iterator i = enemySet.begin(); i != enemySet.end(); i++)
    {
        if ((*i)->dude == enemyDude)
        {
            return *i;
        }
    }
    
    return NULL;
}//findEnemy(Unit *enemyDude)

//private methods
void Enemy::updateEnemy(EnemyUnit *dude)
{
    dude->visible = dude->dude->isVisible();
    if (dude->visible)
    {
        dude->lastKnownHealth = dude->dude->getHitPoints();
        dude->lastKnownPosition = dude->dude->getPosition();
        if (!(dude->dude->exists()))
        {
          dude->dead = true;
        }
    }
}//updateEnemy()

