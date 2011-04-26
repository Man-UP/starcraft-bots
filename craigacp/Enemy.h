#pragma once
#define NOMINMAX
#include <math.h>
#include <BWAPI.h>
#include <BWTA.h>

using namespace std;
using namespace BWAPI;

class EnemyUnit
{
    public:
        Unit *dude;
        int lastKnownHealth;
        Position lastKnownPosition;
        bool visible;
        Unit *lastKnownTarget;
        bool dead;
        int targetingGroup;
        bool operator<(const EnemyUnit second) const
        {
          return dude < second.dude;
        }
};

class Enemy
{
    public:
        Enemy();
        
        //methods
        void onFrame();        
        Unit *getClosestEnemy(Position us);
        Unit *getClosestValidEnemy(Position us, int groupNo);
        Unit *getWeakestEnemy();
        
        void setGroupNo(Unit *dude, int groupNo);

        void addEnemy(Unit* newDude);
        EnemyUnit *findEnemy(Unit *enemyDude);
        int numVisibleEnemies();
        set<EnemyUnit *> getSet() {
          return enemySet;
        }
        
    private:
        void updateEnemy(EnemyUnit *dude);
        
        //variables
        set<EnemyUnit *> enemySet;
        
};//class Enemy
